#!/bin/bash
#PBS -l walltime=00:10:00


# exec_name and output_name must be defined in master_script.sh
cd $PBS_O_WORKDIR
module load openmpi

divisors="1 2 4 6 12"
echo "ciao"
for i in $divisors
do
proc_per_res=$((24/$i))
export OMP_NUM_THREADS=$i
time=`mpirun --map-by ppr:$proc_per_res:node:pe=$i ./$exec_name $mat_size | grep 152`
echo "$n $i $time" >> $output_name
done

# the output is printed with a trailing 152, so as to easily discard all lines of warning
