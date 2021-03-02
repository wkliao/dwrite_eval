#include <mpi.h>
#include <assert.h>

#include "dwrite_eval.hpp"

static double tmax[DWRITE_EVAL_NTIMER], tmin[DWRITE_EVAL_NTIMER], tmean[DWRITE_EVAL_NTIMER], tvar[DWRITE_EVAL_NTIMER], tvar_local[DWRITE_EVAL_NTIMER];
static double tlocal[DWRITE_EVAL_NTIMER], clocal[DWRITE_EVAL_NTIMER];

const char *const tname[] = {
	"dwrite_eval_total",
	"dwrite_eval_ndwrite",
	"dwrite_eval_ndwrite_wr",
	"dwrite_eval_dwrite",
	"dwrite_eval_dwrite_wr",
    "dwrite_eval_dwrite_n",
	"dwrite_eval_dwrite_n_wr",
};

void dwrite_eval_profile_add_time (int id, double t) {
	assert (id >= 0 && id < DWRITE_EVAL_NTIMER);
	tlocal[id] += t;
	clocal[id]++;
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
			printf ("#%%$: %s_time_mean: %lf\n", tname[i], tmean[i]);
			printf ("#%%$: %s_time_max: %lf\n", tname[i], tmax[i]);
			printf ("#%%$: %s_time_min: %lf\n", tname[i], tmin[i]);
			printf ("#%%$: %s_time_var: %lf\n\n", tname[i], tvar[i]);
		}
	}

	MPI_Reduce (clocal, tmax, DWRITE_EVAL_NTIMER, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	MPI_Reduce (clocal, tmin, DWRITE_EVAL_NTIMER, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
	MPI_Allreduce (clocal, tmean, DWRITE_EVAL_NTIMER, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	for (i = 0; i < DWRITE_EVAL_NTIMER; i++) {
		tmean[i] /= np;
		tvar_local[i] = (clocal[i] - tmean[i]) * (clocal[i] - tmean[i]);
	}
	MPI_Reduce (tvar_local, tvar, DWRITE_EVAL_NTIMER, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		for (i = 0; i < DWRITE_EVAL_NTIMER; i++) {
			printf ("#%%$: %s_count_mean: %lf\n", tname[i], tmean[i]);
			printf ("#%%$: %s_count_max: %lf\n", tname[i], tmax[i]);
			printf ("#%%$: %s_count_min: %lf\n", tname[i], tmin[i]);
			printf ("#%%$: %s_count_var: %lf\n\n", tname[i], tvar[i]);
		}
	}
}
void dwrite_eval_profile_reset () {
	int i;

	for (i = 0; i < DWRITE_EVAL_NTIMER; i++) {
		tlocal[i] = 0;
		clocal[i] = 0;
	}
}
