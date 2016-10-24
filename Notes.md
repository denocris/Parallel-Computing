# PARALLEL PROGRAMMING

- distributed memory: multiple processes, in one or multiple systems, which are all made to solve one single problem!


SPMD: single program multiple data

MPI is a library, has a frame called mpirun 

latency: time to create and send the message between processes 

point to point communication: one process of given set sends a message to a process of the same set

collective communication: I have all processes in a given set that are involved in a given communication 

mpicc -c name_prog.c (to compile)

mpicc -o name_prog.x name_prog.o

mpirun -np 5 ./name_prog.x


NEXT = (rank +1)%size

PREC = (rank + size -1)%size

MPI_Reduce + MPI_Bcast = MPI_Allreduce


NPE = 4, NSTEP = NPE -1

//loc to glob index 

L_idx = 1
loc_size = size/num_process
G_idx = (loc_size * mype) + idx



Poin - 2 - Point function

- Send/Recv
- Isend/Irecv
- SendRecv

Collective function ( 1 to N, or N to 1)

- MPI_Bcast
- MPI_Reduce
- MPI_Gather/Scatter

Collective function ( N to N )

- MPI_All...
- MPI_All...v


The collective function which ends with "v" ( MPI_Allreducev( ) ), are those that handle the rest.


_________________________________________________________________

I/O: input/output

Granularity: how many pieces the program can be split into

Distributed Data vs Replicated data (Domain size is the same for every p) (?? check ??)

MPI_BCAST (?? check ??)

You must always pack tha data to be contigous (in C or Fortran this is different)




















