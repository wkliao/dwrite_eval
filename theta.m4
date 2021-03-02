changequote(`[[[', `]]]')dnl
changecom([[[###]]], [[[$$$]]])dnl
#!/bin/bash
#COBALT -q EXP_QUEUE
#COBALT -n EXP_NN
#COBALT -t EXP_TL
#COBALT -o EXP_NAME.txt
#COBALT -e EXP_NAME.err
#COBALT -A EXP_ACC

CONFIG_FDBG=datasets/f_case_866x72_16p.nc
CONFIG_F30=datasets/f_case_48602x72_512p.nc
CONFIG_F120=/lus-projects/CSC250STDM12/khou/FS_48_1M/e3sm/decom/FC5AV1C-H01B_ne120_oRRS18v3_21600p.nc
CONFIG_G=/lus-projects/CSC250STDM12/khou/FS_48_1M/e3sm/decom/GMPAS-NYF_T62_oRRS18to6v3_9600p.nc

DWRITE_EVAL_DATE="EXP_DWRITE_EVAL_DATE"
HDF5_LIB_PATH=EXP_HDF5_LIB_PATH
HDF5_LIB_DATE="EXP_HDF5_LIB_DATE"
PNC_LIB_PATH=EXP_PNC_LIB_PATH
LOGVOL_LIB_PATH=EXP_LOGVOL_LIB_PATH
LOGVOL_LIB_DATE="EXP_LOGVOL_LIB_DATE"
NREC=EXP_RECS
NVAR=EXP_NVAR
PPN=EXP_PPN
RTL=EXP_RTL
CONFIG_POST=EXP_CONFIG
OUTDIR_ROOT=EXP_OUTDIR_ROOT

APP=dwrite_eval
CONFIG="CONFIG_${CONFIG_POST}"
CONFIG=${!CONFIG}

NN=EXP_NN
let NP=NN*PPN

# Print exp setting
echo "#%$: dwrite_eval_hash: EXP_DWRITE_EVAL_HASH"
echo "#%$: pnc_hash: EXP_PNC_HASH"
echo "#%$: pnc_path: ${PNC_LIB_PATH}"
echo "#%$: hdf5_hash: EXP_HDF5_HASH"
echo "#%$: hdf5_path: ${HDF5_LIB_PATH}"
echo "#%$: logvol_hash: EXP_LOGVOL_HASH"
echo "#%$: logvol_path: ${LOGVOL_LIB_PATH}"
echo "#%$: submit_date: EXP_SUBMIT_DATE"
echo "#%$: exe_date: $(date)"
echo "#%$: exp: dwrite_eval"
echo "#%$: repeation: EXP_RUNS"
echo "#%$: nvars: ${NVAR}"
echo "#%$: output_folder: EXP_OUTDIR_ROOT"
echo "#%$: e3sm_config_post: ${CONFIG_POST}"
echo "#%$: e3sm_config: ${CONFIG}"
echo "#%$: number_of_nodes: ${NN}"
echo "#%$: number_of_proc: ${NP}"
echo '-----+-----++----***--------+++++**++++--+---'

echo "mkdir -p ${OUTDIR_ROOT}"
mkdir -p ${OUTDIR_ROOT}

export LD_LIBRARY_PATH=${HDF5_LIB_PATH}/lib:${PNC_LIB_PATH}/lib:${LOGVOL_LIB_PATH}/lib:/opt/intel/compilers_and_libraries_2020.0.166/linux/compiler/lib/intel64:${LD_LIBRARY_PATH}
#export HDF5_PLUGIN_PATH=${HOME}/.local/lib
export H5VL_LOG_METADATA_MERGE=1
export H5VL_LOG_METADATA_ZIP=1
export H5VL_LOG_SEL_ENCODING=1

ulimit -c unlimited

TSTARTTIME=`date +%s.%N`

for i in $(seq EXP_RUNS);
do
    OUTDIR=${OUTDIR_ROOT}
    echo "rm -f ${OUTDIR}/*"
    rm -f ${OUTDIR}/*

    echo "========================== DWRITE_EVAL ${API} ${OP} =========================="
    >&2 echo "========================== DWRITE_EVAL ${API} ${OP}=========================="
    
    CUR_DWRITE_EVAL_DATE=$(stat -c %Y ./dwrite_eval)
    if [[ "${CUR_DWRITE_EVAL_DATE}" != "${DWRITE_EVAL_DATE}" ]]; then
        echo "Warning: dwrite_eval changed after submission"
    fi
    CUR_HDF5_DATE=$(stat -c %Y ${HDF5_LIB_PATH}/lib/libhdf5.so.200.0.0)
    if [[ "${CUR_HDF5_DATE}" != "${HDF5_LIB_DATE}" ]]; then
        echo "Warning: libhdf5.so changed after submission"
    fi
    CUR_LOGVOL_DATE=$(stat -c %Y ${LOGVOL_LIB_PATH}/lib/libH5VL_log.so.0.0.0)
    if [[ "${CUR_LOGVOL_DATE}" != "${LOGVOL_LIB_DATE}" ]]; then
        echo "Warning: libH5VL_log.so.0.0.0 changed after submission"
    fi

    echo "#%$: exp: dwrite_eval"
    echo "#%$: app: ${APP}"
    echo "#%$: config: ${CONFIG}"
    echo "#%$: nrecords: ${NREC}"
    echo "#%$: nvars: ${NVAR}"
    echo "#%$: number_of_nodes: ${NN}"
    echo "#%$: number_of_proc: ${NP}"
    echo "#%$: logvol_metadata_merge: ${H5VL_LOG_METADATA_MERGE}"
    echo "#%$: logvol_metadata_zip: ${H5VL_LOG_METADATA_ZIP}"
    echo "#%$: logvol_metadata_encoding: ${H5VL_LOG_SEL_ENCODING}"
                            
    STARTTIME=`date +%s.%N`

    echo "aprun -n ${NP} -N ${PPN} -t ${RTL} -e H5VL_LOG_METADATA_MERGE=${H5VL_LOG_METADATA_MERGE} -e H5VL_LOG_METADATA_ZIP=${H5VL_LOG_METADATA_ZIP} -e H5VL_LOG_SEL_ENCODING=${H5VL_LOG_SEL_ENCODING} ./${APP} -o ${OUTDIR} -c ${CONFIG} -n ${NVAR}"
    aprun -n ${NP} -N ${PPN} -t ${RTL} -e H5VL_LOG_METADATA_MERGE=${H5VL_LOG_METADATA_MERGE} -e H5VL_LOG_METADATA_ZIP=${H5VL_LOG_METADATA_ZIP} -e H5VL_LOG_SEL_ENCODING=${H5VL_LOG_SEL_ENCODING} ./${APP} -o ${OUTDIR} -c  ${CONFIG} -n ${NVAR}

    ENDTIME=`date +%s.%N`
    TIMEDIFF=`echo "$ENDTIME - $STARTTIME" | bc | awk -F"." '{print $1"."$2}'`

    echo "#%$: exe_time: $TIMEDIFF"

    echo "ls -lah ${OUTDIR}"
    ls -lah ${OUTDIR}
    echo "lfs getstripe ${OUTDIR}"
    lfs getstripe ${OUTDIR}

    echo '-----+-----++------------+++++++++--+---'
done

ENDTIME=`date +%s.%N`
TIMEDIFF=`echo "$ENDTIME - $TSTARTTIME" | bc | awk -F"." '{print $1"."$2}'`
echo "-------------------------------------------------------------"
echo "total_exe_time: $TIMEDIFF"
echo "-------------------------------------------------------------"
