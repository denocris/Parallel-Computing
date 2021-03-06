#!/bin/bash
#PBS -l nodes=6:ppn=20
#PBS -l walltime=00:02:00
#PBS -q reserved3


module purge
module load openmpi
cd /home/cdenobi/P1.3_seed/MPIandOpenmp


size=$((2400))
rm -f matmul_scaling.txt
echo "# **************** OMP=1 NP/node=20 ******************" > matmul_scaling.txt
export OMP_NUM_THREADS=1
for i in 20 60 120; do
	mpirun -np $i --map-by ppr:20:node:pe=1 ./a.out $size | grep 666 >> matmul_scaling.txt
	done

echo "# **************** OMP=2 NP/node=10 ******************" >> matmul_scaling.txt
export OMP_NUM_THREADS=2
for i in 10 30 60; do
	mpirun -np $i --map-by ppr:10:node:pe=2 ./a.out $size | grep 666 >> matmul_scaling.txt
	done

echo "# **************** OMP=5 NP/node=4 ******************" >> matmul_scaling.txt
export OMP_NUM_THREADS=5
for i in 4 12 24; do
	mpirun -np $i --map-by ppr:4:node:pe=5 ./a.out $size | grep 666 >> matmul_scaling.txt
	done

echo "# **************** OMP=10 NP/node=2 ******************" >> matmul_scaling.txt
export OMP_NUM_THREADS=10
for i in 2 6 12; do
	mpirun -np $i --map-by ppr:2:node:pe=10 ./a.out $size | grep 666 >> matmul_scaling.txt
	done

echo "# **************** OMP=20 NP/node=1 ******************" >> matmul_scaling.txt
export OMP_NUM_THREADS=20
for i in 1 3 6; do
	mpirun -np $i --map-by ppr:1:node:pe=20 ./a.out $size | grep 666 >> matmul_scaling.txt
	done
