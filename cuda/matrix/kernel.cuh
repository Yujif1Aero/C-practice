#pragma once
#include <cstdio>

#define CALVECTODOTRPRODUCT
#define BLOCK_SIZEx 1024
#define VECTORNUMx (2048 * 1000)
#define GRID_SIZEx ((VECTORNUMx + BLOCK_SIZEx - 1) / BLOCK_SIZEx)

template<typename T>
__device__ void MatrixMultiplicationKernelT(
    const T* A, const T* B, T* C,
    int* rowm, int* colummn, int* rowp, int* colummq)
{
    int bx = blockIdx.x;
    int tx = threadIdx.x;

    int indexA = bx * blockDim.x + tx;

    T sum = (T)0.0;
    __shared__ T Cresult[BLOCK_SIZEx];

    while (indexA < *colummn) {
        sum += A[indexA] * B[indexA];
        indexA += blockDim.x * gridDim.x;
    }
    Cresult[tx] = sum;
    __syncthreads();

    if (tx == 0) {
        T CresultBlock = (T)0.0;
        for (int k = 0; k < blockDim.x; ++k) {
            CresultBlock += Cresult[k];
        }
        atomicAdd(C, CresultBlock);
    }
}


extern "C" __global__
void MatrixMultiplicationKernel(const void* A, const void* B, void* C,
                                int* rowm, int* colummn, int* rowp, int* colummq, int* size)
{
    if (*size == sizeof(float)) {
        MatrixMultiplicationKernelT<float>(
            (const float*)A, (const float*)B, (float*)C, rowm, colummn, rowp, colummq);
    }
}
