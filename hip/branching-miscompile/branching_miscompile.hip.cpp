// ===========================================================================
// Standalone experiment: does amdclang fold a "pointer-before-array" access
// (base = buf - K ; base[i]) into a flat load with an immediate offset, and
// does that miscompile?  (No OpenLB here — pure compiler experiment.)
//
// Idea: for each pattern we build a device pointer that is DERIVED (base = buf +
// C or buf - C, C a compile-time constant) and then index it so the FINAL
// address is in-bounds. At -O3 amdclang tends to fold the constant into the
// flat/global load's immediate `offset:` field. We compare the GPU result to a
// trivially-correct CPU reference, at -O0 / -O1 / -O2 / -O3.
//
//   mismatch at -O3 only  => amdclang optimizer (offset-folding) miscompile
//   all correct           => not triggered on this toolchain/GPU (see notes)
// ===========================================================================
#include <hip/hip_runtime.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cstdint>

#define CK(x) do{ hipError_t e=(x); if(e!=hipSuccess){                        \
  printf("HIP error %d at %d: %s\n", e, __LINE__, hipGetErrorString(e));      \
  std::exit(2);} }while(0)

static const int N = 1<<16;   // elements per buffer

// ---- Pattern A: base = buf - K (pointer BEFORE the array), read base[i] ------
// final index i-... stays in [0,N); intermediate pointer is out of the array.
template<int K>
__global__ void patA(const double* buf, double* out, const unsigned* idx, int n){
  int t = blockIdx.x*blockDim.x + threadIdx.x; if(t>=n) return;
  const double* base = buf - K;          // <-- pointer before the array (UB)
  unsigned i = idx[t];                   // idx chosen so (i in [K, N+K))
  out[t] = base[i];                      // == buf[i-K]
}

// ---- Pattern B: OpenLB two-pointer select (start1 = buf - (N-shift)) --------
template<int SHIFT>
__global__ void patB(const double* buf, double* out, const unsigned* idx, int n){
  int t = blockIdx.x*blockDim.x + threadIdx.x; if(t>=n) return;
  const double* start0 = buf + SHIFT;
  const double* start1 = buf - (N - SHIFT);   // <-- UB pointer before array
  long remainder = N - SHIFT - 1;
  unsigned i = idx[t];                          // i in [0,N)
  out[t] = ((long)i > remainder) ? start1[i] : start0[i];  // == buf[(SHIFT+i)%N]
}

// ---- Pattern C: negative coefficient  buf2[C - i]  (like ROCm #4389 f[1-i]) --
template<int C>
__global__ void patC(const double* buf, double* out, const unsigned* idx, int n){
  int t = blockIdx.x*blockDim.x + threadIdx.x; if(t>=n) return;
  long i = (long)idx[t];                 // i in [0, C]
  out[t] = buf[C - i];
}

// ---- Pattern D: coupling-shaped. Runtime shift, by-value accessor struct,
// Q populations, neighbour gather -> high register pressure + branchy flat load.
struct Acc {                      // mimics OpenLB CyclicAccessor<Branching>
  const double* start0;
  const double* start1;
  long remainder;
  __device__ double get(unsigned i) const {   // == base[(shift+i) % N]
    return ((long)i > remainder) ? start1[i] : start0[i];
  }
};
static const int Q = 19;
__global__ void patD(const double* buf, const int* shiftDev, double* out,
                     const unsigned* idx, int n){
  int t = blockIdx.x*blockDim.x + threadIdx.x; if(t>=n) return;
  int shift = shiftDev[0];                       // runtime, not compile-time
  Acc acc[Q];
  for(int q=0;q<Q;++q){                           // one accessor per population
    const double* pbase = buf + (long)q*N;        // population q's plane
    acc[q].start0    = pbase + shift;
    acc[q].start1    = pbase - (N - shift);       // <-- pointer before that plane (UB)
    acc[q].remainder = N - shift - 1;
  }
  unsigned i  = idx[t];
  unsigned in = idx[(t+1)%n];                     // "neighbour" cell
  double s = 0;
  for(int q=0;q<Q;++q) s += acc[q].get(i) + acc[q].get(in);
  out[t] = s;
}

// ---- Pattern E: FORCE flat_load. Base pointer is LOADED FROM MEMORY (a table),
// so amdclang cannot prove it is global -> emits generic `flat_load` (the #4389
// buggy path), not `global_load`. Then do the negative-base access.
__global__ void patE(const double* const* baseTable, const int* shiftDev,
                     double* out, const unsigned* idx, int n){
  int t = blockIdx.x*blockDim.x + threadIdx.x; if(t>=n) return;
  const double* buf = baseTable[0];       // loaded from memory -> generic pointer
  int shift = shiftDev[0];
  const double* start0 = buf + shift;
  const double* start1 = buf - (N - shift);   // pointer before the array (UB)
  long remainder = N - shift - 1;
  unsigned i = idx[t];
  out[t] = ((long)i > remainder) ? start1[i] : start0[i];   // == buf[(shift+i)%N]
}

static int check(const char* name, const std::vector<double>& got,
                 const std::vector<double>& ref){
  int bad=0; for(size_t t=0;t<got.size();++t) if(got[t]!=ref[t]){ if(bad<3)
    printf("   [%s] MISMATCH t=%zu got=%.1f ref=%.1f\n",name,t,got[t],ref[t]); ++bad; }
  printf("   [%s] mismatches=%d %s\n", name, bad, bad?"  <<< MISCOMPILE":"ok");
  return bad;
}

int main(){
  const int n=N;
  std::vector<double> hbuf(N); for(int j=0;j<N;++j) hbuf[j]=(double)j;
  double* dbuf; CK(hipMalloc(&dbuf,N*sizeof(double)));
  CK(hipMemcpy(dbuf,hbuf.data(),N*sizeof(double),hipMemcpyHostToDevice));
  unsigned* didx; CK(hipMalloc(&didx,n*sizeof(unsigned)));
  double* dout;   CK(hipMalloc(&dout,n*sizeof(double)));
  std::vector<double> hout(n), ref(n);
  auto run=[&](auto kern, const std::vector<unsigned>& idx){
    CK(hipMemcpy(didx,idx.data(),n*sizeof(unsigned),hipMemcpyHostToDevice));
    kern<<<(n+255)/256,256>>>(dbuf,dout,didx,n);
    CK(hipGetLastError()); CK(hipDeviceSynchronize());
    CK(hipMemcpy(hout.data(),dout,n*sizeof(double),hipMemcpyDeviceToHost));
  };
  int total=0;
  // A: base=buf-K, idx in [K,N) so buf[i-K] valid
  { const int K=1000; std::vector<unsigned> idx(n);
    for(int t=0;t<n;++t) idx[t]=K + (t % (N-K));
    for(int t=0;t<n;++t) ref[t]=hbuf[idx[t]-K];
    run(patA<K>, idx); total+=check("A buf-1000",hout,ref); }
  // B: OpenLB pattern, idx in [0,N)
  { const int S=1234; std::vector<unsigned> idx(n);
    for(int t=0;t<n;++t) idx[t]=t % N;
    for(int t=0;t<n;++t) ref[t]=hbuf[(S+idx[t])%N];
    run(patB<S>, idx); total+=check("B two-ptr",hout,ref); }
  // C: buf[C - i], idx=i in [0,C]
  { const int C=50000; std::vector<unsigned> idx(n);
    for(int t=0;t<n;++t) idx[t]=t % (C+1);
    for(int t=0;t<n;++t) ref[t]=hbuf[C-(long)idx[t]];
    run(patC<C>, idx); total+=check("C C-i",hout,ref); }

  // D: coupling-shaped, Q planes, runtime shift, neighbour gather
  { const int S=1234; std::vector<unsigned> idx(n);
    for(int t=0;t<n;++t) idx[t]=t % N;
    // need Q planes in dbuf: reallocate a bigger buffer
    double* dbufQ; CK(hipMalloc(&dbufQ,(size_t)Q*N*sizeof(double)));
    std::vector<double> hbufQ((size_t)Q*N);
    for(int q=0;q<Q;++q) for(int j=0;j<N;++j) hbufQ[(size_t)q*N+j]=(double)(q*1000000+j);
    CK(hipMemcpy(dbufQ,hbufQ.data(),(size_t)Q*N*sizeof(double),hipMemcpyHostToDevice));
    int* dshift; CK(hipMalloc(&dshift,sizeof(int))); CK(hipMemcpy(dshift,&S,sizeof(int),hipMemcpyHostToDevice));
    CK(hipMemcpy(didx,idx.data(),n*sizeof(unsigned),hipMemcpyHostToDevice));
    patD<<<(n+255)/256,256>>>(dbufQ,dshift,dout,didx,n);
    CK(hipGetLastError()); CK(hipDeviceSynchronize());
    CK(hipMemcpy(hout.data(),dout,n*sizeof(double),hipMemcpyDeviceToHost));
    for(int t=0;t<n;++t){ unsigned i=idx[t], in=idx[(t+1)%n]; double s=0;
      for(int q=0;q<Q;++q) s += hbufQ[(size_t)q*N+(S+i)%N] + hbufQ[(size_t)q*N+(S+in)%N];
      ref[t]=s; }
    total+=check("D coupling",hout,ref);
    CK(hipFree(dbufQ)); CK(hipFree(dshift)); }

  // E: force flat_load via a pointer table
  { const int S=1234; std::vector<unsigned> idx(n);
    for(int t=0;t<n;++t) idx[t]=t % N;
    for(int t=0;t<n;++t) ref[t]=hbuf[(S+idx[t])%N];
    const double* htab[1] = { dbuf };
    const double** dtab; CK(hipMalloc(&dtab,sizeof(htab)));
    CK(hipMemcpy(dtab,htab,sizeof(htab),hipMemcpyHostToDevice));
    int* dshift; CK(hipMalloc(&dshift,sizeof(int))); CK(hipMemcpy(dshift,&S,sizeof(int),hipMemcpyHostToDevice));
    CK(hipMemcpy(didx,idx.data(),n*sizeof(unsigned),hipMemcpyHostToDevice));
    patE<<<(n+255)/256,256>>>(dtab,dshift,dout,didx,n);
    CK(hipGetLastError()); CK(hipDeviceSynchronize());
    CK(hipMemcpy(hout.data(),dout,n*sizeof(double),hipMemcpyDeviceToHost));
    total+=check("E flat-load",hout,ref);
    CK(hipFree(dtab)); CK(hipFree(dshift)); }

  printf("=> TOTAL mismatches=%d : %s\n", total,
         total? "*** amdclang MISCOMPILE reproduced ***" : "no miscompile on this build");
  return total?1:0;
}
