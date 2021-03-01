#include <mpi.h>
#include <assert.h>

#include "dwriten_demo.hpp"

static double tmax[DEMO_NTIMER], tmin[DEMO_NTIMER], tmean[DEMO_NTIMER], tvar[DEMO_NTIMER], tvar_local[DEMO_NTIMER];
static double tlocal[DEMO_NTIMER], clocal[DEMO_NTIMER];

const char *const tname[] = {
	"demo_total",
	"demo_ndwrite",
	"demo_ndwrite_wr",
	"demo_dwrite",
	"demo_dwrite_wr",
    "demo_dwrite_n",
	"demo_dwrite_n_wr",
};

void demo_profile_add_time (int id, double t) {
	assert (id >= 0 && id < DEMO_NTIMER);
	tlocal[id] += t;
	clocal[id]++;
}

// Note: This only work if everyone calls H5Fclose
void demo_profile_print () {
	int i;
	int np, rank, flag;

	MPI_Initialized (&flag);
	if (!flag) { MPI_Init (NULL, NULL); }

	MPI_Comm_size (MPI_COMM_WORLD, &np);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);

	MPI_Reduce (tlocal, tmax, DEMO_NTIMER, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	MPI_Reduce (tlocal, tmin, DEMO_NTIMER, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
	MPI_Allreduce (tlocal, tmean, DEMO_NTIMER, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	for (i = 0; i < DEMO_NTIMER; i++) {
		tmean[i] /= np;
		tvar_local[i] = (tlocal[i] - tmean[i]) * (tlocal[i] - tmean[i]);
	}
	MPI_Reduce (tvar_local, tvar, DEMO_NTIMER, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		for (i = 0; i < DEMO_NTIMER; i++) {
			printf ("#%%$: %s_time_mean: %lf\n", tname[i], tmean[i]);
			printf ("#%%$: %s_time_max: %lf\n", tname[i], tmax[i]);
			printf ("#%%$: %s_time_min: %lf\n", tname[i], tmin[i]);
			printf ("#%%$: %s_time_var: %lf\n\n", tname[i], tvar[i]);
		}
	}

	MPI_Reduce (clocal, tmax, DEMO_NTIMER, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	MPI_Reduce (clocal, tmin, DEMO_NTIMER, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
	MPI_Allreduce (clocal, tmean, DEMO_NTIMER, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	for (i = 0; i < DEMO_NTIMER; i++) {
		tmean[i] /= np;
		tvar_local[i] = (clocal[i] - tmean[i]) * (clocal[i] - tmean[i]);
	}
	MPI_Reduce (tvar_local, tvar, DEMO_NTIMER, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		for (i = 0; i < DEMO_NTIMER; i++) {
			printf ("#%%$: %s_count_mean: %lf\n", tname[i], tmean[i]);
			printf ("#%%$: %s_count_max: %lf\n", tname[i], tmax[i]);
			printf ("#%%$: %s_count_min: %lf\n", tname[i], tmin[i]);
			printf ("#%%$: %s_count_var: %lf\n\n", tname[i], tvar[i]);
		}
	}
}
void demo_profile_reset () {
	int i;

	for (i = 0; i < DEMO_NTIMER; i++) {
		tlocal[i] = 0;
		clocal[i] = 0;
	}
}
