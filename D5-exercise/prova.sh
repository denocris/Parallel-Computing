#!/bin/bash

ppn=16

for nodes in `seq 1 6`
do

target=tmp_`echo $nodes`_`echo $ppn`
cat inizio.sh > $target
echo "#PBS -l nodes=$nodes:ppn=$ppn" >> $target
echo "module load openmpi" >> $target
echo 'cd $PBW_O_WORKDIR' >> $target
echo "mpirun -np $(($ppn*$nodes)) -npernode $ppn ./a.out" >> $target
qsub $target
done