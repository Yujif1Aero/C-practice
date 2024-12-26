#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int main() {
    int jmax = 10, kmax = 10, lmax = 10;
    vector<vector<vector<double>>> x(jmax, vector<vector<double>>(kmax, vector<double>(lmax)));
    vector<vector<vector<double>>> y(jmax, vector<vector<double>>(kmax, vector<double>(lmax)));
    vector<vector<vector<double>>> z(jmax, vector<vector<double>>(kmax, vector<double>(lmax)));

    // ファイルを開く
    ofstream mb("grid.pl3d", ios::out | ios::binary);

    // jmax、kmax、およびlmaxの値を書き込む
    mb.write(reinterpret_cast<char*>(&jmax), sizeof(jmax));
    mb.write(reinterpret_cast<char*>(&kmax), sizeof(kmax));
    mb.write(reinterpret_cast<char*>(&lmax), sizeof(lmax));

    // x、y、およびzの値を書き込む
    for (int l = 0; l < lmax; l++) {
        for (int k = 0; k < kmax; k++) {
            for (int j = 0; j < jmax; j++) {
                mb.write(reinterpret_cast<char*>(&x[j][k][l]), sizeof(double));
            }
        }
    }
    for (int l = 0; l < lmax; l++) {
        for (int k = 0; k < kmax; k++) {
            for (int j = 0; j < jmax; j++) {
                mb.write(reinterpret_cast<char*>(&y[j][k][l]), sizeof(double));
            }
        }
    }
    for (int l = 0; l < lmax; l++) {
        for (int k = 0; k < kmax; k++) {
            for (int j = 0; j < jmax; j++) {
                mb.write(reinterpret_cast<char*>(&z[j][k][l]), sizeof(double));
            }
        }
    }

    // ファイルを閉じる
    mb.close();

    // jmax、kmax、およびlmaxの値を表示する
    cout << "jmax,kmax,lmax   " << jmax << "," << kmax << "," << lmax << endl;

    return 0;
}
