#ifndef HALFINPUTSTREAM_H_
#define HALFINPUTSTREAM_H_

#include "InputStream.h"

class HalfInputStream : public InputStream {
protected:
    virtual void SetBase(); 
};

#endif //#ifndef HALFINPUTSTREAM_H_