#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include <math.h>

void main(int argc, char *argv[])
{
  int my_rank, p, dest;
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  int n = 8;
  int x = 2; // x
  int my_step = 0;
  int total_step = log2(p);
  int arr[] = {2, 1, 1, 1, 1, 1, 1, 1};
  int mask = my_rank % 2;

  int ans = arr[my_rank];
  while (my_step < total_step)
  {
    if (mask == 1)
    {
      ans *= x;
    }
    x *= x;
    my_step++;
    if (my_rank < n / 2)
    {
      dest = 2 * my_rank;
    }
    else
    {
      dest = 2 * my_rank - n + 1;
    }
    int new_mask = mask;
    if (my_rank != 0 && my_rank != p - 1)
    {
      MPI_Send(&mask, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
      MPI_Recv(&new_mask, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    mask = new_mask;
  }
  if (my_rank != 0)
  {
    MPI_Send(&ans, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
  }
  else
  {
    int temp;
    for (int i = 1; i < p; i++)
    {
      MPI_Recv(&temp, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
      ans += temp;
    }
    printf("The value of polynomial is %d\n", ans);
  }
  MPI_Finalize();
}
