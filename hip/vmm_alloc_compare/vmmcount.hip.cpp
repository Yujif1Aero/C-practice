// Create many small VMM columns on DEVICE 0 ONLY and measure real VRAM cost.
// On failure, print exactly which call failed, at which count, and the
// free/total VRAM at that moment (to prove "OOM while VRAM is free").
#include <hip/hip_runtime.h>
#include <cstdio>
static void vram(const char* tag){
  size_t f=0,t=0; hipMemGetInfo(&f,&t);
  printf("%s free/total = %.2f / %.2f GiB\n", tag, f/1073741824.0, t/1073741824.0);
}
int main(){
  int n=0; hipGetDeviceCount(&n);
  hipDeviceProp_t dp{}; hipGetDeviceProperties(&dp,0);
  printf("visible devices=%d ; using device 0 = %s (PCI %02x:%02x.0)\n",
         n, dp.name, dp.pciBusID, dp.pciDeviceID);
  hipSetDevice(0);
  hipMemAllocationProp prop{};
  prop.type=hipMemAllocationTypePinned;
  prop.location.type=hipMemLocationTypeDevice;
  prop.location.id=0;                       // allocate ON DEVICE 0
  hipMemAccessDesc acc{}; acc.location=prop.location;
  acc.flags=hipMemAccessFlagsProtReadWrite;

  const size_t sz = 64*1024;                // request 64 KiB per column
  const int    N  = 10000;
  size_t f0=0,t=0; hipMemGetInfo(&f0,&t);
  printf("start: free = %.2f GiB (total %.2f GiB)\n", f0/1073741824.0, t/1073741824.0);
  int i=0;
  for(; i<N; ++i){
    hipDeviceptr_t p; hipMemGenericAllocationHandle_t h; hipError_t e;
    if((e=hipMemAddressReserve(&p,2*sz,0,0,0))){
      printf("FAIL hipMemAddressReserve at #%d: %s\n",i,hipGetErrorString(e)); vram("  at-fail"); break; }
    if((e=hipMemCreate(&h,sz,&prop,0))){
      printf("FAIL hipMemCreate at #%d: %s\n",i,hipGetErrorString(e)); vram("  at-fail"); break; }
    if((e=hipMemMap(p,sz,0,h,0))){
      printf("FAIL hipMemMap(1st) at #%d: %s\n",i,hipGetErrorString(e)); vram("  at-fail"); break; }
    if((e=hipMemMap((hipDeviceptr_t)((uint8_t*)p+sz),sz,0,h,0))){
      printf("FAIL hipMemMap(2nd) at #%d: %s\n",i,hipGetErrorString(e)); vram("  at-fail"); break; }
    if((e=hipMemSetAccess(p,2*sz,&acc,1))){
      printf("FAIL hipMemSetAccess at #%d: %s\n",i,hipGetErrorString(e)); vram("  at-fail"); break; }
    if(i%2000==0){ size_t f; hipMemGetInfo(&f,&t);
      printf("  #%-6d free = %.2f GiB\n", i, f/1073741824.0); }
  }
  size_t f1=0; hipMemGetInfo(&f1,&t);
  double usedMiB=(f0-f1)/1048576.0;
  printf("made %d columns (request 64 KiB each)\n", i);
  printf("total real usage = %.1f MiB  ->  %.3f MiB per column\n", usedMiB, i? usedMiB/i:0.0);
  return (i==N)?0:1;
}
