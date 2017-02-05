#!/bin/sh
# Job name:
#SBATCH --job-name=grooves
#
# Project:
#SBATCH --account=uio
#
# Wall clock limit:
#SBATCH --mail-type=ALL
#SBATCH --time='48:00:00'
#
# Max memory usage per task:
#SBATCH --mem-per-cpu=3600M
#
#SBATCH --cpus-per-task=16

## Set up job environment
source /cluster/bin/jobsetup
module load gcc

## Run command
./FrictionProject 
