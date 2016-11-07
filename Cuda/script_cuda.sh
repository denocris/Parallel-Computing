#!/bin/bash
#PBS -l nodes=4:ppn=20
#PBS -l walltime=00:10:00
#PBS -q reserved2
#PBS -N cuda_reverse

module purge
module load gcc/4.8.2
module load cudatoolkit


cd /home/cdenobi/P1.3_seed/Cuda

rm cuda_reverse.*

./a.out
