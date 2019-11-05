#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>

#define RAND_MAX_GEN 50.0

double random_double();
double *generate_random(size_t num, int rank);
double find_sum(double *numbers, size_t size);
void start_find_sum(int rank, int size, double *random_numbers, size_t num_per_proc, double *overall_sum);
void start_find_psum(int rank, int size, double *random_numbers, size_t num_per_proc, double sum);

int main(int argc, char **argv)
{
  int rank, size;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Barrier(MPI_COMM_WORLD);

  size_t num = 4;
  size_t pos = 3;

  size_t num_per_proc = num / size;
  double *random_numbers = generate_random(num_per_proc, rank);

  double sum;

  start_find_sum(rank, size, random_numbers, num_per_proc, &sum);
  start_find_psum(rank, size, random_numbers, num_per_proc, sum);
  MPI_Barrier(MPI_COMM_WORLD);

  size_t start_pos = num_per_proc * rank;
  size_t end_pos = num_per_proc * (rank + 1) - 1;
  if (pos >= start_pos && pos <= end_pos)
  {
    printf("Prefix sum number %zu is %lf\n", pos, random_numbers[pos - start_pos]);
  }

  free(random_numbers);
  MPI_Finalize();
  return 0;
}

double random_double()
{
  return (double)random() / ((double)RAND_MAX + (double)1);
}

double *generate_random(size_t num, int rank)
{
  srandom(time(NULL) + rank);
  double *random_numbers = (double *)malloc(sizeof(double) * num);
  for (size_t i = 0; i < num; i++)
  {
    random_numbers[i] = random_double() * RAND_MAX_GEN;
    printf("%.5f  + ", random_numbers[i]);
  }
  return random_numbers;
}

double find_sum(double *numbers, size_t size)
{
  double sum = 0.0;
  for (size_t i = 0; i < size; i++)
    sum += numbers[i];
  return sum;
}

void start_find_sum(int rank, int size, double *random_numbers, size_t num_per_proc, double *overall_sum)
{
  MPI_Status status;

  double sum = find_sum(random_numbers, num_per_proc);

  int still_alive = 1;
  int level;

  for (level = 0; level < (int)log2(size); level++)
  {
    if (still_alive)
    {
      int position = rank / (int)pow(2, level);

      if (position % 2 == 0)
      {
        // I am a receiver
        double sender_sum;
        int sending_rank = rank + (int)pow(2, level);

        MPI_Recv(&sender_sum, 1, MPI_DOUBLE, sending_rank, 0, MPI_COMM_WORLD, &status);

        sum += sender_sum;
      }

      else
      {
        // I am a sender
        int receiving_rank = rank - (int)pow(2, level);

        MPI_Send(&sum, 1, MPI_DOUBLE, receiving_rank, 0, MPI_COMM_WORLD);
        still_alive = 0;
      }
    }

    MPI_Barrier(MPI_COMM_WORLD);
  }

  *overall_sum = sum;
}

void start_find_psum(int rank, int size, double *random_numbers, size_t num_per_proc, double sum)
{
  double psum;
  int level;
  MPI_Status status;

  if (rank == 0)
  {
    psum = sum;
  }
  for (level = (int)log2(size) - 1; level >= 0; level--)
  {
    if (level == 0 || rank % (int)pow(2, level) == 0)
    {
      int position = rank / (int)pow(2, level);

      if (position % 2 == 0)
      {
        double sender_sum;
        int sending_rank = rank + (int)pow(2, level);

        MPI_Send(&psum, 1, MPI_DOUBLE, sending_rank, 0, MPI_COMM_WORLD);

        MPI_Recv(&sender_sum, 1, MPI_DOUBLE, sending_rank, 0, MPI_COMM_WORLD, &status);

        psum -= sender_sum;
      }

      else
      {
        int receiving_rank = rank - (int)pow(2, level);

        MPI_Recv(&psum, 1, MPI_DOUBLE, receiving_rank, 0, MPI_COMM_WORLD, &status);

        MPI_Send(&sum, 1, MPI_DOUBLE, receiving_rank, 0, MPI_COMM_WORLD);
      }
    }

    MPI_Barrier(MPI_COMM_WORLD);
  }

  double next_sum = random_numbers[num_per_proc - 1];
  random_numbers[num_per_proc - 1] = psum;

  int j;
  for (j = num_per_proc - 2; j >= 0; j--)
  {
    double next_sum_tmp = random_numbers[j];
    random_numbers[j] = random_numbers[j + 1] - next_sum;
    next_sum = next_sum_tmp;
  }
}
