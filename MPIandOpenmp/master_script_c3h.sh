#!/bin/bash

projname=Mmul_hybrid
max_nodes=$1
mat_size=$2
# go up to 20480 if you want powers of 2 times 5, or up to 4*10^5 otherwise
exec_name="$projname.$mat_size.x"

module purge
module load openmpi
mpicc -fopenmp $projname.c -DMAT_SIZE=$mat_size -D__PRINT_TIME -o $exec_name

for i in `seq 1 $max_nodes`
do
output_name="data.$projname.n$i.size$mat_size.dat"
rm -f $output_name
qsub -v exec_name=$exec_name,output_name=$output_name,n=$i,mat_size=$mat_size ./node_script.sh -l nodes=$i:ppn=24
done
