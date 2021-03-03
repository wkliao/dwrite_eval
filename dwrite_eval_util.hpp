#pragma once

#include <H5VL_log.h>
#include <hdf5.h>

#include <string>
#include <vector>

#include "dwrite_eval.hpp"

inline herr_t dwrite_eval_balloc (int nvar,
                           int ndecom,
                           MPI_Offset dims[][2],
                           int nreqs[],
                           int *lens[],
                           std::vector<char *> &bufs) {
    herr_t err = 0;
    int i, j;
    std::vector<size_t> bsizes;
    size_t bsize, esize;
    char *buf;

    for (i = 0; i < ndecom; i++) {
        bsize = 0;
        for (j = 0; j < nreqs[i]; j++) { bsize += lens[i][j]; }
        bsize *= sizeof (int);
        bsizes.push_back (bsize);
    }
    for (i = 0; i < nvar; i++) {
        buf = (char *)malloc (bsizes[i % ndecom]);
        CHECK_PTR (buf)
        bufs.push_back (buf);
    }

err_out:;
    return err;
}

inline herr_t dwrite_eval_init (MPI_Comm comm,
                         std::string path,
                         int nvar,
                         int ndecomp,
                         MPI_Offset dims[][2],
                         hid_t &fid,
                         std::vector<hid_t> &dids,
                         std::vector<hid_t> &dsids) {
    herr_t err     = 0;
    hid_t log_vlid = -1;
    hid_t faplid   = -1;
    hid_t did      = -1;
    hid_t dsid     = -1;
    hsize_t dim[2];
    int i;

    // Register LOG VOL plugin
    log_vlid = H5VLregister_connector (&H5VL_log_g, H5P_DEFAULT);

    faplid = H5Pcreate (H5P_FILE_ACCESS);
    // MPI and collective metadata is required by LOG VOL
    H5Pset_fapl_mpio (faplid, MPI_COMM_WORLD, MPI_INFO_NULL);
    H5Pset_all_coll_metadata_ops (faplid, 1);
    H5Pset_vol (faplid, log_vlid, NULL);

    // Create file
    fid = H5Fcreate (path.c_str (), H5F_ACC_TRUNC, H5P_DEFAULT, faplid);
    CHECK_HID (fid)

    for (i = 0; i < ndecomp; i++) {
        dim[0] = (hsize_t)dims[i][0];
        // dim[1] = 1;
        dsid = H5Screate_simple (1, dim, dim);
        CHECK_HID (dsid)

        dsids.push_back (dsid);
    }
    for (i = 0; i < nvar; i++) {
        char dname[128];
        sprintf (dname, "D_%d", i);
        did = H5Dcreate2 (fid, dname, H5T_STD_I32LE, dsids[i % ndecomp], H5P_DEFAULT, H5P_DEFAULT,
                          H5P_DEFAULT);
        CHECK_HID (did)

        dids.push_back (did);
    }

err_out:;
    if (faplid >= 0) { H5Pclose (faplid); }
    if (err) {
        if (dsid >= 0) { H5Sclose (dsid); }
        if (did >= 0) { H5Dclose (did); }
    }
    return err;
}

inline herr_t dwrite_eval_fin (hid_t &fid,
                        std::vector<hid_t> &dids,
                        std::vector<hid_t> &dsids,
                        std::vector<char *> &bufs) {
    for (auto &dsid : dsids) { H5Sclose (dsid); }
    for (auto &did : dids) { H5Dclose (did); }
    for (auto &buf : bufs) { free (buf); }
    H5Fclose (fid);
    return 0;
}