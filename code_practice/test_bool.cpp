#include <iostream>
using namespace std;

bool Input (double &n){
  cin >> n;
  return n >= 0;
}

int main(){
  double n;
  cout << Input(n) << endl;
  
  
}
