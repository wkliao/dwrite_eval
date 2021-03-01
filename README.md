## H5Dwrite_n demo

This is a demo program containing a case study of different approaches 
to writing multiple hyper-slabs in HDF5 datasets using the 
[log-io VOL](https://github.com/DataLib-ECP/log_io_vol).
The demo program simulates the I/O pattern of the 
[E3SM-IO Benchmark](https://github.com/Parallel-NetCDF/E3SM-IO) where 
each process performs a large amount of non-contiguous write to datasets.
The demo reports the time to write those datasets using the three 
methods below:
  1. Call H5Dwrite on each non-contiguous hyper-slab to write.
  2. Use H5S_SELECT_OR to select all hyper-slabs to write in a dataset 
     and only call H5Dwrite once.
  3. Use the H5Dwrite_n API provided in the log-io VOL.

### Compile command to build the executable of benchmark program, `e3sm_io`:
* Edit `Makefile` to customize the MPI compiler, compile options, location of
  PnetCDF library, location of HDF5 library, etc.
* The minimum required PnetCDF version is 1.11.0.
* The minimum required HDF5 version is 1.12.0.
* Run the command below to generate the executable program named `demo.`
  ```
    % make 
  ```

### Run command:
* An example run command using `mpiexec` and 16 MPI processes is:
  ```
    % mpiexec -n 16 ./demo -c datasets/f_case_866x72_16p.nc -o test
  ```
* Command-line options:
  ```
    % ./demo -h
    Usage: ./demo [OPTION]...
            [-h] Print help
            [-v] Verbose mode
            [-n nvar] Number of datasets to simulate
            [-c file_path] Name of input netCDF file describing data decompositions
            [-o output_dir] Output directory name (default ./)
  ```
