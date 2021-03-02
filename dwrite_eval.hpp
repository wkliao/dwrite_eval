/*********************************************************************
 *
 * Copyright (C) 2021, Northwestern University
 * See COPYRIGHT notice in top-level directory.
 *
 * This program dwrite_evalnstrates the performance different of
 * H5Dwrite and H5Drwite_N in log-io-vol.
 *
 * See README.md for compile and run instructions.
 *
 *********************************************************************/

#pragma once

#include <hdf5.h>
#include <mpi.h>

#include <string>

#define MAX_NUM_DECOMP 6

#define DWRITE_EVAL_TIMER_ALL        0
#define DWRITE_EVAL_TIMER_NDWRITE    1
#define DWRITE_EVAL_TIMER_NDWRITE_WR 2
#define DWRITE_EVAL_TIMER_DWRITE     3
#define DWRITE_EVAL_TIMER_DWRITE_WR  4
#define DWRITE_EVAL_TIMER_DWRITEN    5
#define DWRITE_EVAL_TIMER_DWRITEN_WR 6
#define DWRITE_EVAL_NTIMER           7

#define CHECK_NCERR                                                                      \
    {                                                                                    \
        if (ncerr != NC_NOERR) {                                                         \
            printf ("Error in %s:%d: %s\n", __FILE__, __LINE__, ncmpi_strerror (ncerr)); \
            nerrs++;                                                                     \
            goto err_out;                                                                \
        }                                                                                \
    }

#define CHECK_HID(A)                                          \
    {                                                         \
        if (A < 0) {                                          \
            printf ("Error in %s:%d:\n", __FILE__, __LINE__); \
            H5Eprint1 (stderr);                               \
            goto err_out;                                     \
        }                                                     \
    }

#define CHECK_ERR CHECK_HID (err)

#define CHECK_PTR(P)                                              \
    {                                                             \
        if (P == NULL) {                                          \
            printf ("Error in %s:%d: ptr\n", __FILE__, __LINE__); \
            goto err_out;                                         \
        }                                                         \
    }

#define TIMER_START          \
    {                        \
        double tstart, tend; \
        tstart = MPI_Wtime ();

#define TIMER_STOP(A)                         \
    tend = MPI_Wtime ();                      \
    dwrite_eval_profile_add_time (A, tend - tstart); \
    }

void dwrite_eval_profile_print ();
void dwrite_eval_profile_reset ();
void dwrite_eval_profile_add_time (int id, double t);

herr_t read_decomp (
    MPI_Comm io_comm,     /* MPI communicator that includes all the tasks involved in IO */
    const char *infname,  /* IN */
    int *num_decomp,      /* OUT */
    MPI_Offset dims[][2], /* OUT */
    int contig_nreqs[],   /* OUT */
    int *disps[],         /* OUT: to be freed by caller */
    int *blocklens[],
    bool verbose); /* OUT: to be freed by caller */

herr_t dwrite_eval_ndwrite (MPI_Comm comm,
                     std::string &out_dir_path,
                     int nvar,
                     int ndecom,
                     MPI_Offset dims[][2],
                     int nreqs[],
                     int *offs[],
                     int *lens[]);

herr_t dwrite_eval_dwrite (MPI_Comm comm,
                     std::string &out_dir_path,
                     int nvar,
                     int ndecom,
                     MPI_Offset dims[][2],
                     int nreqs[],
                     int *offs[],
                     int *lens[]);

herr_t dwrite_eval_dwrite_n (MPI_Comm comm,
                     std::string &out_dir_path,
                     int nvar,
                     int ndecom,
                     MPI_Offset dims[][2],
                     int nreqs[],
                     int *offs[],
                     int *lens[]);