#!/bin/bash
#PBS -l walltime=00:30:00


# exec_name and output_name must be defined in master_script.sh
cd $PBS_O_WORKDIR
module load openmpi

export OMP_NUM_THREADS=1
time_serial=`mpirun -np 1 ./$exec_name $mat_size | grep 152`
echo "$time_serial" >> time_serial_size$mat_size.dat

divisors="1 2 4 5 10"
for i in $divisors
do
proc_per_res=$((10/$i))
export OMP_NUM_THREADS=$i
time=`mpirun --map-by ppr:$proc_per_res:socket:pe=$i ./$exec_name $mat_size | grep 152`
echo "$n $i $time" >> $output_name
done

# the output is printed with a trailing 152, so as to easily discard all lines of warning
