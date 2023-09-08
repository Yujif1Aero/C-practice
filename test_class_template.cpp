#include <iostream>
#include <string>
using namespace std;
const int MAX_NAME = 16;

template <typename TYPE>
class Student {
public:
  // char name[MAX_NAME + 1];
  //char *name;
  string name;
  int scoreJapanese;
  int scoreMath;
  TYPE scoreEnglish;
  Student(string studentname, int japanese, int math, TYPE english);
  Student();
  template<typename UYPE>
  UYPE Show(UYPE a);
};

template <typename TYPE>
Student<TYPE>::Student(string studentname, int japanese, int math,
                 TYPE english) {
    name =studentname, scoreJapanese = japanese, scoreMath = math,
    scoreEnglish = english;
}

template <typename TYPE>
Student<TYPE>::Student() {
    name = "student1", scoreJapanese = 100, scoreMath = 90, scoreEnglish = 80;
}

template <typename TYPE>
template <typename UYPE>
UYPE Student<TYPE>::Show(UYPE a) {
    cout << "name：" << name << endl
         << "Japanese：" << scoreJapanese << endl
         << "Math：" << scoreMath << endl
         << "English：" << scoreEnglish << endl;
    cout << a << endl;
    return a;
}


int main() {
   
   int arry_test[5] = {1 ,3, 4, };
   string hoge = "hogehoge";
   Student<int> *student;
   Student<int> student1[6] = { 
   Student("yamada", 10, 20 , 30),
   Student("tanaka", 40, 50, 60),
   Student("yamazaki", 70, 80 ,90),
   };
   student = &student1[1];
   

   cout << (student1+1)->template Show<double>(10.1) << endl;
   cout << student1[1]. template Show<int>(99) << endl;
   cout << student1[0].Show(hoge) << endl;
   cout << student->Show<string>(hoge) << endl;
   int size = sizeof (arry_test) / sizeof (int);
   cout << size <<endl; 
   for(int i = 0; i < size; ++i ){
      cout << arry_test[i] << endl;
    }
}

//#include <iostream>
// #include <typeinfo>

// template <typename T>
// class MyClass {
// public:
//     MyClass(T value) : data(value) {}

//     // 関数の定義（宣言）
//     template <typename U>
//     U postProcess(U value);

// private:
//     T data;
// };

// // 関数の実装
// template <typename T>
// template <typename U>
// U MyClass<T>::postProcess(U value) {
//     std::cout << "Data: " << data << ", Processing with type " << typeid(U).name() << std::endl;
//     return value; // 適切な戻り値を返す
// }

// int main() {
//     MyClass<int> intObj(42);
//     MyClass<double> doubleObj(3.14);

//     int intValue = 10;
//     double doubleValue = 2.718;

//     //    int result1 =
//       std::cout << intObj.template postProcess<int>(intValue) << std::endl;
//       // double result2 =
//       std::cout << doubleObj.template postProcess<double>(doubleValue) << std::endl;

//     // TとUの型を別々に指定
//     //double result3 =
//       std::cout << intObj.template postProcess<double>(doubleValue) << std::endl;
//       //int result4 =
//       std::cout << doubleObj.template postProcess<int>(intValue) << std::endl;

//     return 0;
// }
