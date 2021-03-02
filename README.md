## H5Dwrite_n dwrite_eval

This is a dwrite_eval program containing a case study of different approaches 
to writing multiple hyper-slabs in HDF5 datasets using the 
[log-io VOL](https://github.com/DataLib-ECP/log_io_vol).
The dwrite_eval program simulates the I/O pattern of the 
[E3SM-IO Benchmark](https://github.com/Parallel-NetCDF/E3SM-IO) where 
each process performs a large amount of non-contiguous write to datasets.
The dwrite_eval reports the time to write those datasets using the three 
methods below:
  1. Call H5Dwrite on each non-contiguous hyper-slab to write.
  2. Use H5S_SELECT_OR to select all hyper-slabs to write in a dataset 
     and only call H5Dwrite once.
  3. Use the H5Dwrite_n API provided in the log-io VOL.

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
  % ./dwrite_eval -o . -c datasets/f_case_866x72_16p.nc 
  Reading decom file ...
  Testing N H5Dwrite ...
  Testing single H5Dwrite ...
  Testing H5Dwrite_n ...
  total run time (mean): 0.007596
  total run time (max): 0.007596
  total run time (min): 0.007596
  total run time (var): 0.000000

  end to end I/O time using H5Dwrite per hyper-slab (mean): 0.002850
  end to end I/O time using H5Dwrite per hyper-slab (max): 0.002850
  end to end I/O time using H5Dwrite per hyper-slab (min): 0.002850
  end to end I/O time using H5Dwrite per hyper-slab (var): 0.000000

  dataset write (posting) time using H5Dwrite per hyper-slab (mean): 0.000066
  dataset write (posting) time using H5Dwrite per hyper-slab (max): 0.000066
  dataset write (posting) time using H5Dwrite per hyper-slab (min): 0.000066
  dataset write (posting) time using H5Dwrite per hyper-slab (var): 0.000000

  end to end I/O time using H5S_SELECT_OR and single H5Dwrite (mean): 0.001286
  end to end I/O time using H5S_SELECT_OR and single H5Dwrite (max): 0.001286
  end to end I/O time using H5S_SELECT_OR and single H5Dwrite (min): 0.001286
  end to end I/O time using H5S_SELECT_OR and single H5Dwrite (var): 0.000000

  dataset write (posting) time using H5S_SELECT_OR and single H5Dwrite (mean): 0.000044
  dataset write (posting) time using H5S_SELECT_OR and single H5Dwrite (max): 0.000044
  dataset write (posting) time using H5S_SELECT_OR and single H5Dwrite (min): 0.000044
  dataset write (posting) time using H5S_SELECT_OR and single H5Dwrite (var): 0.000000

  end to end I/O time using log-based VOL H5Dwrite_n (mean): 0.000865
  end to end I/O time using log-based VOL H5Dwrite_n (max): 0.000865
  end to end I/O time using log-based VOL H5Dwrite_n (min): 0.000865
  end to end I/O time using log-based VOL H5Dwrite_n (var): 0.000000

  dataset write (posting) time using log-based VOL H5Dwrite_n (mean): 0.000033
  dataset write (posting) time using log-based VOL H5Dwrite_n (max): 0.000033
  dataset write (posting) time using log-based VOL H5Dwrite_n (min): 0.000033
  dataset write (posting) time using log-based VOL H5Dwrite_n (var): 0.000000
  ```
