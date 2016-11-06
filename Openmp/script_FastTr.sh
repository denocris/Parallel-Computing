#!/bin/bash

#PBS -l nodes=1:ppn=20
#PBS -l walltime=00:01:00
#PBS -q reserved3
#PBS -N fast_transp

matrix_size=2048

cd /home/cdenobi/P1.3_seed/Openmp/

gcc -fopenmp FastTr_omp.c -o FastTr_omp
data="data.dat"
rm -f $data
for ((i=1; i<=16; i*=2)); do
    echo $i
    out=`OMP_NUM_THREADS=$i ./FastTr_omp $matrix_size | tail -1`
    echo "$i $out" >> $data
done