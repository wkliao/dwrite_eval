#include <hdf5.h>
#include <mpi.h>

#include <string>
#include <vector>

#include "dwrite_eval_util.hpp"
#include "dwrite_eval.hpp"

herr_t dwrite_eval_dwrite (MPI_Comm comm,
                    std::string &out_dir_path,
                    int nvar,
                    int ndecom,
                    MPI_Offset dims[][2],
                    int nreqs[],
                    int *offs[],
                    int *lens[]) {
    herr_t err = 0;
    int i, j;
    int nreq;
    hid_t did;
    hid_t dsid;
    hid_t msid;
    hid_t fid;
    hid_t dxplid;
    std::vector<hid_t> dids;
    std::vector<hid_t> dsids;
    std::vector<char *> bufs;
    hsize_t start[1];
    hsize_t count[1];
    hsize_t one[1] = {1};
    hsize_t msize;
    int *len;
    int *off;
    MPI_Offset *dim;
    int *buf;

    TIMER_START

    err = dwrite_eval_init (comm, out_dir_path + "/dwrite.h5", nvar, ndecom, dims, fid, dids, dsids);
    CHECK_ERR

    err = dwrite_eval_balloc (nvar, ndecom, dims, nreqs, lens, bufs);
    CHECK_ERR

    TIMER_STOP (DWRITE_EVAL_TIMER_DWRITE)
    MPI_Barrier(comm);
    TIMER_START

    dxplid = H5Pcreate (H5P_DATASET_XFER);
    CHECK_HID (dxplid)
    err = H5Pset_dxpl_mpio (dxplid, H5FD_MPIO_COLLECTIVE);
    CHECK_ERR

    // Memory space
    count[0] = INT_MAX;  // Log VOL can only handle 2 GiB at a time
    msid     = H5Screate_simple (1, count, count);
    CHECK_HID (msid)

    for (i = 0; i < nvar; i++) {
        buf  = (int *)bufs[i];
        dsid = dsids[i % ndecom];
        did  = dids[i];
        len  = lens[i % ndecom];
        off  = offs[i % ndecom];
        dim  = dims[i % ndecom];

        err = H5Sselect_none (dsid);
        CHECK_ERR

        msize = 0;
        for (j = 0; j < nreqs[i % ndecom]; j++) {
            start[0] = off[j];
            count[0] = len[j];

            err = H5Sselect_hyperslab (dsid, H5S_SELECT_OR, start, NULL, one, count);
            CHECK_ERR

            msize += count[0];
        }

        start[0] = 0;
        err      = H5Sselect_hyperslab (msid, H5S_SELECT_SET, start, NULL, one, &msize);
        CHECK_ERR

        TIMER_START
        err = H5Dwrite (did, H5T_NATIVE_INT, msid, dsid, dxplid, buf);
        TIMER_STOP (DWRITE_EVAL_TIMER_DWRITE_WR)
    }
    err = H5Sclose (msid);
    CHECK_ERR

    err = H5Pclose (dxplid);
    CHECK_ERR

    TIMER_STOP (DWRITE_EVAL_TIMER_DWRITE_POST)
    MPI_Barrier(comm);
    TIMER_START
    
    err = dwrite_eval_fin (fid, dids, dsids, bufs);
    CHECK_ERR

err_out:;
    TIMER_STOP (DWRITE_EVAL_TIMER_DWRITE)
    return err;
}