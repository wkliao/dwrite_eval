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

#include "dwrite_eval.hpp"

#include <mpi.h>
#include <unistd.h>

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>

/*----< usage() >------------------------------------------------------------*/
static void usage (char *argv0) {
    std::string help =
        "Usage: %s [OPTION]...\n"
        "       [-h] Print help\n"
        "       [-v] Verbose mode\n"
        "       [-n nvar] Number of datasets to simulate\n"
        "       [-c file_path] Name of input netCDF file describing data decompositions\n"
        "       [-o output_dir] Output directory name (default ./)\n";
    fprintf (stderr, help.c_str (), argv0);
}

/*----< main() >-------------------------------------------------------------*/
int main (int argc, char **argv) {
    int i;
    int err;
    int rank, np;
    std::string out_dir_path;
    std::string config_path;
    MPI_Comm io_comm = MPI_COMM_WORLD;
    bool verbose     = false;

    int ndecom;
    int nvar = 1;
    int contig_nreqs[MAX_NUM_DECOMP], *disps[MAX_NUM_DECOMP];
    int *blocklens[MAX_NUM_DECOMP];
    MPI_Offset dims[MAX_NUM_DECOMP][2], estimated_nc_ibuf_size;
    MPI_Info info = MPI_INFO_NULL;

    MPI_Init (&argc, &argv);

    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &np);

    /* command-line arguments */
    while ((i = getopt (argc, argv, "c:o:n:v")) != EOF) {
        switch (i) {
            case 'c':
                config_path = std::string (optarg);
                break;
            case 'o':
                out_dir_path = std::string (optarg);
                break;
            case 'n':
                nvar = atoi (optarg);
                break;
            case 'v':
                verbose = true;
                break;
            case 'h':
            default:
                if (rank == 0) usage (argv[0]);
                err = -1;
        }
    }

    dwrite_eval_profile_reset ();

    TIMER_START

    if(rank==0){
        std::cout << "Reading decom file ..." << std::endl;        
    }

    err = read_decomp (io_comm, config_path.c_str (), &ndecom, dims, contig_nreqs, disps, blocklens,
                       verbose);
    CHECK_ERR

    if(rank==0){
        std::cout << "Testing N H5Dwrite ..." << std::endl;        
    }

    err = dwrite_eval_ndwrite (io_comm, out_dir_path, nvar, ndecom, dims, contig_nreqs, disps, blocklens);
    CHECK_ERR

    if(rank==0){
        std::cout << "Testing single H5Dwrite ..." << std::endl;        
    }

    err = dwrite_eval_dwrite (io_comm, out_dir_path, nvar, ndecom, dims, contig_nreqs, disps, blocklens);
    CHECK_ERR

    if(rank==0){
        std::cout << "Testing H5Dwrite_n ..." << std::endl;        
    }

    err = dwrite_eval_dwrite_n (io_comm, out_dir_path, nvar, ndecom, dims, contig_nreqs, disps, blocklens);
    CHECK_ERR

    TIMER_STOP (DWRITE_EVAL_TIMER_ALL)

    dwrite_eval_profile_print ();

err_out:;
    return 0;
}