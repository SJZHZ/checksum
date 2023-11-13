#!/bin/bash
#SBATCH -o job_%j_%N.out
#SBATCH -J check
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -t 1200:00


export OMP_NUM_THREADS=1
echo -----CHECK-----
./checksum 1 11 100000000 400
# ./checksum 1 13 10 100
# ./test
echo -----END------