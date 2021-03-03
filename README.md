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
  % srun -n 1024 -t 10 ./dwrite_eval -o /global/cscratch1/sd/khl7265/FS_64_1M/E3SM -c /global/cscratch1/sd/khl7265/FS_64_1M/E3SM/decom/FC5AV1C-H01B_ne120_oRRS18v3_21600p.nc -n 3
  Reading decom file ...
  Testing N H5Dwrite ...
  Testing single H5Dwrite ...
  Testing H5Dwrite_n ...
  total run time (mean): 60.776972
  total run time (max): 60.978292
  total run time (min): 60.736244
  total run time (var): 1.651960

  end to end I/O time using H5Dwrite per hyper-slab (mean): 0.472968
  end to end I/O time using H5Dwrite per hyper-slab (max): 0.480310
  end to end I/O time using H5Dwrite per hyper-slab (min): 0.471733
  end to end I/O time using H5Dwrite per hyper-slab (var): 0.001038

  write request posting time using H5Dwrite per hyper-slab (mean): 0.114535
  write request posting time using H5Dwrite per hyper-slab (max): 0.244320
  write request posting time using H5Dwrite per hyper-slab (min): 0.075005
  write request posting time using H5Dwrite per hyper-slab (var): 1.308148

  time calling H5Dwrite using H5Dwrite per hyper-slab (mean): 0.108226
  time calling H5Dwrite using H5Dwrite per hyper-slab (max): 0.229852
  time calling H5Dwrite using H5Dwrite per hyper-slab (min): 0.070552
  time calling H5Dwrite using H5Dwrite per hyper-slab (var): 1.164127

  end to end I/O time using H5S_SELECT_OR and single H5Dwrite (mean): 0.236215
  end to end I/O time using H5S_SELECT_OR and single H5Dwrite (max): 0.237016
  end to end I/O time using H5S_SELECT_OR and single H5Dwrite (min): 0.235130
  end to end I/O time using H5S_SELECT_OR and single H5Dwrite (var): 0.000039

  write request posting time using H5S_SELECT_OR and single H5Dwrite (mean): 13.994905
  write request posting time using H5S_SELECT_OR and single H5Dwrite (max): 59.055004
  write request posting time using H5S_SELECT_OR and single H5Dwrite (min): 4.688443
  write request posting time using H5S_SELECT_OR and single H5Dwrite (var): 109542.572527

  time calling H5Dwrite using H5S_SELECT_OR and single H5Dwrite (mean): 0.005857
  time calling H5Dwrite using H5S_SELECT_OR and single H5Dwrite (max): 0.013836
  time calling H5Dwrite using H5S_SELECT_OR and single H5Dwrite (min): 0.003763
  time calling H5Dwrite using H5S_SELECT_OR and single H5Dwrite (var): 0.003809

  end to end I/O time using log-based VOL H5Dwrite_n (mean): 0.184648
  end to end I/O time using log-based VOL H5Dwrite_n (max): 0.188309
  end to end I/O time using log-based VOL H5Dwrite_n (min): 0.183816
  end to end I/O time using log-based VOL H5Dwrite_n (var): 0.000290

  write request posting time using log-based VOL H5Dwrite_n (mean): 0.000710
  write request posting time using log-based VOL H5Dwrite_n (max): 0.013382
  write request posting time using log-based VOL H5Dwrite_n (min): 0.000352
  write request posting time using log-based VOL H5Dwrite_n (var): 0.002046

  time calling H5Dwrite using log-based VOL H5Dwrite_n (mean): 0.000398
  time calling H5Dwrite using log-based VOL H5Dwrite_n (max): 0.007939
  time calling H5Dwrite using log-based VOL H5Dwrite_n (min): 0.000198
  time calling H5Dwrite using log-based VOL H5Dwrite_n (var): 0.000712
  ```
