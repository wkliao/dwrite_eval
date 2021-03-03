#!/bin/bash

# Defaults
NN=1
PPN=32
RUNS=1
TL=30
CONFIG=F120
RECS=1
NVAR=3
OP=w
HX=0
DEBUG=0
HOSTNAME_PREFIX=${HOSTNAME:0:4}
if [[ "$HOSTNAME_PREFIX" == "cori" ]]; then
    # Cori
    HDF5_SRC_PATH=/global/homes/k/khl7265/hdf5/1.12.0
    HDF5_LIB_PATH=/global/homes/k/khl7265/.local/hdf5/1.12.0
    PNC_SRC_PATH=/global/homes/k/khl7265/pnetcdf/master
    PNC_LIB_PATH=/global/homes/k/khl7265/.local/pnetcdf/master
    LOGVOL_SRC_PATH=/global/homes/k/khl7265/logvol/master
    LOGVOL_LIB_PATH=/global/homes/k/khl7265/.local/log_io_vol/master
    OUTPATH_ROOT=/global/cscratch1/sd/khl7265/FS_64_1M/E3SM
    INFILE=/global/cscratch1/sd/wkliao/FS_1M_32/ND_1951_256K.1.h5
    ACC=m2956
    QUEUE=regular
    SCRIPT=cori
    NODE_TYPE=haswell
    RTL=3
elif [[ "$HOSTNAME_PREFIX" == "thet" ]]; then
    # Theta (Not supported yet)
    HDF5_SRC_PATH=/gpfs/mira-home/khou/hdf5/1.12.0
    HDF5_LIB_PATH=/gpfs/mira-home/khou/.local/hdf5/1.12.0
    PNC_SRC_PATH=/gpfs/mira-home/khou/pnetcdf/master
    PNC_LIB_PATH=/gpfs/mira-home/khou/.local/pnetcdf/master
    LOGVOL_SRC_PATH=/gpfs/mira-home/khou/logvol/master
    LOGVOL_LIB_PATH=/gpfs/mira-home/khou/.local/log_io_vol/master
    OUTPATH_ROOT=/lus-projects/CSC250STDM12/khou/FS_48_1M/e3sm
    INFILE=/global/cscratch1/sd/wkliao/FS_1M_32/ND_1951_256K.1.h5
    ACC=CSC250STDM12
    SCRIPT=theta
    QUEUE=default
    NODE_TYPE=knl
    RTL=180
elif [[ "$HOSTNAME_PREFIX" == "Ubun" ]]; then
    # Dev VM
    HDF5_SRC_PATH=${HOME}/Desktop/hdf5/1.12.0
    HDF5_LIB_PATH=${HOME}/.local/hdf5/1.12.0
    PNC_SRC_PATH=${HOME}/Desktop/pnetcdf/master
    PNC_LIB_PATH=${HOME}/.local/pnetcdf/master
    LOGVOL_SRC_PATH=${HOME}/.local/log_io_vol/master
    LOGVOL_LIB_PATH=${HOME}/.local/log_io_vol/master
    OUTPATH_ROOT=${HOME}/FS_TEST/dwrite_eval
    INFILE=${HOME}/Desktop/nova_data/neardet_r00012096_s01_t00_R19-02-23-miniprod5.i_v1_data.h5caf.h5
    ACC=test_acc
    SCRIPT=local
    NODE_TYPE=local
    RTL=0
    PPN=16
fi

# Parse options
OTHER_PARAMS=""
while (( "$#" )); do
    case "$1" in
        -n|--nn)
            NN=$2
            shift 
            shift 
            ;;
        -N|--node)
            NODE_TYPE=$2
            shift 
            shift 
            ;;
        -d|--debug)
            DEBUG=1
            shift 
            shift 
            ;;
        -p|--ppn)
            PPN=$2
            shift 
            shift 
            ;;
        -r|--repeation)
            RUNS=$2
            shift 
            shift 
            ;;
        -R|--record)
            RECS=$2
            shift 
            shift 
            ;;
        -a|--account)
            ACC=$2
            shift 
            shift 
            ;;
        -T|--timeout)
            TL=$2
            shift 
            shift 
            ;;
        -t|--runtimeout)
            RTL=$2
            shift 
            shift 
            ;;
        -c|--config)
            CONFIG=$2
            shift 
            shift 
            ;;
        -s|--script)
            SCRIPT=$2
            shift 
            shift 
            ;;
        -o|--outdir)
            OUTPATH_ROOT=$2
            shift 
            shift 
            ;;
        -v|--nvar)
            NVAR=$2
            shift 
            shift 
            ;;
        -*|--*=) # unsupported flags
            echo "Error: Unsupported flag $1" >&2
            exit 1
            ;;
        *) # preserve positional arguments
            OTHER_PARAMS="$PARAMS $1"
            shift
            ;;
    esac
done

if [[ "${DEBUG}" == "1" ]]; then
    NN=1
    RUNS=1
    CONFIG=FDBG
    if [[ "$HOSTNAME_PREFIX" == "cori" ]]; then
        QUEUE=debug
    elif [[ "$HOSTNAME_PREFIX" == "thet" ]]; then
        QUEUE=debug-flat-quad
    fi
fi

NP=NN*PPN

<<<<<<< HEAD
DWRITE_EVAL_HASH=$(git --git-dir .git rev-parse HEAD)
DWRITE_EVAL_HASH_PREFIX=${DWRITE_EVAL_HASH:0:5}
DWRITE_EVAL_DATE=$(stat -c %Y ./dwrite_eval)
=======
DEMO_HASH=$(git --git-dir .git rev-parse HEAD)
DEMO_HASH_PREFIX=${DEMO_HASH:0:5}
DEMO_DATE=$(stat -c %Y ./demo)
>>>>>>> job script for cori and theta

HDF5_HASH=$(git --git-dir ${HDF5_SRC_PATH}/.git rev-parse HEAD)
HDF5_HASH_PREFIX=${HDF5_HASH:0:5}
HDF5_DATE=$(stat -c %Y ${HDF5_LIB_PATH}/lib/libhdf5.so.200.0.0)

PNC_HASH=$(git --git-dir ${PNC_SRC_PATH}/.git rev-parse HEAD)
PNC_HASH_PREFIX=${PNC_HASH:0:5}

LOGVOL_HASH=$(git --git-dir ${LOGVOL_SRC_PATH}/.git rev-parse HEAD)
LOGVOL_HASH_PREFIX=${LOGVOL_HASH:0:5}
LOGVOL_DATE=$(stat -c %Y ${LOGVOL_LIB_PATH}/lib/libH5VL_log.so.0.0.0)

<<<<<<< HEAD
FNAME="${CONFIG}_logvl${LOGVOL_HASH_PREFIX}_e3sm${DWRITE_EVAL_HASH_PREFIX}_hdf${HDF5_HASH_PREFIX}_pnc${PNC_HASH_PREFIX}_${HOSTNAME_PREFIX}_${NN}_${PPN}"
=======
FNAME="${CONFIG}_logvl${LOGVOL_HASH_PREFIX}_e3sm${DEMO_HASH_PREFIX}_hdf${HDF5_HASH_PREFIX}_pnc${PNC_HASH_PREFIX}_${HOSTNAME_PREFIX}_${NN}_${PPN}"
>>>>>>> job script for cori and theta

SUBMIT_DATE=$(date)

GEN_SCRIPT="m4 -D EXP_NAME=\"${FNAME}\" -D EXP_NN=\"${NN}\" -D EXP_PPN=\"${PPN}\" -D EXP_TL=\"${TL}\" -D EXP_ACC=\"${ACC}\" -D EXP_NODE_TYPE=\"${NODE_TYPE}\" \
-D EXP_OUTDIR_ROOT=\"${OUTPATH_ROOT}\" -D EXP_CONFIG=\"${CONFIG}\" -D EXP_RECS=\"${RECS}\" -D EXP_NVAR=\"${NVAR}\" \
-D EXP_RUNS=\"${RUNS}\" -D EXP_RTL=\"${RTL}\" -D EXP_SUBMIT_DATE=\"${SUBMIT_DATE}\"  -D EXP_QUEUE=\"${QUEUE}\" \
<<<<<<< HEAD
-D EXP_DWRITE_EVAL_HASH=\"${DWRITE_EVAL_HASH}\" -D EXP_DWRITE_EVAL_DATE=\"${DWRITE_EVAL_DATE}\" \
=======
-D EXP_DEMO_HASH=\"${DEMO_HASH}\" -D EXP_DEMO_DATE=\"${DEMO_DATE}\" \
>>>>>>> job script for cori and theta
-D EXP_HDF5_LIB_PATH=\"${HDF5_LIB_PATH}\" -D EXP_HDF5_LIB_DATE=\"${HDF5_DATE}\" -D EXP_HDF5_HASH=\"${HDF5_HASH}\" \
-D EXP_PNC_LIB_PATH=\"${PNC_LIB_PATH}\" -D EXP_PNC_HASH=\"${PNC_HASH}\" \
-D EXP_LOGVOL_LIB_PATH=\"${LOGVOL_LIB_PATH}\" -D EXP_LOGVOL_HASH=\"${LOGVOL_HASH}\" -D EXP_LOGVOL_LIB_DATE=\"${LOGVOL_DATE}\" \
\"${SCRIPT}\".m4 > \"${FNAME}\".sl"

echo ${GEN_SCRIPT}
eval "${GEN_SCRIPT}"
chmod 755 ${FNAME}.sl
