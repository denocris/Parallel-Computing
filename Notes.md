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


ls mat_mult_cris.o* | xargs head -n2 | sed '/mat_mult/d' | sed '/Primary/d' | sed '/------/d' > strong.dat

# SHARED MEMORY PROGRAMMING PARADIGM

processes sharing data within the same memory space (therefore no message passing!): they are called threads

negative side effect: 
- our scaling is limited by one single system memory
- is very common tha one thread is writing a message where the another is reading (rescondition)

Processes & Threads

When we have a process than we have also a threads.
How they see the memory ?

Stack: area where tmp data into a function the are stored 
$: ulmit -s unlimited

When we creates differents threads from a process, the threads ahve they private stack memory (every one have their own variables)

Process
Each process provides the resources needed to execute a program. A process has a virtual address space, executable code, open handles to system objects, a security context, a unique process identifier, environment variables, a priority class, minimum and maximum working set sizes, and at least one thread of execution. Each process is started with a single thread, often called the primary thread, but can create additional threads from any of its threads.

Thread
A thread is the entity within a process that can be scheduled for execution. All threads of a process share its virtual address space and system resources. In addition, each thread maintains exception handlers, a scheduling priority, thread local storage, a unique thread identifier, and a set of structures the system will use to save the thread context until it is scheduled. The thread context includes the thread's set of machine registers, the kernel stack, a thread environment block, and a user stack in the address space of the thread's process. Threads can also have their own security context, which can be used for impersonating clients.


USARE: /usr/local/gfortran/bin/gcc hello_world_omp.c -fopenmp



bash -x ./script.sh  // USALO per vedere le varie righe dello script (utile per debug!!!)

GNUPLOT:

set term png
set out "out.png"

p "data.dat" u 1:2 w lp




mpicc  -O3 -o mpi_matmul_allgather.x mpi_matmul_allgather.o -L/u/shared/programs/x86_64/mkl/11.1.3/composer_xe_2013_sp1.3.174/mkl/lib/intel64 -lmkl_intel_lp64 -lmkl_intel_thread -lmkl_core -openmp


CORES_PER_PROCESS=??
PROCESSES_PER_NODE=??
PROCESSES=??
mpirun -np ${PROCESSES} --map-by ppr:${PROCESSES_PER_NODE}:node:pe=${CORES_PER_PROCESS} ./mpi_matmul_allgather.x


mpirun -np 4 --map-by ppr:2:node:pe=10 ./a.out 2048







