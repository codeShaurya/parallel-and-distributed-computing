#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include <math.h>
double f(double a){
  return 4.0/(1.0+a*a);
}
void main(int argc, char *argv[])
{
  int my_rank, p, dest;
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  double a = 0,b=1;
  double h = (b-a)/(double)p;
  double area = (f(a+my_rank*h)+f(a+(my_rank+1)*h))*h/2.0;
  double total=0;
  MPI_Reduce(&area,&total,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);

  if (my_rank == 0)
  {
    printf("Value is %.5f",total);
  }
  MPI_Finalize();
}
