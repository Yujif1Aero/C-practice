// ===========================================================================
// Two modes, to SHOW what actually decides fault-vs-correct:
//
//  (default, no args)  SAFE mode  -> kern():  base[i + c]   (i has a POSITIVE
//     coefficient). The compiler folds so the load's base register = start1 + i,
//     which lands back INSIDE the allocation. flat_load ... offset: is emitted,
//     but it does NOT fault. This is the "instruction-only" reproduction.
//
//  ./ff <i> [useShared]  FAULT mode -> kern_neg(): base[1 - i]  (i has a NEGATIVE
//     coefficient, the #4389 shape). Now the folded base register = base - i, so
//     for i>0 the BASE ITSELF is before the allocation -> the hardware decides the
//     address space from that out-of-bounds base -> illegal access (HIP 700).
//
//  => An argument alone cannot make the SAFE kernel fault: what matters is that
//     the folded base register is out of bounds, which needs the negative-index
//     ACCESS PATTERN. The argument then just picks how far out (which i faults).
//
// Build: hipcc -O3 -std=c++17 --offload-arch=gfx1100 flat_force.hip.cpp -o ff
// ===========================================================================
#include <hip/hip_runtime.h>
#include <cstdio>
#include <cstdlib>
#include <vector>

#define CK(x) do{ hipError_t e=(x); if(e!=hipSuccess){                     \
  printf("HIP error %d at %d: %s\n",e,__LINE__,hipGetErrorString(e));      \
  std::exit(2);} }while(0)

static const long N = 1L<<22;   // 4M doubles per plane => 32 MB buffer
static const int  Q = 19;       // small constant fan -> compile-time offsets to fold

extern __shared__ double s_lds[];

// SAFE: base[i + c], i positive => folded base = start1+i stays in-bounds => correct.
__global__ void kern(const double* g, double* out, const unsigned* idx,
                     long n, int shift, int useShared){
  long t = (long)blockIdx.x*blockDim.x + threadIdx.x; if(t>=n) return;
  const double* buf = useShared ? s_lds : g;
  const double* start0 = buf + shift;
  const double* start1 = buf - (N - shift);
  long rem = N - shift - 1;
  long i = idx[t];
  const double* base = (i > rem) ? start1 : start0;
  double s = 0;
#pragma unroll
  for(int c=0;c<Q;++c) s += base[i + c];         // POSITIVE i -> base stays in-bounds
  out[t] = s;
}

// FAULT: base[1 - i], i negative-coefficient (#4389) => folded base = buf - i is
// BEFORE the allocation for i>0 => HW misclassifies the address space => HIP 700.
__global__ void kern_neg(const double* g, double* out, long n, int i, int useShared){
  long t = (long)blockIdx.x*blockDim.x + threadIdx.x; if(t>=n) return;
  const double* buf = useShared ? s_lds : g;     // flat (can't prove global)
  out[t] = buf[1 - i];                            // base = buf - i, offset:+1 folded
}

int main(int argc, char** argv){
  const long n = 1L<<20;               // 1M threads
  std::vector<double> hg(N);
  for(long j=0;j<N;++j) hg[j]=(double)(j%1000003);
  double* g;  CK(hipMalloc(&g,N*sizeof(double)));
  CK(hipMemcpy(g,hg.data(),N*sizeof(double),hipMemcpyHostToDevice));
  double* out; CK(hipMalloc(&out,n*sizeof(double)));

  // ---- FAULT mode: ./ff <i> [useShared] --------------------------------------
  if(argc>1){
    int i = atoi(argv[1]);
    int useShared = (argc>2)? atoi(argv[2]) : 0;
    printf("FAULT mode: base[1 - i] with i=%d useShared=%d\n", i, useShared);
    kern_neg<<<(n+255)/256,256, 8>>>(g,out,n,i,useShared);
    hipError_t le=hipGetLastError(), se=hipDeviceSynchronize();
    printf("launch=%s  sync=%s%s\n", hipGetErrorString(le), hipGetErrorString(se),
           se!=hipSuccess? "   <<< FAULT (HIP 700: base went out of the allocation)":"  (no fault at this i)");
    return se!=hipSuccess;
  }

  // ---- SAFE mode (default): instruction-only, correct -------------------------
  const int shift = 1234;
  unsigned* didx; CK(hipMalloc(&didx,n*sizeof(unsigned)));
  std::vector<unsigned> idx(n);
  for(long t=0;t<n;++t) idx[t]=(unsigned)((N-shift) + (t % shift));  // force start1
  CK(hipMemcpy(didx,idx.data(),n*sizeof(unsigned),hipMemcpyHostToDevice));
  kern<<<(n+255)/256,256, 8>>>(g,out,didx,n,shift,/*useShared=*/0);
  hipError_t le=hipGetLastError(), se=hipDeviceSynchronize();
  printf("SAFE mode: launch=%s sync=%s\n", hipGetErrorString(le), hipGetErrorString(se));
  if(se!=hipSuccess){ printf(">>> unexpected fault\n"); return 1; }
  std::vector<double> hout(n); CK(hipMemcpy(hout.data(),out,n*sizeof(double),hipMemcpyDeviceToHost));
  long bad=0; for(long t=0;t<n;++t){
    long base_i=(long)idx[t]-(N-shift); double ref=0;
    for(int c=0;c<Q;++c) ref += hg[base_i+c];
    if(hout[t]!=ref){ if(bad<3) printf("  MISMATCH t=%ld\n",t); ++bad; } }
  printf("mismatches=%ld : %s  (instruction-only; pass an arg like './ff 2' to fault)\n",
         bad, bad? ">>> MISCOMPILE":"correct");
  return 0;
}
