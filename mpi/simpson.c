#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define N 32

double f(double);

int main(int argc, char *argv[]){
  int Procs;
  int my_rank;
  MPI_Status status;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  MPI_Comm_size(MPI_COMM_WORLD, &Procs);
  double  h = 1.0/(double)(Procs+1);
  double a=0,b=1;
  if (my_rank == 0)
  {
    int i;
    for (i = 1; i < Procs; i++){
      double send = a+(double)i*h;
      MPI_Send(&send,1,MPI_DOUBLE,i,0,MPI_COMM_WORLD);
    }
    double sum=f(a)+f(b);//
    for (i = 1; i < Procs; i++){
      double send;
      MPI_Recv(&send,1,MPI_DOUBLE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
      sum+=send;
    }
    printf("%.5f is the value of pi",sum*h/3);
  }
  else
  {
    double recv;
    MPI_Recv(&recv,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD,&status);
    double toSend;
     if (my_rank % 2 == 0)
    {
       toSend= 4*f(recv);
    }else{
       toSend = 2*f(recv);
    }
    MPI_Send(&toSend,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
  }

  MPI_Finalize();
}

double f(double x){
  return 4.0 / (1.0 + x * x);
}
