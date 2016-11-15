#!/bin/bash

projname=Mmul_hybrid
max_nodes=$1
mat_size=$2
# go up to 20480 if you want powers of 2 times 5, or up to 4*10^5 otherwise
exec_name="$projname.$mat_size.x"

module load openmpi
mpicc -fopenmp $projname.c -DMAT_SIZE=$mat_size -D__PRINT_TIME -o $exec_name

for num_nods in `seq 1 $max_nodes`
do
  output_name="timedata.$projname.n$num_nods.size$mat_size.dat"
  rm -f $output_name
  qsub -v exec_name=$exec_name,output_name=$output_name,num_nods=$num_nods,mat_size=$mat_size ./node_scr.sh -l nodes=$num_nods:ppn=20
done
