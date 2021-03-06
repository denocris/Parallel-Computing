#!/bin/bash
#PBS -l nodes=6:ppn=20
#PBS -l walltime=00:10:00

cd parallel_prog_2

module load gcc/4.8.2
module load openmpi

size=$((100))
touch matmul_scaling.txt
echo "# **************** OMP=1 NP/node=20 ******************" >> matmul_scaling.txt
export OMP_NUM_THREADS=1
for i in 20 60 120; do
	t=$(($i*1))		
	mpirun -np $i --map-by ppr:20:node:pe=1 ./ultra_matmul $size | sed 's/$/		'$t'/' | sed '/Mismatch/d' >> matmul_scaling.txt
	done

echo "# **************** OMP=2 NP/node=10 ******************" >> matmul_scaling.txt
export OMP_NUM_THREADS=2
for i in 10 30 60; do
	t=$(($i*2))	
	mpirun -np $i --map-by ppr:10:node:pe=2 ./ultra_matmul $size | sed 's/$/		'$t'/' | sed '/Mismatch/d' >> matmul_scaling.txt
	done

echo "# **************** OMP=5 NP/node=4 ******************" >> matmul_scaling.txt
export OMP_NUM_THREADS=5
for i in 4 12 24; do
	t=$(($i*5))
	mpirun -np $i --map-by ppr:4:node:pe=5 ./ultra_matmul $size | sed 's/$/		'$t'/' | sed '/Mismatch/d' >> matmul_scaling.txt
	done

echo "# **************** OMP=10 NP/node=2 ******************" >> matmul_scaling.txt
export OMP_NUM_THREADS=10
for i in 2 6 12; do
	t=$(($i*10))
	mpirun -np $i --map-by ppr:2:node:pe=10 ./ultra_matmul $size | sed 's/$/		'$t'/' | sed '/Mismatch/d' >> matmul_scaling.txt
	done

echo "# **************** OMP=20 NP/node=1 ******************" >> matmul_scaling.txt
export OMP_NUM_THREADS=20
for i in 1 3 6; do
	t=$(($i*20))
	mpirun -np $i --map-by ppr:1:node:pe=20 ./ultra_matmul $size | sed 's/$/		'$t'/' | sed '/Mismatch/d' >> matmul_scaling.txt
	done
