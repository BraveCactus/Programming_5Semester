#!/bin/bash

#PBS -l walltime=00:01:00,nodes=3:ppn=2
#PBS -N omp_hello
#PBS -q batch

cd $PBS_O_WORKDIR
export OMP_NUM_THREADS=$PBS_NUM_PPN
./a.out