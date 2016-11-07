#!/bin/bash
#PBS -l nodes=6:ppn=20
#PBS -l walltime=00:01:00
#PBS -q reserved2

cd /home/cdenobi/P1.3_seed/Cuda

touch cuda_vectrev.txt
nvcc ./a.out >> cuda_vectrev.txt
