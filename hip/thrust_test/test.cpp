#include <thrust/device_vector.h>
#include <thrust/fill.h>
#include <hip/hip_runtime.h>
#include <iostream>

int main() {
    std::cout << "1. Initializing HIP..." << std::endl;
    hipError_t err = hipInit(0);
    if (err != hipSuccess) {
        std::cerr << "HIP Init Failed: " << hipGetErrorString(err) << std::endl;
        return 1;
    }

    std::cout << "2. Setting Device 0..." << std::endl;
    hipSetDevice(0);

    std::cout << "3. Testing raw HIP malloc..." << std::endl;
    void* ptr;
    err = hipMalloc(&ptr, 1024);
    if (err != hipSuccess) {
        std::cerr << "Raw hipMalloc failed: " << hipGetErrorString(err) << std::endl;
        return 1;
    }
    hipFree(ptr);
    std::cout << "   Raw malloc success." << std::endl;

    std::cout << "4. Testing Thrust uint8_t device_vector..." << std::endl;
    try {
        thrust::device_vector<std::uint8_t> test_uint(100, 0);
        std::cout << "   Thrust allocation success." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "   Thrust Exception: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "ALL TESTS PASSED." << std::endl;
    return 0;
}
