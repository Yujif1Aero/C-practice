#ifndef INPUTSTREAM_H_
#define INPUTSTREAM_H_
#include <iostream>
using namespace std;
#include "Stream.h"

class InputStream : public Stream {
  public:
    InputStream();
    InputStream(double n);
  public:
    virtual ~InputStream();
  protected:
    virtual void SetBase();
};

#endif // #ifndef INPUTSTREAM_H_