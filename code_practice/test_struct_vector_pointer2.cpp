#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <vector>
using namespace std;

struct seiseki {
    int no;
    char name[20];
    double heikin;
    // inline seiseki(){
    //     this -> no;
    //     this -> name;
    //     this -> heikin;
    // }
    //inline seiseki(){};
};
vector<seiseki> v_seiseki;
inline vector<seiseki> get_seiseki(vector<seiseki> &seito) {
    //vector<seiseki> v_seiseki;
    //seiseki a = {1, "person1", 80}, b = {2, "person2", 60}, c = {3, "person3", 70};
    // v_seiseki.push_back(a);
    // v_seiseki.push_back(b);
    // v_seiseki.push_back(c);
    cout << "inline &seito" << &seito << endl;
    cout << "inline &v_seiseki" << &v_seiseki <<endl;
    v_seiseki = seito;
    return v_seiseki;
}
int main(int argc, char **argv) {
    //vector<seiseki> seito1 = get_seiseki();
     vector <seiseki>  seitotachi = {
        {1, "vector_person1", 50},
        {2, "vector_person2", 60},
        {3, "vector_person3", 70},
    };
    cout << "&seitotachi" << &seitotachi << endl;
    struct seiseki seito2[] = {
        {1, "strcut_person1", 80},
        {2, "strcut_person2", 90},
        {3, "strcut_person3", 100},
    };
    vector<seiseki> seito1 = get_seiseki(seitotachi);
    cout << "vector_pointer without []" << &seito1 << "\n";
    for (int i = 0; i < 3; i++) {
        cout << "vector_pointer"
             << "[" << i << "]" << &seito1[i] << "\n";
    }

    cout << "struct_pointer without []" << &seito2 << "\n";
    for (int i = 0; i < 3; i++) {
        cout << "struct_pointer"
             << "[" << i << "]" << &seito2[i] << "\n";
    }

    // for (std::vector<seiseki>::const_iterator i = v.begin(); i != v.end();
    // i++) {
    for (std::vector<seiseki>::const_iterator j = seito1.begin();
         j != seito1.end(); j++) {

        printf("%0x\n", j);
        printf("%d\n", j->no);
        printf("%s\n", j->name);
        //printf("%f\n", j->heikin);
        cout << j -> heikin <<endl;
    }

    
    for (int k = 0; k < 3; k++){
        cout << seito2[k].no << "\n" <<
        seito2[k].name <<"\n" << seito2[k].heikin << endl;
    }
    exit(EXIT_SUCCESS);
}