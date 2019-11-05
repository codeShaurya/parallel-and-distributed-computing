#include <stdio.h>
#include <cuda.h>
#include <stdlib.h>

__global__ void gpu_matrixadd(int *a, int *b, int *c, int N)
{

  int col = threadIdx.x + blockDim.x * blockIdx.x;
  int row = threadIdx.y + blockDim.y * blockIdx.y;

  int index = row * N + col;

  if (col < N && row < N)
    c[index] = a[index] + b[index];
}

void cpu_matrixadd(int *a, int *b, int *c, int N)
{

  int index;
  for (int col = 0; col < N; col++)
    for (int row = 0; row < N; row++)
    {
      index = row * N + col;
      c[index] = a[index] + b[index];
    }
}

int main(int argc, char *argv[])
{

  char key;

  int i, j; // loop counters

  int Grid_Dim_x = 1, Grid_Dim_y = 1;   //Grid structure values
  int Block_Dim_x = 1, Block_Dim_y = 1; //Block structure values

  int noThreads_x, noThreads_y; // number of threads available in device, each dimension
  int noThreads_block;          // number of threads in a block

  int N = 10; // size of array in each dimension
  int *a, *b, *c, *d;
  int *dev_a, *dev_b, *dev_c;
  int size; // number of bytes in arrays

  cudaEvent_t start, stop; // using cuda events to measure time
  float elapsed_time_ms;   // which is applicable for asynchronous code also

  /* --------------------ENTER INPUT PARAMETERS AND DATA -----------------------*/

  do
  { // loop to repeat complete program

  __global__ void input_parameter(sizeof(x), sizeof(y), nub_block))


		x = (int*) malloc(size);		//this time use dynamically allocated memory for arrays on host
  y = (int *)malloc(size);
  nub_block = (int *)malloc(size); // results from GPU
                                   // results from CPU

  for (i = 0; i < N; i++) // load arrays with some numbers
    for (j = 0; j < N; j++)
    {
      a[i * N + j] = i;
      b[i * N + j] = i;
    }
  }
  }
