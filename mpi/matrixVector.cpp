//matrix-vector multiplication

#include <iostream>
#include <stdlib.h>
#include "mpi.h"
#define MAXI 100
using namespace std;

int main(int argc, char *argv[])
{
    int np, pid;
    MPI_Status status;

    int row, column;
    int mat[MAXI][MAXI];
    int vec[MAXI], ans[MAXI];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);

    if (pid == 0)
    {
        cout << "Enter Row and column: ";
        cin >> row >> column;
        cout << "Enter Matrix:\n";
        for (int i = 0; i < row; i++)
            for (int j = 0; j < column; j++)
                cin >> mat[i][j];
        cout << "Enter Vector of length " << column << endl;
        for (int i = 0; i < column; i++)
            cin >> vec[i];
    }
    MPI_Bcast(&row, 1, MPI_INT, 0, MPI_COMM_WORLD); //buffer,count,datatype,root,communicator
    MPI_Bcast(&column, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&vec, column, MPI_INT, 0, MPI_COMM_WORLD);

    if (pid == 0)
    {
        for (int i = 1; i < row; i++)
        {
            MPI_Send(&mat[i], column, MPI_INT, i, 1, MPI_COMM_WORLD); //buffer,count,datatype,dest,tag,comm
        }
        int val = 0;

        for (int i = 0; i < column; i++)
            val += mat[0][i] * vec[i];

        ans[0] = val;
        for (int i = 1; i < row; i++)
        {
            int temp;
            MPI_Recv(&temp, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            ans[i] = temp;
        }
        cout << "Answer Vector:\n";
        for (int i = 0; i < column; i++)
            cout << ans[i] << endl;
    }
    if (pid != 0)
    {
        int trow[column];
        MPI_Recv(&trow, column, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        int val = 0;
        for (int i = 0; i < 3; i++)
            val += trow[i] * vec[i];
        MPI_Send(&val, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}