#!/bin/bash

#PBS -l walltime=00:01:00,nodes=3:ppn=2
#PBS -N search_pi_with_bcast_and_reduce
#PBS -q batch

cd $PBS_O_WORKDIR
mpirun --hostfile $PBS_NODEFILE -np 6 ./a.out