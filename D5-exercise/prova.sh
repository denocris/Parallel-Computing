#!/bin/bash

#ppn=18
nodes=1

#for nodes in `seq 1 1`
for ppn in $(seq 2 2 10);
do

target=tmp_`echo $nodes`_`echo $ppn`
cat inizio.sh > $target
echo "#PBS -l nodes=$nodes:ppn=20" >> $target
echo "module load openmpi" >> $target
#echo 'cd $PBS_O_WORKDIR' >> $target
echo 'cd /home/cdenobi/P1.3_seed/D5-exercise' >> $target
echo "mpirun -np $(($ppn*$nodes)) -npernode $ppn ./mm.out 48" >> $target
qsub $target
done