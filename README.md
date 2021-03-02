## Performance evaluation of different H5Dwrite methods

This `dwrite_eval` program is a case study of different approaches 
to writing multiple hyper-slabs into an HDF5 dataset using the 
[log-io VOL](https://github.com/DataLib-ECP/log_io_vol).
`dwrite_eval` emulates the I/O pattern of the
[E3SM](https://github.com/E3SM-Project/E3SM) and
[E3SM-IO Benchmark](https://github.com/Parallel-NetCDF/E3SM-IO) where 
each process performs a large amount of non-contiguous write requests to datasets.
`dwrite_eval` reports the time to write those datasets using the following three 
methods:
  1. Call `H5Dwrite` on each non-contiguous hyper-slab to write.
  2. Use `H5S_SELECT_OR` to select all hyper-slabs to write in a dataset 
     and only call `H5Dwrite` once.
  3. Use the new `H5Dwrite_n` API provided in the log-based VOL.

### Compile command to build the executable of benchmark program, `dwrite_eval`:
* Edit `Makefile` to customize the MPI compiler, compile options, location of
  PnetCDF library, location of HDF5 library, etc.
* The minimum required PnetCDF version is 1.11.0.
* The minimum required HDF5 version is 1.12.0.
* Run the command below to generate the executable program named `dwrite_eval.`
  ```
    % make 
  ```

### Run command:
* An example run command using `mpiexec` and 16 MPI processes is:
  ```
    % mpiexec -n 16 ./dwrite_eval -c datasets/f_case_866x72_16p.nc -o out_dir
  ```
* Command-line options:
  ```
    % ./dwrite_eval -h
    Usage: ./dwrite_eval [OPTION]...
            [-h] Print help
            [-v] Verbose mode
            [-n nvar] Number of datasets to simulate
            [-c file_path] Name of input netCDF file describing data decompositions
            [-o output_dir] Output directory name (default ./)
  ```
* Sample output:
  ```
  % ./dwrite_eval -c datasets/f_case_866x72_16p.nc -o ./
  Reading decom file ...
  Testing N H5Dwrite ...
  Testing single H5Dwrite ...
  Testing H5Dwrite_n ...
  #%$: dwrite_eval_total_time_mean: 0.054922
  #%$: dwrite_eval_total_time_max: 0.054922
  #%$: dwrite_eval_total_time_min: 0.054922
  #%$: dwrite_eval_total_time_var: 0.000000

  #%$: dwrite_eval_ndwrite_time_mean: 0.016290
  #%$: dwrite_eval_ndwrite_time_max: 0.016290
  #%$: dwrite_eval_ndwrite_time_min: 0.016290
  #%$: dwrite_eval_ndwrite_time_var: 0.000000

  #%$: dwrite_eval_ndwrite_wr_time_mean: 0.000072
  #%$: dwrite_eval_ndwrite_wr_time_max: 0.000072
  #%$: dwrite_eval_ndwrite_wr_time_min: 0.000072
  #%$: dwrite_eval_ndwrite_wr_time_var: 0.000000

  #%$: dwrite_eval_dwrite_time_mean: 0.006794
  #%$: dwrite_eval_dwrite_time_max: 0.006794
  #%$: dwrite_eval_dwrite_time_min: 0.006794
  #%$: dwrite_eval_dwrite_time_var: 0.000000

  #%$: dwrite_eval_dwrite_wr_time_mean: 0.000048
  #%$: dwrite_eval_dwrite_wr_time_max: 0.000048
  #%$: dwrite_eval_dwrite_wr_time_min: 0.000048
  #%$: dwrite_eval_dwrite_wr_time_var: 0.000000

  #%$: dwrite_eval_dwrite_n_time_mean: 0.029205
  #%$: dwrite_eval_dwrite_n_time_max: 0.029205
  #%$: dwrite_eval_dwrite_n_time_min: 0.029205
  #%$: dwrite_eval_dwrite_n_time_var: 0.000000

  #%$: dwrite_eval_dwrite_n_wr_time_mean: 0.000041
  #%$: dwrite_eval_dwrite_n_wr_time_max: 0.000041
  #%$: dwrite_eval_dwrite_n_wr_time_min: 0.000041
  #%$: dwrite_eval_dwrite_n_wr_time_var: 0.000000

  #%$: dwrite_eval_total_count_mean: 1.000000
  #%$: dwrite_eval_total_count_max: 1.000000
  #%$: dwrite_eval_total_count_min: 1.000000
  #%$: dwrite_eval_total_count_var: 0.000000

  #%$: dwrite_eval_ndwrite_count_mean: 1.000000
  #%$: dwrite_eval_ndwrite_count_max: 1.000000
  #%$: dwrite_eval_ndwrite_count_min: 1.000000
  #%$: dwrite_eval_ndwrite_count_var: 0.000000

  #%$: dwrite_eval_ndwrite_wr_count_mean: 1.000000
  #%$: dwrite_eval_ndwrite_wr_count_max: 1.000000
  #%$: dwrite_eval_ndwrite_wr_count_min: 1.000000
  #%$: dwrite_eval_ndwrite_wr_count_var: 0.000000

  #%$: dwrite_eval_dwrite_count_mean: 1.000000
  #%$: dwrite_eval_dwrite_count_max: 1.000000
  #%$: dwrite_eval_dwrite_count_min: 1.000000
  #%$: dwrite_eval_dwrite_count_var: 0.000000

  #%$: dwrite_eval_dwrite_wr_count_mean: 1.000000
  #%$: dwrite_eval_dwrite_wr_count_max: 1.000000
  #%$: dwrite_eval_dwrite_wr_count_min: 1.000000
  #%$: dwrite_eval_dwrite_wr_count_var: 0.000000

  #%$: dwrite_eval_dwrite_n_count_mean: 1.000000
  #%$: dwrite_eval_dwrite_n_count_max: 1.000000
  #%$: dwrite_eval_dwrite_n_count_min: 1.000000
  #%$: dwrite_eval_dwrite_n_count_var: 0.000000

  #%$: dwrite_eval_dwrite_n_wr_count_mean: 1.000000
  #%$: dwrite_eval_dwrite_n_wr_count_max: 1.000000
  #%$: dwrite_eval_dwrite_n_wr_count_min: 1.000000
  #%$: dwrite_eval_dwrite_n_wr_count_var: 0.000000
  ```
