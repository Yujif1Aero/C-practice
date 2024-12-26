#include <iostream>
#include <vector>
using namespace std;

void array1(vector<double> &x) {
    int jmax = 100;
    x.resize(jmax);

    for (int i = 0; i < jmax; i++) {
        x[i] = i * i;
    }
}

int main() {
    vector<double> x;
    //int jmax = 100;
    // x.resize(jmax);

    // for (int i = 0; i < jmax; i++) {
    //     x[i] = i * i;
    // }
    array1(x);
    for (int i = 0, size = x.size(); i < size; i++) {
        cout << x[i] << endl;
    }
    return 0;
}

// void Show(const int* array, int size){
//     for(int i = 0; i < size; ++i){
//         cout << array[i] << ' ';
    
//     }
//     cout << endl;
// }
//  int main(){
//     vector<int> v(5);

//     for(int i = 0, size = v.size(); i < size; ++i){
//         v[i] = i * i;
//     }
    
//     Show(&v[0], v.size());
//  }