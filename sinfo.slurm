#!/bin/bash
#SBATCH -o job_%j_%N.out
#SBATCH -J check
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=10
#SBATCH -t 300:00

lscpu