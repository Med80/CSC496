#!/bin/bash
#SBATCH -p RM
#SBATCH -t 00:05:00
#SBATCH -N 2
#SBATCH --ntasks-per-node 16

#echo commands to stdout
set -x

#load MPI libraries
module add mpi/gcc_openmpi > /dev/null 2>&1

# change directory to where the source codes are
cd $HOME/intro-mpi

#set variable so that task placement works as expected
export  I_MPI_JOB_RESPECT_PROCESS_PLACEMENT=0


#compile MPI program
mpicc $1

#run MPI program
mpirun -np $2 ./a.out
