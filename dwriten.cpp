#include <hdf5.h>
#include <mpi.h>

#include <string>
#include <vector>

#include "dwrite_eval_util.hpp"
#include "dwrite_eval.hpp"

herr_t dwrite_eval_dwrite_n (MPI_Comm comm,
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
    hid_t fid;
    hid_t dxplid;
    std::vector<hid_t> dids;
    std::vector<hid_t> dsids;
    std::vector<char *> bufs;
    std::vector<hsize_t **> starts;
    std::vector<hsize_t **> counts;
    hsize_t **start;
    hsize_t **count;
    int *len;
    int *off;
    MPI_Offset *dim;
    int *buf;

    TIMER_START

    err = dwrite_eval_init (comm, out_dir_path + "/dwriten.h5", nvar, ndecom, dims, fid, dids, dsids);
    CHECK_ERR

    err = dwrite_eval_balloc (nvar, ndecom, dims, nreqs, lens, bufs);
    CHECK_ERR

    TIMER_START

    dxplid = H5Pcreate (H5P_DATASET_XFER);
    CHECK_HID (dxplid)
    err = H5Pset_dxpl_mpio (dxplid, H5FD_MPIO_COLLECTIVE);
    CHECK_ERR

    for (i = 0; i < ndecom; i++) {
        start    = (hsize_t **)malloc (sizeof (hsize_t *) * nreqs[i] * 2);
        count    = start + nreqs[i];
        start[0] = (hsize_t *)malloc (sizeof (hsize_t) * nreqs[i] * 2);
        count[0] = start[0] + nreqs[i];
        start[0][0] = (hsize_t)offs[i][0];
        count[0][0] = (hsize_t)lens[i][0];

        for (j = 1; j < nreqs[i]; j++) {
            start[j]    = start[j - 1] + 1;
            count[j]    = count[j - 1] + 1;
            start[j][0] = (hsize_t)offs[i][j];
            count[j][0] = (hsize_t)lens[i][j];
        }

        starts.push_back (start);
        counts.push_back (count);
    }

    for (i = 0; i < nvar; i++) {
        err = H5Dwrite_n (dids[i], H5T_NATIVE_INT, nreqs[i % ndecom], starts[i%ndecom], counts[i%ndecom], dxplid, bufs[i]);
    }

    for (i = 0; i < ndecom; i++) {
        free(starts[i][0]);
        free(starts[i]);
    }
    TIMER_STOP (DWRITE_EVAL_TIMER_DWRITEN_WR)

    err = dwrite_eval_fin (fid, dids, dsids, bufs);
    CHECK_ERR

err_out:;
    TIMER_STOP (DWRITE_EVAL_TIMER_DWRITEN)
    return err;
}