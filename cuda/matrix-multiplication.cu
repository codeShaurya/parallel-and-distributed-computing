#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 16

_global_ void gpu_matrix_mult(int *a, int *b, int *c, int m, int n, int k)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int sum = 0;
    if (col < k && row < m)
    {
        for (int i = 0; i < n; i++)
        {
            sum += a[row * n + i] * b[i * k + col];
        }
        c[row * k + col] = sum;
    }
}

int main(int argc, char const *argv[])
{
    int m, n, k;
    printf("please type in m n and k\n");
    scanf("%d %d %d", &m, &n, &k);

    // allocate memory in host RAM, h_cc is used to store CPU result
    int *h_a, *h_b, *h_c, *h_cc;
    cudaMallocHost((void **)&h_a, sizeof(int) * m * n);
    cudaMallocHost((void **)&h_b, sizeof(int) * n * k);
    cudaMallocHost((void **)&h_c, sizeof(int) * m * k);
    cudaMallocHost((void **)&h_cc, sizeof(int) * m * k);

    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            h_a[i * n + j] = rand() % 1024;
        }
    }

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < k; ++j)
        {
            h_b[i * k + j] = rand() % 1024;
        }
    }

    int *d_a, *d_b, *d_c;
    cudaMalloc((void **)&d_a, sizeof(int) * m * n);
    cudaMalloc((void **)&d_b, sizeof(int) * n * k);
    cudaMalloc((void **)&d_c, sizeof(int) * m * k);

    cudaMemcpy(d_a, h_a, sizeof(int) * m * n, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, h_b, sizeof(int) * n * k, cudaMemcpyHostToDevice);

    unsigned int grid_rows = (m + BLOCK_SIZE - 1) / BLOCK_SIZE;
    unsigned int grid_cols = (k + BLOCK_SIZE - 1) / BLOCK_SIZE;
    dim3 dimGrid(grid_cols, grid_rows);
    dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);

    gpu_matrix_mult<<<dimGrid, dimBlock>>>(d_a, d_b, d_c, m, n, k);
    cudaMemcpy(h_c, d_c, sizeof(int) * m * k, cudaMemcpyDeviceToHost);
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < k; ++j)
        {
            printf("%d ",h_c[i*m+j]);
        }
        printf("\n");
    }

    // free memory
    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);
    cudaFreeHost(h_a);
    cudaFreeHost(h_b);
    cudaFreeHost(h_c);
    cudaFreeHost(h_cc);
    return 0;
}
