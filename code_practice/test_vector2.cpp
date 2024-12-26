#include <iostream>
#include <vector>
#include <string>

int main(int argc, char* *argv)
{
    using namespace std;
    cout << argv[0] << "\n" //各項目の文字列
        << *argv[1] << "\n" //先頭の文字列
        << argv << "\n" //argv[]先頭のアドレス
        << *argv << "\n"
        << **argv << "\n"
        << &argv[1] << endl;
    vector<string> args(argv, argv + argc);
    for (const auto& arg : args) {
        cout << "\nfor\n" << &arg << "\n" << arg  << "\n" ;
        printf("%s \nend for \n \n",arg.c_str());
        //break;argc回しか回らない
    }
    cout << &args << "\n";
    cout << args[1] << "\n";
    cout << &argc << "\n";
    cout << *argv << "\n";
    cout << argv << "\n";
    cout << argv + argc << "\n";
    int i;

     for (int i = 0; i < argc; ++i) {
        cout << i << "\n"
        <<argv[i] << endl;
    }
    return 0;
}














