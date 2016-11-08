#!/bin/bash
#PBS -l nodes=6:ppn=20
#PBS -l walltime=00:02:00
#PBS -q reserved3


# exec_name and output_name must be defined in master_script.sh
cd $PBS_O_WORKDIR
module load openmpi

divisors="1 2 4 5 10"
for i in $divisors
do
proc_per_res=$((10/$i))
export OMP_NUM_THREADS=$i
time=`mpirun --map-by ppr:$proc_per_res:socket:pe=$i ./$exec_name 1024 | grep 666`
echo "$n $i $time" >> $output_name
done

# the output is printed with a trailing 666, so as to easily discard all lines of warning