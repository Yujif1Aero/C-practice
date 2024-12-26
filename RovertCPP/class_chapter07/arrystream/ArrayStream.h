#ifndef ARRAYSTREAM_H_
#define ARRAYSTREAM_H_

#include "Stream.h"

class ArrayStream : public Stream {
  public:
    ArrayStream(const double *array, int size);
   virtual ~ArrayStream();

  protected:
    virtual void SetBase();
  
  private:
    double *m_array;
    int m_i;
};

#endif // ARRAYSTREAM_H_