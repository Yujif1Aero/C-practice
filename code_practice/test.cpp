#include <iostream>
#include <thread>
using namespace std;
 
//# スレッドの中で呼び出す関数
void hoge(int Z, int thrNo)
{
    cout<<"### THREAD#"<<thrNo<<" START : ";
    for(int i=0; i<Z; i++){
        cout << i << " ";
    }
    cout<<endl;
}
 
int main()
{
    //# スレッド起動（関数ポインタをCallableとして起動）
    thread thr1(hoge, 10, 1);
    thread thr2(hoge, 20, 2);
    thread thr3(hoge, 30, 3);
    //# thNの完了を待つ
    thr1.join();   
    thr2.join(); 
    thr3.join(); 
    return 0;
}