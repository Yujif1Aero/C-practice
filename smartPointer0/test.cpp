#include <iostream>
#include <vector>
#include <utility>

template <typename T>
void process(std::vector<T>& vec)
{
  std::cout << "Lvalue vector: ";
  for (const auto& val : vec) {
    std::cout << val << " ";
  }
  std::cout << std::endl;
}

template <typename T>
void process(std::vector<T>&& vec)
{
  std::cout << "Rvalue vector: ";
  for (const auto& val : vec) {
    std::cout << val << " ";
  }
  std::cout << std::endl;
}
template <typename T>
void wrapperDirect(T&& arg)
{
  process(arg); // 性質を保たず、常に左辺値として渡す
}
//ys 
template <typename T>
void wrapperForward(T&& arg)
{
  process(std::forward<T>(arg)); // 性質を保って転送
}

void func(int&& arg)
{
  std::cout << "Right value reference, pointer = " << &arg << std::endl;
  std::cout << "Right value reference, value = " << arg << std::endl;
}

int main()
{
  std::vector<int> v1 = {1, 2, 3};
  std::vector<int> v2 = v1;
  std::vector<int> v3 = std::move(v1);

  std::cout << "v1 size: " << v1.size() << std::endl;
  std::cout << "v3 size: " << v3.size() << std::endl;

  process(v2);
  process(std::vector<int> {5, 6, 7, 8});
  process(std::move(v2));

  wrapperForward(v2);
  wrapperForward(std::vector<int> {5, 6, 7, 8});
  wrapperDirect(v2);
  wrapperDirect(std::vector<int> {5, 6, 7, 8});

  int   a = 10;
  int&& b = 5;
  std::cout << "pointer a = " << &a << std::endl;
  func(std::move(a));
  std::cout << "pointer b = " << &b << std::endl;
  func(std::move(b));
  func(9);

  int   x = 10;
  int&& r = x; //error
  int&& r = std::move(x); // x を右辺値として扱う
  r       = 20;           // r を通じて x の値を更新
  x       = 30;           // x を更新

  std::cout << "x = " << x << std::endl; // x の値は 20
  std::cout << "r = " << r << std::endl; // r の値も 20（x を参照しているため）

  return 0;
}
