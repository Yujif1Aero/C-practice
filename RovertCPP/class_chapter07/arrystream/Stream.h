#ifndef STREAM_H_
#define STREAM_H_


class Stream {
  public:
    Stream();
    Stream(double n);
    double Get() const;
    bool Set();

  protected:
    double m_n;
   
  protected:
    virtual void SetBase() = 0;
  
  public:
   virtual ~Stream(){};
};

#endif // ifndef STREAM_H_