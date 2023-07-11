#!/bin/bash
#SBATCH -o job_%j_%N.out
#SBATCH -p cpu
#SBATCH -J check
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -t 3:00
#SBATCH --nodelist cu05


export OMP_NUM_THREADS=1
echo -----CHECK-----
./checksum
