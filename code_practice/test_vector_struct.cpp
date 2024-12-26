#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <vector>
using namespace std;

struct foo {
    int x;
    int y;
    int z;
};
inline vector<foo> get_v(const bool print_info) {
    vector<foo> v;
    if (print_info) {
        foo a = {9, 8, 7}, b = {3, 2, 3};//, c = {-1, -2, -3};

        v.push_back(a);
        v.push_back(b);
        //v.push_back(c);
        return v;
    } else {
        cout << "FALSE" << endl;
        // break;
        exit(1);
    }
}
int main(int argc, char **argv) {
    void ch();
    // vector<foo> v = get_v(true);
    vector<foo> vl = get_v(true);

    // for (auto d : v) {cout << d<< endl;}

    for (std::vector<foo>::const_iterator i = vl.begin(); i != vl.end(); i++) {

        printf("%0x\n", i);
        printf("%d\n", i->x);
        printf("%d\n", i->y);
        printf("%d\n", i->z);
    }
    cout << &vl << endl;
    cout << &vl[0] << endl;
    for (size_t i = 0; i < vl.size(); ++i) {
        

        cout << &vl[i] << endl;
        cout << vl.size() << "\n" << i << "\n" << endl;
       // cout << v[i].size() << "\n" << i << "\n" << endl;
    }

    exit(EXIT_SUCCESS);
}
