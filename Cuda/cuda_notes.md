
# CUDA

A cuda code is also a CPU code (that run on the host (Jergo in cuda language to say CPU))!

Function Declaration

__host__: normal C function

__global__: called from CPU but executed on GPU

__device_: funciton that works on the GPU and you call it form GPU

to compile: nvcc -o hello hello.cu
to execute: ./hello

global function do not return values! So we have to use a reference.


ls aa.o* | xargs cat $1 > new_file.txt

ls aa.o* | xargs sed 's/a/cristiano/g' $1 | awk '/come/'


module show cudatoolkit (then choose the PATH to samples)