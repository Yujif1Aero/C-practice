#include <mpi.h>
#include <hip/hip_runtime.h>
#include <iostream>
#include <vector>

#define CHECK_HIP(call)                                                                                                \
  do {                                                                                                                 \
    hipError_t err = call;                                                                                             \
    if (err != hipSuccess) {                                                                                           \
      std::cerr << "HIP error: " << hipGetErrorString(err) << " at " << __FILE__ << ":" << __LINE__ << std::endl;      \
      MPI_Abort(MPI_COMM_WORLD, 1);                                                                                    \
    }                                                                                                                  \
  } while (0)

__global__ void fill_kernel(int* p, int value) { p[0] = value; }

int main(int argc, char** argv)
{
  MPI_Init(&argc, &argv);

  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (size < 2) {
    if (rank == 0) {
      std::cerr << "Run with at least 2 MPI processes." << std::endl;
    }
    MPI_Finalize();
    return 1;
  }


  int device_count = 0;
  CHECK_HIP(hipGetDeviceCount(&device_count));

  if (device_count == 0) {
    if (rank == 0) {
      std::cerr << "No HIP-capable GPU found." << std::endl;
    }
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  int device_id = 0;
  CHECK_HIP(hipSetDevice(device_id));

  hipDeviceProp_t prop;
  CHECK_HIP(hipGetDeviceProperties(&prop, device_id));

  std::cout << "Rank " << rank << " using GPU " << device_id << ": " << prop.name << std::endl;

  int* d_buf = nullptr;
  CHECK_HIP(hipMalloc(&d_buf, sizeof(int)));

  if (rank == 0) {
    fill_kernel<<<1, 1>>>(d_buf, 12345);
    CHECK_HIP(hipDeviceSynchronize());

    MPI_Send(d_buf, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    std::cout << "Rank 0 sent GPU buffer." << std::endl;
  }
  else if (rank == 1) {
    CHECK_HIP(hipMemset(d_buf, 0, sizeof(int)));

    MPI_Recv(d_buf, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    int h_value = 0;
    CHECK_HIP(hipMemcpy(&h_value, d_buf, sizeof(int), hipMemcpyDeviceToHost));

    std::cout << "Rank 1 received value: " << h_value << std::endl;
  }

  CHECK_HIP(hipFree(d_buf));
  MPI_Finalize();
  return 0;
}
