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



_________________________________________________________________

code with pointers

void loc_matmul( double *A, double * B, double * C, int loc_size)

the following is the normal mat mul

int i, j, k;
for (i=0, i < loc_size;i++)
for (j=0, j < loc_size;j++)
for (k=0, k < loc_size;k++)
C[(i*size)+j] += A[(i*size) + k]*B[(k*size) + j]

Let us think in the case of:

count = 0

Initially my_block =  rank;

A_block = A + (rank * loc_size);
B_block = B;
C_block = C;

for(count=0; count<nprocs; count++){
    



loc_matmul( A_block, B_block, C_block, int loc_size)
    B_block += loc_size;
    C_block += loc_size
    

}

















