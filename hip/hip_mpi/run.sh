#!/bin/bash
which mpirun
mpirun --version
mpicc --showme
ompi_info | grep -i rocm
ompi_info | grep -i ucx
ucx_info -d | grep -i rocm
OMPI_CXX=hipcc mpicxx hip_mpi_test.cpp -o hip_mpi_test
mpirun -np 2 --mca pml ucx --mca pml_base_verbose 100 ./hip_mpi_test
UCX_LOG_LEVEL=info mpirun -np 2 --mca pml ucx ./hip_mpi_test
