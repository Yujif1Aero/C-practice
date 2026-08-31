// Report visible GPUs, then VMM granularity (min/rec) and VRAM of device 0.
// run.sh sets HIP_VISIBLE_DEVICES=0 so ONLY device 0 is visible to HIP.
#include <hip/hip_runtime.h>
#include <cstdio>
int main(){
  int n=0; hipGetDeviceCount(&n);
  printf("visible HIP devices : %d\n", n);
  for(int d=0; d<n; ++d){
    hipDeviceProp_t p{}; hipGetDeviceProperties(&p,d);
    printf("  device %d : %s | totalMem %.1f GiB | PCI %02x:%02x.0\n",
           d, p.name, p.totalGlobalMem/1073741824.0, p.pciBusID, p.pciDeviceID);
  }
  hipSetDevice(0);
  hipMemAllocationProp prop{};
  prop.type = hipMemAllocationTypePinned;
  prop.location.type = hipMemLocationTypeDevice;
  prop.location.id = 0;
  size_t gmin=0, grec=0;
  hipMemGetAllocationGranularity(&gmin, &prop, hipMemAllocationGranularityMinimum);
  hipMemGetAllocationGranularity(&grec, &prop, hipMemAllocationGranularityRecommended);
  size_t freeB=0, totB=0; hipMemGetInfo(&freeB,&totB);
  int hipVer=0; hipRuntimeGetVersion(&hipVer);
  printf("HIP runtime version : %d\n", hipVer);
  printf("granularity  min = %zu bytes (%.3f MiB)\n", gmin, gmin/1048576.0);
  printf("granularity  rec = %zu bytes (%.3f MiB)\n", grec, grec/1048576.0);
  printf("VRAM free/total  = %.1f / %.1f GiB\n", freeB/1073741824.0, totB/1073741824.0);
  return 0;
}
