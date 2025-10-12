#include <cstdio>
#include <cuda_runtime.h>

__global__ void test_kernel2() {
    int a = threadIdx.x;
    printf("thread %d running\n", a);
}

int main() {

    int device = 0;
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, device);
    printf("executing on %s (sm_%d%d)\n", prop.name, prop.major, prop.minor);

    dim3 grid(1);
    dim3 block(32);

    // <<< >>> means execute test_kernel2() in GPU with grid and block argument.
    test_kernel2<<<grid, block>>>();

    cudaDeviceSynchronize();

    printf("done.\n");
    return 0;
}
