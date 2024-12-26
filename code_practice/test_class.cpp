#include <iostream>
inline void ch(int a) {
	std::cout << "chk" << a << "\n";
}
class SuperClass {
  private:
    int a = 1;

  public:
    int b = 2;
    int d = 3;

  protected:
    int c = 3;
    int get() { return this->a; }
};

//派生クラスSUPER
class SubClass : public SuperClass {
  public:
    //int get2() { return SuperClass::get(); }
    int get2(){ return SuperClass::d;}
};

//inline SubClass select_number(& num){
//  cout << num <<endl;
//}
int main(void) {

    SubClass s;
    s.b = 5;
    ch(s.b);
    std::cout << s.get2() << "\n";
    std::cout << s.b <<"\n";
    s.d = 6;
    std::cout << s.d <<"\n";
    return 0;
} 