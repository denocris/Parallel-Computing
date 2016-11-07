#!/bin/bash
#PBS -l nodes=4:ppn=20
#PBS -l walltime=00:01:00
#PBS -q reserved2
#PBS -N cuda_reverse

cd /home/cdenobi/P1.3_seed/Cuda

touch cuda_vectrev.txt
./a.out
