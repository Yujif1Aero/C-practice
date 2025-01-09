///g++ -std=c++20 test.cpp

#include <iostream>

template <auto Func>
void callFunction(int a, int b){
  std::cout << "callFunction : " << Func(a, b) << std::endl;
}

template <typename Func>
void template_callFunction(Func func,int a, int b){
  std::cout << "template_callFunction : " << func(a,b) << std::endl;
}

template <auto& Func>
void template_callFunction(int a, int b){
  std::cout << "template_callFunction without func : " << Func(a,b) << std::endl;
}

template <auto Func,typename... Args>
void template_callFunction(Args&&... args){
  std::cout << "template_callFunction without func and args : " << Func(std::forward<Args>(args)...) << std::endl;

}



struct Adder {
  int operator()(int a, int b) {
    std::cout << "Adderoperator() : " << a+b << std::endl;
    return a + b;
  }
  int operator()(int a, int b) const  {
    std::cout << "Adder operator() : " << a+b << std::endl;
    return a + b;
  }
};
struct Adder_args {
  template <typename... Args>
    int operator()(Args&&... args) {
//ys         int result = (0 + ... + args); // 可変長テンプレート引数を畳み込み演算
    int result = ( ... + args); // 可変長テンプレート引数を畳み込み演算
    std::cout << "Adder_args operator() : " << result << std::endl;
    return result;
    }
   template <typename... Args>
    int operator()(Args&&... args)const {
//ys         int result = (0 + ... + args); // 可変長テンプレート引数を畳み込み演算
    int result = ( ... + args); // 可変長テンプレート引数を畳み込み演算
    std::cout << "Adder_args operator() : " << result << std::endl;
    return result;
    }
};
struct Descending {
  bool operator()(int a, int b) {
    return a > b;
  }
};

struct genricOperator {
  template <typename T>
  bool operator()(T a, T b) {
    return a > b;
  }
};


int add(int a, int b){
  return a + b;
}

int main (){
  
  callFunction<add>(1, 2); // pointer of function as template parameter
  Adder{} (3,4);
  Adder add{};
  template_callFunction<Adder>(Adder{},8, 2); // pointer of templete function as template parameter
  template_callFunction<Adder{}>(5, 2); // pointer of templete function as template parameter
  template_callFunction<Adder_args{}>(5, 2,1); // pointer of templete function as template parameter
  template_callFunction<add>(3,4);
 
  return 0;
}
