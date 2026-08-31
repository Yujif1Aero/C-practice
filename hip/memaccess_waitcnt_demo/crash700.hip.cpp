// Deliberately produce HIP error 700 (illegal memory access) by dereferencing
// a bogus pointer. This is the SAME endpoint the -g miscompile reaches by
// accident: computing an address from a not-yet-arrived (garbage) value.
#include <hip/hip_runtime.h>
#include <cstdio>
__global__ void chase(long** table, long* out){
  long* p = table[0];   // (1) read an address (we planted a bogus one)
  *out = *p;            // (2) read that non-existent address -> crash
}
int main(){
  long** table; long* out;
  hipMalloc(&table, sizeof(long*));
  hipMalloc(&out,   sizeof(long));
  long* garbage = (long*)0xdeadbeef00;                 // clearly invalid address
  hipMemcpy(table, &garbage, sizeof(long*), hipMemcpyHostToDevice);
  chase<<<1,1>>>(table, out);
  hipError_t e = hipDeviceSynchronize();
  printf("result: HIP error %d = %s\n", (int)e, hipGetErrorString(e));
  return (int)e;
}
