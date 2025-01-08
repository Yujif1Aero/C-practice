//nvcc testcuFFT.cu -o testcuFFT -lcufft
#include <iostream>
#include <vector>
#include <complex>
#include <cmath> // for sin()
#include <cuda_runtime.h>
#include <cufft.h>
#include <iostream>

int main()
{
    std::cout << "------------------test complex----------------------" << std::endl;
  {
    const int                         Nx    = 5;
    const int                         Ny    = 5;
    const int                         Nz    = 5;
    double                            Lx    = 1.0;
    const int                         procs = 1;
    const int                         rank  = 0;
    const double                      dx = 1.0, dy = 1.0, dz = 1.0;
    std::vector<std::complex<double>> data_cpu_in(Nx * Ny * (Nz + 2));

    for (int64_t k = 0; k < Nz; k++) {
      for (int64_t j = 0; j < Ny; j++) {
        for (int64_t i = 0, l = ((Nx / procs) * rank); i < (Nx / procs), l < ((Nx / procs) * (rank + 1)); i++, l++) {
          //ys           int idx = x + Nx * (y + Ny * z);
          double freq = 1.0; /*  */

          double dx     = 1.0 / Nx; // 1 wave divided by number of points
          double x_phys = i * dx;
          double kn     = 2.0 * M_PI * freq;

          reinterpret_cast<double*>(data_cpu_in.data())[(k * Ny * (Nz + 2)) + (j * (Nz + 2)) + i] =
              //ys 8 * ((sin(1.0 * l * dx) * sin(2.0 * j * dy) * sin(3.0 * k * dz)) +
              //ys      (sin(4.0 * l * dx) * sin(5.0 * j * dy) * sin(6.0 * k * dz)));
              8 * sin(kn * x_phys);
        }
      }
    }

    for (int64_t k = 0; k < Nz; k++) {
      for (int64_t j = 0; j < Ny; j++) {
        for (int64_t i = 0, l = ((Nx / procs) * rank); i < (Nx / procs), l < ((Nx / procs) * (rank + 1)); i++, l++) {
          auto& value = data_cpu_in[(k * Ny * (Nz + 2)) + (j * (Nz + 2)) + i];
          std::cout << "data_cpu_in[" << i << "," << j << "," << k << "] = "
                    << "Real: " << value.real() << ", Imag: " << value.imag() << std::endl;
        }
      }
    }

    std::cout << "----------------------------------------" << std::endl;
    for (int64_t k = 0; k < Nz; k++) {
      for (int64_t j = 0; j < Ny; j++) {
        for (int64_t i = 0, l = ((Nx / procs) * rank); i < (Nx / procs), l < ((Nx / procs) * (rank + 1)); i++, l++) {

          double freq = 1.0; /*  */

          double dx     = 1.0 / Nx; // 1 wave divided by number of points
          double x_phys = i * dx;
          double kn     = 2.0 * M_PI * freq;

          data_cpu_in[(k * Ny * (Nz + 2)) + (j * (Nz + 2)) + i].real(
              //ys 8 * ((sin(1.0 * l * dx) * sin(2.0 * j * dy) * sin(3.0 * k * dz)) +
              //ys      (sin(4.0 * l * dx) * sin(5.0 * j * dy) * sin(6.0 * k * dz)));
              8 * sin(kn * x_phys));
          data_cpu_in[(k * Ny * (Nz + 2)) + (j * (Nz + 2)) + i].imag(0.0);
        }
      }
    }
    for (int64_t k = 0; k < Nz; k++) {
      for (int64_t j = 0; j < Ny; j++) {
        for (int64_t i = 0, l = ((Nx / procs) * rank); i < (Nx / procs), l < ((Nx / procs) * (rank + 1)); i++, l++) {

          auto& value = data_cpu_in[(k * Ny * (Nz + 2)) + (j * (Nz + 2)) + i];
          std::cout << "data_cpu_in[" << i << "," << j << "," << k << "] = "
                    << "Real: " << value.real() << ", Imag: " << value.imag() << std::endl;
        }
      }
    }

    std::cout << "---------------------test FFT----------------" << std::endl;
  }

  {
    const int    Nx = 120.0;
    const int    Ny = 2.0;
    const int    Nz = 2.0;
    const double Lx = 1.0;
    //     index = x + Nx*(y + Ny*z)
    cufftReal* data_cpu = new cufftReal[Nx * Ny * Nz];
    for (int z = 0; z < Nz; ++z) {
      for (int y = 0; y < Ny; ++y) {
        for (int x = 0; x < Nx; ++x) {
          int idx = x + Nx * (y + Ny * z);

          double freq = 240.0; /*  */

          double dx     = 1.0 / Nx; // 1 wave divided by number of points
          double x_phys = x * dx;
          double k      = 2.0 * M_PI * freq;

          //ys         data_cpu[idx] = 8.0f * cos((2.0 * M_PI *freq) * Lx / Nx * x);

          data_cpu[idx] = 8.0f * cos(k * x_phys);
          std::cout << "data_cpu[" << x << "][" << y << "][" << z << "] = " << data_cpu[idx] << std::endl;
        }
      }
    }

    cufftReal* data_in_gpu;
    cudaMalloc((void**)&data_in_gpu, Nx * Ny * Nz * sizeof(cufftReal));
    cudaMemcpy(data_in_gpu, data_cpu, Nx * Ny * Nz * sizeof(cufftReal), cudaMemcpyHostToDevice);

    cufftComplex* data_out_gpu;
    cudaMalloc((void**)&data_out_gpu, (Nx / 2 + 1) * Ny * Nz * sizeof(cufftComplex));

    cufftHandle plan;
    cufftPlan1d(&plan, Nx, CUFFT_R2C, Ny * Nz);

    cufftExecR2C(plan, data_in_gpu, data_out_gpu);

    cufftComplex* result = new cufftComplex[(Nx / 2 + 1) * Ny * Nz];
    cudaMemcpy(result, data_out_gpu, (Nx / 2 + 1) * Ny * Nz * sizeof(cufftComplex), cudaMemcpyDeviceToHost);

    for (int by = 0; by < Ny; ++by) {
      for (int bz = 0; bz < Nz; ++bz) {
        int batchIndex = by + Ny * bz;
        std::cout << "=== batch (y=" << by << ", z=" << bz << ") ===" << std::endl;

        for (int k = 0; k < Nx / 2 + 1; ++k) {

          int idx = batchIndex * (Nx / 2 + 1) + k;

          float re = result[idx].x / Nx;
          float im = result[idx].y / Nx;

          std::cout << "  k=" << k << "  Re=" << re << "  Im=" << im << std::endl;
        }
      }
    }

    double E_time = 0.0;
    for (int n = 0; n < Nx * Ny * Nz; ++n) {
      E_time += (data_cpu[n] * data_cpu[n]);
    }

    double E_freq = 0.0;

    for (int by = 0; by < Ny; ++by) {
      for (int bz = 0; bz < Nz; ++bz) {
        int batchIndex = by + Ny * bz;
        for (int k = 0; k < Nx / 2 + 1; ++k) {
          int    idx  = batchIndex * (Nx / 2 + 1) + k;
          double re   = result[idx].x;
          double im   = result[idx].y;
          double mag2 = re * re + im * im;

          E_freq += 2.0 * mag2;
        }
      }
    }

    E_freq /= Nx;

    std::cout << "[Time domain energy] E_time = " << E_time << std::endl;
    std::cout << "[Freq domain energy] E_freq = " << E_freq << std::endl;

    cufftDestroy(plan);
    cudaFree(data_in_gpu);
    cudaFree(data_out_gpu);
    delete[] data_cpu;
    delete[] result;

    return 0;
  }
}
