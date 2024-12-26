#include <stdio.h>
#include <iostream>
//template <typename T, typename S>
class AnalyticalIdentity;
class AnalyticalF {
  public:
    int _D;
    int _n;

    // n denotes the target dimension
    AnalyticalF(int n, int D):_n(n), _D(D) {
        //static_assert(D == 1 || D == 2 || D == 3, "Only D=1,2,3 allowed.");
        std::cout << "constructor of AnalyticalF" << std::endl;
    }

    using identity_functor_type = AnalyticalIdentity;
    //static constexpr unsigned dim = D;

    virtual bool operator()(float output[], const float input[]) = 0;
};

class AnalyticalIdentity : public AnalyticalF{
  protected:
    AnalyticalF & _f;

  public:
    AnalyticalIdentity(AnalyticalF & f);
    bool operator()(float output[], const float input[]) override;
};


// identity to "store results"

AnalyticalIdentity::AnalyticalIdentity(AnalyticalF & f)
    : AnalyticalF(6,2),_f(f){
    std::cout << "constructor of AnalyticalIdentity" << std::endl;
}

bool AnalyticalIdentity::operator()(float output[], const float input[]) {
    _f(output, input);
    return true;
}


int main() {
    AnalyticalIdentity test(6, 2);
    float output[6];
    float input[2];
    bool e = test(output, input);
    std::cout << e << std::endl;
}
