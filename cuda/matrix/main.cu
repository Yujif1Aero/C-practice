#include <vector>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cuda_runtime.h>
#include "kernel.cuh"

template <typename T>
void GPUMatrixMultiplication(const std::vector<T>& A, const std::vector<T>& B, T& C)
{
  int m = 1;
  int n = VECTORNUMx;
  int p = n;
  int q = 1;

  int *d_rowm, *d_colummn, *d_rowp, *d_colummq, *d_size;
  T *  d_A, *d_B, *d_C;

  int sizeA = m * n * sizeof(T);
  int sizeB = p * q * sizeof(T);
  int sizeC = m * q * sizeof(T);

  cudaMalloc(&d_A, sizeA);
  cudaMalloc(&d_B, sizeB);
  cudaMalloc(&d_C, sizeC);
  cudaMemset(d_C, 0, sizeC);

  cudaMalloc(&d_rowm, sizeof(int));
  cudaMalloc(&d_colummn, sizeof(int));
  cudaMalloc(&d_rowp, sizeof(int));
  cudaMalloc(&d_colummq, sizeof(int));
  cudaMalloc(&d_size, sizeof(int));

  int rowm = m, colummn = n, rowp = p, colummq = q, size = sizeof(T);
  // --- data transfer CPU->GPU ---
  cudaMemcpy(d_A, A.data(), sizeA, cudaMemcpyHostToDevice);
  cudaMemcpy(d_B, B.data(), sizeB, cudaMemcpyHostToDevice);
  cudaMemcpy(d_rowm, &rowm, sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(d_colummn, &colummn, sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(d_rowp, &rowp, sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(d_colummq, &colummq, sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(d_size, &size, sizeof(int), cudaMemcpyHostToDevice);

  // execute kernel
  dim3 grid(GRID_SIZEx);
  dim3 block(BLOCK_SIZEx);

  MatrixMultiplicationKernel<<<grid, block>>>(d_A, d_B, d_C, d_rowm, d_colummn, d_rowp, d_colummq, d_size);

  cudaDeviceSynchronize();

  // --- data transfer GPU->CPU ---
  cudaMemcpy(&C, d_C, sizeof(T), cudaMemcpyDeviceToHost);

  cudaFree(d_A);
  cudaFree(d_B);
  cudaFree(d_C);
  cudaFree(d_rowm);
  cudaFree(d_colummn);
  cudaFree(d_rowp);
  cudaFree(d_colummq);
  cudaFree(d_size);
}

int main()
{
  printf(">> executing on CUDA\n");

  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, 0);
  printf("executing on %s (sm_%d%d)\n", prop.name, prop.major, prop.minor);

  srand((unsigned)time(NULL));

  std::vector<float> A(VECTORNUMx);
  std::vector<float> B(VECTORNUMx);
  float              C = 0.0f;

  for (int i = 0; i < VECTORNUMx; ++i) {
    A[i] = static_cast<float>(rand()) / RAND_MAX - 0.5f;
    B[i] = static_cast<float>(rand()) / RAND_MAX - 0.5f;
  }

  GPUMatrixMultiplication(A, B, C);

  printf("Dot product (AdotB) = %f\n", C);
  return 0;
}
