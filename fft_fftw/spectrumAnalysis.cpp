#include <fftw3.h>
#include <vector>
#include <iostream>
#include <cmath> // for sin()

enum Axis { X_AXIS = 0, Y_AXIS = 1, Z_AXIS = 2 };
int energySpectra1D(std::vector<double> input, const int nAxis, const int n1, const int n2, Axis axis)
{

  //ys int nAxis = 0;
  //ys if (axis == X_AXIS)
  //ys   nAxis = nx;
  //ys else if (axis == Y_AXIS)
  //ys   nAxis = ny;
  //ys else
  //ys   nAxis = nz;

  const int    halfNplus1 = nAxis / 2 + 1;
  const size_t c_size     = nAxis * n1 * n2;

  // FFT用バッファ
  std::vector<double>       row_data(nAxis);
  std::vector<fftw_complex> row_out(halfNplus1);
  std::vector<fftw_complex> complex_data(halfNplus1 * n1 * n2); // 波数空間の全データ
  //ys   const size_t c_size = (nAxis / 2 + 1) * ((axis == X_AXIS) ? ny * nz : (axis == Y_AXIS) ? nx * nz : nx * ny);

  fftw_plan plan;

  // 各スライスでFFTを実行
  for (int d2 = 0; d2 < n2; ++d2) {
    for (int d1 = 0; d1 < n1; ++d1) {
      // 入力データの抽出
      for (int i = 0; i < nAxis; ++i) {
        if (axis == X_AXIS) {
          // X軸に沿って変動する場合
          // index = z * Ny * Nx + y * Nx + x
          // d2 = z, d1 = y, i = x
          row_data[i] = input[i + nAxis * (d1 + n1 * d2)];
        }
        else if (axis == Y_AXIS) {
          // Y軸に沿って変動する場合
          // index = z * Ny * Nx + y * Nx + x
          // d2 = z, d1 = x, i = y
          //ys           row_data[i] = input[d1 + n1 * (i + nAxis * d2)];
          row_data[i] = input[i * nAxis + d2];
        }
        else if (axis == Z_AXIS) {
          // Z軸に沿って変動する場合
          // index = z * Ny * Nx + y * Nx + x
          // d2 = y, d1 = x, i = z
          row_data[i] = input[i * n1 * n2 + d1 * n2 + d2]; //[z][y][x] index = z * (Ny * Nx) + y * Nx + x
        }
      }

      // FFTプランの作成と実行
      plan = fftw_plan_dft_r2c_1d(nAxis, row_data.data(), row_out.data(), FFTW_MEASURE);
      if (!plan) {
        std::cerr << "FFTW plan creation failed!" << std::endl;
        return 1;
      }
      fftw_execute(plan);

      // 結果を保存
      for (int i = 0; i < halfNplus1; ++i) {
        int idx =   i + halfNplus1 * (d1 + n1 * d2);

        //ys if (axis == X_AXIS) {
        //ys   idx = i + halfNplus1 * (d1 + n1 * d2);
        //ys }
        //ys else if (axis == Y_AXIS) {
        //ys   idx = d1 + n1 * (i + halfNplus1 * d2);
        //ys }
        //ys else if (axis == Z_AXIS) {
        //ys   idx = d2 + n2 * (i + halfNplus1 * d1);
        //ys }
        complex_data[idx][0] = row_out[i][0]/nAxis;
        complex_data[idx][1] = row_out[i][1]/nAxis;
      }

      fftw_destroy_plan(plan);
    }
  }

  // FFT結果の正規化と出力
  for (int d2 = 0; d2 < n2; d2++) {
    std::cout << "start: d2=" << d2 << std::endl;
    for (int d1 = 0; d1 < n1; d1++) {
      std::cout << " start: d1=" << d1 << std::endl;

      // パワースペクトル計算用
      std::vector<double> power_spectrum(halfNplus1, 0.0);

      for (int i = 0; i < halfNplus1; i++) {
        std::cout << "  start: i=" << i << std::endl;

        // FFT結果のインデックス計算

        int idx = i + halfNplus1 * (d1 + n1 * d2);

        // FFT結果の正規化
        //ys complex_data[idx][0] /= nAxis;
        //ys complex_data[idx][1] /= nAxis;

        // FFT結果の表示
        std::cout << "FFT[" << i << "] = " << complex_data[idx][0] << " + i" << complex_data[idx][1] << "\n";

        // パワースペクトル計算
        double real       = complex_data[idx][0];
        double imag       = complex_data[idx][1];
        power_spectrum[i] = (real * real + imag * imag);

        // パワースペクトルの表示
        std::cout << "Power Spectrum for d1 = " << d1 << ", d2 = " << d2 << ":";
        std::cout << "power_spectrum[" << i << "] = " << power_spectrum[i] << "\n";
        std::cout << "  i=" << i << " :done" << std::endl;
      }
      std::cout << " d1=" << d1 << " :done" << std::endl;
    }
    std::cout << "d2=" << d2 << " :done" << std::endl;
  }

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
