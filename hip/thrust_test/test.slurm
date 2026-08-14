#!/bin/bash
#SBATCH --ntasks=8
#SBATCH --partition=MI250
#SBATCH --gres=gpu:8
#SBATCH --time=00:20:00
#SBATCH --cpus-per-task=4
#SBATCH --exclusive

module purge
#module load openmpi/rocm-7.2.2/5.0.10
module load rocm/7.2.2

hipcc test.cpp -o test_thrust
./test_thrust
