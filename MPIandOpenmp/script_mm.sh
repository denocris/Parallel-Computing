#!/bin/bash
#PBS -l nodes=6:ppn=20
#PBS -l walltime=00:01:00
#PBS -q reserved3
#PBS -N matmul

module purge
module load openmpi
cd /home/cdenobi/P1.3_seed/MPIandOpenmp


size=$((12000))
rm mat_mul.*

mpirun -np $i --map-by ppr:20:node:pe=1 ./a.out $size > matmul_scaling.txt

