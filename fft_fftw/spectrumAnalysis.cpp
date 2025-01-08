#include <fftw3.h>
#include <vector>
#include <iostream>
#include <cmath> // for sin()

enum Axis { X_AXIS = 0, Y_AXIS = 1, Z_AXIS = 2 };
int energySpectra1D(std::vector<double> input, const int nx, const int ny, const int nz, Axis axis)
{

  //ys int nAxis = 0;
  //ys if (axis == X_AXIS)
  //ys   nAxis = nx;
  //ys else if (axis == Y_AXIS)
  //ys   nAxis = ny;
  //ys else
  //ys   nAxis = nz;
  const int nAxis = (axis == X_AXIS) ? nx : (axis == Y_AXIS) ? ny : nz;

  const int halfNplus1 = nAxis / 2 + 1;

  //ys   const size_t c_size = (nAxis / 2 + 1) * ((axis == X_AXIS) ? ny * nz : (axis == Y_AXIS) ? nx * nz : nx * ny);
  size_t c_size   = 0;
  size_t out_size = nAxis / 2 + 1;
  if (axis == X_AXIS) {
    c_size = (size_t)(nx / 2 + 1) * ny * nz;
  }
  else if (axis == Y_AXIS) {
    c_size = (size_t)nx * (ny / 2 + 1) * nz;
  }
  else {
    c_size = (size_t)nx * ny * (nz / 2 + 1);
  }

  size_t real_size = (size_t)nx * ny * nz;

  // 各 Y, Z 点で 1D FFT を個別に計算
  fftw_plan                 plan;
  std::vector<double>       row_data(nAxis);     // 1次元のデータ
  std::vector<fftw_complex> row_out(halfNplus1); // 1次元 FFT 結果
  std::vector<fftw_complex> complex_data(c_size);

  for (int k = 0; k < nz; k++) {
    for (int j = 0; j < ny; j++) {
      // 1Dデータを抽出
      for (int i = 0; i < nAxis; i++) {
        if (axis == X_AXIS) {
          row_data[i] = input[i + nx * (j + ny * k)];
        }
        else if (axis == Y_AXIS) {
          row_data[i] = input[i + nx * (j + ny * k)];
        }
        else if (axis == Z_AXIS) {
          row_data[i] = input[i + nx * (j + ny * k)];
        }
      }

      // FFT プランの作成
      plan = fftw_plan_dft_r2c_1d(nAxis, row_data.data(), row_out.data(), FFTW_MEASURE);
      if (!plan) {
        std::cerr << "FFTW plan creation failed!" << std::endl;
        return 1;
      }

      // FFT 実行
      fftw_execute(plan);
      // 結果を保存
      for (int i = 0; i < halfNplus1; i++) {
        int idx = 0;
        if (axis == X_AXIS) {
          idx = i + halfNplus1 * (j + ny * k);
        }
        else if (axis == Y_AXIS) {
          idx = i + halfNplus1 * (j + ny * k);
        }
        else if (axis == Z_AXIS) {
          idx = i + halfNplus1 * (j + ny * k);
        }
        complex_data[idx][0] = row_out[i][0];
        complex_data[idx][1] = row_out[i][1];
      }

      fftw_destroy_plan(plan);
    }
  }
  for (int k = 0; k < nz; k++) {
    std::cout << "start: k=" << k << std::endl;
    for (int j = 0; j < ny; j++) {
      std::cout << " start: j=" << j << std::endl;
      //ys         std::cout << "Power Spectrum for Y = " << j << ", Z = " << k << ":\n";

      // 各波数成分を計算
      std::vector<double> power_spectrum(halfNplus1, 0.0);
      for (int i = 0; i < halfNplus1; i++) {
        std::cout << "  start: i=" << i << std::endl;
        int idx = i + halfNplus1 * (j + ny * k);
        complex_data[idx][0] /= nAxis;
        complex_data[idx][1] /= nAxis;
        std::cout << "FFT[" << i << "] = " << complex_data[idx][0] << " + i" << complex_data[idx][1] << "\n";
        double real       = complex_data[idx][0];
        double imag       = complex_data[idx][1];
        power_spectrum[i] = (real * real + imag * imag);

        // 結果を出力
        std::cout << "Power Spectrum for Y = " << j << ", Z = " << k << ":";
        std::cout << "power_spectrum[" << i << "] = " << power_spectrum[i] << "\n";
        std::cout << "  i=" << i << " :done" << std::endl;
      }
      std::cout << " j=" << j << " :done" << std::endl;
    }
    std::cout << "k=" << k << " :done" << std::endl;
  }
  //ys // エネルギースペクトルとパワースペクトルを計算
  //ys std::vector<double> energy_spectrum(out_size, 0.0);
  //ys std::vector<double> power_spectrum(out_size, 0.0);
  //ys double              total_magnitude_squared = {};
  //ys double              total_magnitude         = {};

  //ys for (int k = 0; k < out_size; ++k) {
  //ys   double real              = complex_data[k][0];
  //ys   double imag              = complex_data[k][1];
  //ys   double magnitude_squared = real * real + imag * imag;
  //ys   power_spectrum[k]        = magnitude_squared;
  //ys }

  //ys // 結果を出力
  //ys std::cout << "\nFrequency\tPower Spectrum\n";
  //ys for (int k = 0; k < out_size; ++k) {
  //ys   std::cout << k << "\t\t" << power_spectrum[k] << "\n";
  //ys }

  //ys fftw_destroy_plan(plan);

  return 0;
}

int main()
{
  std::cout << "------------------test fftw----------------------" << std::endl;
  {
    const int           Nx    = 5;
    const int           Ny    = 5;
    const int           Nz    = 1;
    double              Lx    = 1.0;
    const int           procs = 1;
    const int           rank  = 0;
    const double        dx = 1.0, dy = 1.0, dz = 1.0;
    std::vector<double> data_cpu_in(Nx * Ny * Nz);

    for (int64_t k = 0; k < Nz; k++) {
      for (int64_t j = 0; j < Ny; j++) {
        for (int64_t i = 0; i < Nx; i++) {
          //ys           int idx = x + Nx * (y + Ny * z);
          double freq = 1.0; /*  */

          double dx     = 1.0 / Nx; // 1 wave divided by number of points
          double dy     = 1.0 / Ny; // 1 wave divided by number of points
          double x_phys = i * dx;
          double y_phys = j * dy;
          double kn     = 2.0 * M_PI * freq;

          data_cpu_in.data()[(k * Ny * Nx) + (j * Nx) + i] = 8 * cos(kn * x_phys);
        }
      }
    }

    for (int64_t k = 0; k < Nz; k++) {
      for (int64_t j = 0; j < Ny; j++) {
        for (int64_t i = 0; i < Nx; i++) {
          auto& value = data_cpu_in[(k * Ny * Nx) + (j * (Nx)) + i];
          std::cout << "data_cpu_in[" << i << "," << j << "," << k << "] = "
                    << "Real: " << value << std::endl;
        }
      }
    }
    //ys Axis axisx = X_AXIS;
    //ys energySpectra1D(data_cpu_in, Nx, Ny, Nz, axisx);
    Axis axisy = Y_AXIS;
    energySpectra1D(data_cpu_in, Ny, Nz, Nx, axisy);
  }
}
