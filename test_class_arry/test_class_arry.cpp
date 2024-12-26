#include <iostream>
using namespace std;

class Integer {
  public:
    int m_value;
    int n_value;
    int l_value;
    Integer(int num1 = 0);
    // Integer(int num);
    void Show();
};

Integer::Integer(int num1) {m_value =num1, n_value = 1, l_value = 10;  }
// Integer::Integer(int num) { m_value = num; }

void Integer::Show() { cout << m_value<< "\t" << n_value << "\t" << l_value << endl; }

int main() {
    Integer a, b;
    Integer c(100);
    //cout << a.m_value << endl;
    const char *name;
    name = "student";
    a.Show();
    b.Show();
    c.Show();
}