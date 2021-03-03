#include <mpi.h>
#include <assert.h>

#include "dwrite_eval.hpp"

static double tmax[DWRITE_EVAL_NTIMER], tmin[DWRITE_EVAL_NTIMER], tmean[DWRITE_EVAL_NTIMER], tvar[DWRITE_EVAL_NTIMER], tvar_local[DWRITE_EVAL_NTIMER];
static double tlocal[DWRITE_EVAL_NTIMER];

const char *const tname[] = {
	"total run time",
	"end to end I/O time using H5Dwrite per hyper-slab",
	"dataset write (posting) time using H5Dwrite per hyper-slab",
	"end to end I/O time using H5S_SELECT_OR and single H5Dwrite",
	"dataset write (posting) time using H5S_SELECT_OR and single H5Dwrite",
	"end to end I/O time using log-based VOL H5Dwrite_n",
	"dataset write (posting) time using log-based VOL H5Dwrite_n",
};

void dwrite_eval_profile_add_time (int id, double t) {
	assert (id >= 0 && id < DWRITE_EVAL_NTIMER);
	tlocal[id] += t;
}

// Note: This only work if everyone calls H5Fclose
void dwrite_eval_profile_print () {
	int i;
	int np, rank, flag;

	MPI_Initialized (&flag);
	if (!flag) { MPI_Init (NULL, NULL); }

	MPI_Comm_size (MPI_COMM_WORLD, &np);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);

	MPI_Reduce (tlocal, tmax, DWRITE_EVAL_NTIMER, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	MPI_Reduce (tlocal, tmin, DWRITE_EVAL_NTIMER, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
	MPI_Allreduce (tlocal, tmean, DWRITE_EVAL_NTIMER, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	for (i = 0; i < DWRITE_EVAL_NTIMER; i++) {
		tmean[i] /= np;
		tvar_local[i] = (tlocal[i] - tmean[i]) * (tlocal[i] - tmean[i]);
	}
	MPI_Reduce (tvar_local, tvar, DWRITE_EVAL_NTIMER, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		for (i = 0; i < DWRITE_EVAL_NTIMER; i++) {
			printf ("%s (mean): %lf\n", tname[i], tmean[i]);
			printf ("%s (max): %lf\n", tname[i], tmax[i]);
			printf ("%s (min): %lf\n", tname[i], tmin[i]);
			printf ("%s (var): %lf\n\n", tname[i], tvar[i]);
		}
	}
}
void dwrite_eval_profile_reset () {
	int i;

	for (i = 0; i < DWRITE_EVAL_NTIMER; i++) {
		tlocal[i] = 0;
	}
}
