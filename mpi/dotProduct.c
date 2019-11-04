#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int tempa1[1000];
int tempa2[1000];
int main(int argc, char *argv[])
{
  int n = 10;
  int x[n], y[n];
  int total = 0;
  int mytotal = 0;
  int i, myid, p, index;
  int element_per_process, elements_left, elements_recieved;

  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  if (myid == 0)
  {
    for (int j = 0; j < n; j++)
    {
      x[j] = j + 1;
      y[j] = x[j];
    }
    int rem = n - (n / p) * p;
    element_per_process = n / p;
    if (p > 1)
    {
      for (i = 1; i < p - 1; i++)
      {
        index = i * element_per_process;
        MPI_Send(&element_per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        MPI_Send(&x[index], element_per_process, MPI_INT, i, 0, MPI_COMM_WORLD);
        MPI_Send(&y[index], element_per_process, MPI_INT, i, 0, MPI_COMM_WORLD);
      }
      index = i * element_per_process;
      elements_left = n - index;
      MPI_Send(&elements_left, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      MPI_Send(&x[index], elements_left, MPI_INT, i, 0, MPI_COMM_WORLD);
      MPI_Send(&y[index], elements_left, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    for (int j = 0; j < element_per_process; j++)
    {
      total = total + x[j] * y[j];
    }
    int tmp;
    for (int j = 1; j < p; j++)
    {
      MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
      total = total + tmp;
    }
    printf("dot product is %d", total);
  }
  else
  {
    MPI_Recv(&elements_recieved, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(&tempa1, elements_recieved, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(&tempa2, elements_recieved, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    for (int j = 0; j < elements_recieved; j++)
    {
      mytotal += tempa1[j] * tempa2[j];
    }
    MPI_Send(&mytotal, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}