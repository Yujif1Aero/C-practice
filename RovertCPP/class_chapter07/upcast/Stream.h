#ifndef STREAM_H_
#define STREAM_H_

class Stream {
  public:
    double Get() const;
    //virtual bool Set();
    bool Set();

  protected:
    double m_n;
   
  protected:
    virtual void SetBase();
};

#endif // ifndef STREAM_H_