#include <hdf5.h>
#include <mpi.h>

#include <string>
#include <vector>

#include "demo_util.hpp"
#include "dwriten_demo.hpp"

herr_t demo_ndwrite (MPI_Comm comm,
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
    hsize_t esize;
    int *len;
    int *off;
    MPI_Offset *dim;
    int *buf;

    TIMER_START

    err = demo_init (comm, out_dir_path + "/ndwrite.h5", nvar, ndecom, dims, fid, dids, dsids);
    CHECK_ERR

    err = demo_balloc (nvar, ndecom, dims, nreqs, lens, bufs);
    CHECK_ERR

    TIMER_START

    dxplid = H5Pcreate (H5P_DATASET_XFER);
    CHECK_HID (dxplid)
    err = H5Pset_dxpl_mpio (dxplid, H5FD_MPIO_COLLECTIVE);
    CHECK_ERR

    // Memory space
    count[0] = INT_MAX - 1;  // Max 2G block
    msid     = H5Screate_simple (1, count, count);
    CHECK_HID (msid)

    for (i = 0; i < nvar; i++) {
        buf  = (int *)bufs[i];
        dsid = dsids[i % ndecom];
        did  = dids[i];
        len  = lens[i % ndecom];
        off  = offs[i % ndecom];
        dim  = dims[i % ndecom];

        for (j = 0; j < nreqs[i % ndecom]; j++) {
            start[0] = off[j];
            count[0] = len[j];

            err = H5Sselect_hyperslab (dsid, H5S_SELECT_SET, start, NULL, one, count);
            CHECK_ERR

            start[0] = 0;
            err      = H5Sselect_hyperslab (msid, H5S_SELECT_SET, start, NULL, one, count);
            CHECK_ERR

            err = H5Dwrite (did, H5T_NATIVE_INT, msid, dsid, dxplid, buf);
            buf += count[0];
        }
    }
    err = H5Sclose (msid);
    CHECK_ERR

    err = H5Pclose (dxplid);
    CHECK_ERR

    TIMER_STOP (DEMO_TIMER_NDWRITE_WR)

    err = demo_fin (fid, dids, dsids, bufs);
    CHECK_ERR

err_out:;
    TIMER_STOP (DEMO_TIMER_NDWRITE)
    return err;
}