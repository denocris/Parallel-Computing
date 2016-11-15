#!/bin/bash
#PBS -l walltime=00:40:00


# exec_name and output_name must be defined in master_script.sh
cd $PBS_O_WORKDIR
module load openmpi

export OMP_NUM_THREADS=1
time_serial=`mpirun -np 1 ./$exec_name $mat_size | grep 152`
echo "$time_serial" >> time_serial_size$mat_size.dat


for num_core_per_proc in "1 2 4 5 10"
do
    export OMP_NUM_THREADS=$num_core_per_proc
    proc_per_res=$((20/$i))
    tot_pros=$((20*$num_nods/$i))
    time=`mpirun -np $tot_pros --map-by ppr:$proc_per_res:node:pe=$num_core_per_proc ./$exec_name $mat_size | grep 152`
    echo "$num_nods $num_core_per_proc $tot_pros $time" >> $output_name
done

# the output is printed with a trailing 152, so as to easily discard all lines of warning
