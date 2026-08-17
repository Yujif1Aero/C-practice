#include <hip/hip_runtime.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

#define HIP_CHECK(call)                                                        \
do {                                                                           \
    hipError_t _err = (call);                                                  \
    if (_err != hipSuccess) {                                                  \
        std::fprintf(stderr,                                                   \
                     "HIP error: %s (%d) at %s:%d\n",                          \
                     hipGetErrorString(_err),                                  \
                     static_cast<int>(_err),                                   \
                     __FILE__,                                                 \
                     __LINE__);                                                \
        std::exit(EXIT_FAILURE);                                               \
    }                                                                          \
} while (0)

static std::size_t round_up(std::size_t value, std::size_t granularity)
{
    return ((value + granularity - 1) / granularity) * granularity;
}

static std::size_t align_down_u32(std::size_t value)
{
    return value & ~(sizeof(std::uint32_t) - 1);
}

__global__ void write_first_kernel(std::uint32_t* first, std::uint32_t value)
{
    if (blockIdx.x == 0 && threadIdx.x == 0) {
        first[0] = value;
    }
}

__global__ void read_second_kernel(const std::uint32_t* second,
                                   std::uint32_t* output)
{
    if (blockIdx.x == 0 && threadIdx.x == 0) {
        output[0] = second[0];
    }
}

__global__ void same_kernel_alias_test(std::uint32_t* first,
                                       const std::uint32_t* second,
                                       std::uint32_t* output,
                                       std::uint32_t probe)
{
    if (blockIdx.x == 0 && threadIdx.x == 0) {
        first[0] = probe;
        output[0] = second[0];
    }
}

__global__ void same_kernel_alias_test_volatile(volatile std::uint32_t* first,
                                                volatile std::uint32_t* second,
                                                std::uint32_t* output,
                                                std::uint32_t probe)
{
    if (blockIdx.x == 0 && threadIdx.x == 0) {
        first[0] = probe;
        output[0] = second[0];
    }
}

static bool host_alias_probe(std::uint8_t* writeBase,
                             std::uint8_t* readBase,
                             const std::vector<std::size_t>& offsets,
                             std::uint32_t seed,
                             const char* label)
{
    bool allOk = true;

    std::printf("\n[%s]\n", label);

    for (std::size_t i = 0; i < offsets.size(); ++i) {
        const std::size_t off = offsets[i];
        const std::uint32_t probe =
            seed ^ static_cast<std::uint32_t>(0x01010101u * (i + 1));

        std::uint32_t back = 0;

        HIP_CHECK(hipMemcpy(writeBase + off,
                            &probe,
                            sizeof(probe),
                            hipMemcpyHostToDevice));

        HIP_CHECK(hipMemcpy(&back,
                            readBase + off,
                            sizeof(back),
                            hipMemcpyDeviceToHost));

        const bool ok = (back == probe);
        allOk = allOk && ok;

        std::printf("  offset=%12zu  wrote=0x%08x  read=0x%08x  %s\n",
                    off,
                    static_cast<unsigned>(probe),
                    static_cast<unsigned>(back),
                    ok ? "OK" : "BROKEN");
    }

    return allOk;
}

int main(int argc, char** argv)
{
    int device = 0;
    HIP_CHECK(hipGetDevice(&device));

    hipDeviceProp_t devProp{};
    HIP_CHECK(hipGetDeviceProperties(&devProp, device));

    std::printf("=== HIP VMM alias check ===\n");
    std::printf("device           : %d\n", device);
    std::printf("name             : %s\n", devProp.name);

    int vmmSupported = 0;
    HIP_CHECK(hipDeviceGetAttribute(
        &vmmSupported,
        hipDeviceAttributeVirtualMemoryManagementSupported,
        device));

    std::printf("VMM supported    : %s\n",
                vmmSupported ? "YES" : "NO");

    if (!vmmSupported) {
        std::fprintf(stderr,
                     "This device/runtime reports that HIP VMM is unsupported.\n");
        return EXIT_FAILURE;
    }

    // Default logical test size: 1 MiB.
    // You may pass a different byte count:
    //   ./vmm_alias_check 100000
    std::size_t logicalBytes = 1u << 20;

    if (argc >= 2) {
        const unsigned long long parsed =
            std::strtoull(argv[1], nullptr, 10);

        if (parsed < sizeof(std::uint32_t)) {
            std::fprintf(stderr,
                         "Requested size must be at least %zu bytes.\n",
                         sizeof(std::uint32_t));
            return EXIT_FAILURE;
        }

        logicalBytes = static_cast<std::size_t>(parsed);
    }

    hipMemAllocationProp prop{};
    prop.type = hipMemAllocationTypePinned;
    prop.location.type = hipMemLocationTypeDevice;
    prop.location.id = device;

    std::size_t granularity = 0;
    HIP_CHECK(hipMemGetAllocationGranularity(
        &granularity,
        &prop,
        hipMemAllocationGranularityMinimum));

    const std::size_t mappedBytes =
        round_up(logicalBytes, granularity);

    const std::size_t reserveBytes =
        2 * mappedBytes;

    std::printf("logicalBytes     : %zu\n", logicalBytes);
    std::printf("granularity      : %zu\n", granularity);
    std::printf("mappedBytes      : %zu\n", mappedBytes);
    std::printf("reserveBytes     : %zu\n", reserveBytes);

    hipMemGenericAllocationHandle_t handle{};

    // One backing allocation only.
    HIP_CHECK(hipMemCreate(
        &handle,
        mappedBytes,
        &prop,
        0));

    void* base = nullptr;

    // Reserve two consecutive virtual ranges.
    HIP_CHECK(hipMemAddressReserve(
        &base,
        reserveBytes,
        granularity,
        nullptr,
        0));

    auto* first =
        static_cast<std::uint8_t*>(base);

    auto* second =
        first + mappedBytes;

    std::printf("first VA         : %p\n",
                static_cast<void*>(first));
    std::printf("second VA        : %p\n",
                static_cast<void*>(second));
    std::printf("VA distance      : %zu bytes\n",
                static_cast<std::size_t>(second - first));

    // Map the SAME physical allocation handle twice.
    HIP_CHECK(hipMemMap(
        first,
        mappedBytes,
        0,
        handle,
        0));

    HIP_CHECK(hipMemMap(
        second,
        mappedBytes,
        0,
        handle,
        0));

    hipMemAccessDesc accessDesc{};
    accessDesc.location.type = hipMemLocationTypeDevice;
    accessDesc.location.id = device;
    accessDesc.flags = hipMemAccessFlagsProtReadWrite;

    // Set access independently for both mappings.
    HIP_CHECK(hipMemSetAccess(
        first,
        mappedBytes,
        &accessDesc,
        1));

    HIP_CHECK(hipMemSetAccess(
        second,
        mappedBytes,
        &accessDesc,
        1));

    // Build several probe offsets.
    std::vector<std::size_t> offsets;

    auto add_offset = [&](std::size_t off) {
        if (mappedBytes < sizeof(std::uint32_t)) {
            return;
        }

        off = std::min(off, mappedBytes - sizeof(std::uint32_t));
        off = align_down_u32(off);

        if (std::find(offsets.begin(), offsets.end(), off) == offsets.end()) {
            offsets.push_back(off);
        }
    };

    add_offset(0);
    add_offset(4096);
    if (granularity >= sizeof(std::uint32_t)) {
        add_offset(granularity - sizeof(std::uint32_t));
    }
    add_offset(mappedBytes / 2);
    add_offset(mappedBytes - sizeof(std::uint32_t));

    // ------------------------------------------------------------
    // Test 1: host memcpy, first -> second
    // ------------------------------------------------------------
    const bool hostForwardOk =
        host_alias_probe(
            first,
            second,
            offsets,
            0xA5A50000u,
            "host memcpy: first -> second");

    // ------------------------------------------------------------
    // Test 2: host memcpy, second -> first
    // ------------------------------------------------------------
    const bool hostReverseOk =
        host_alias_probe(
            second,
            first,
            offsets,
            0x5A5A0000u,
            "host memcpy: second -> first");

    // Device-side result buffer.
    std::uint32_t* dResult = nullptr;
    HIP_CHECK(hipMalloc(
        reinterpret_cast<void**>(&dResult),
        sizeof(std::uint32_t)));

    // ------------------------------------------------------------
    // Test 3: separate kernels
    // ------------------------------------------------------------
    constexpr std::uint32_t separateProbe = 0x13579BDFu;

    write_first_kernel<<<1, 1>>>(
        reinterpret_cast<std::uint32_t*>(first),
        separateProbe);
    HIP_CHECK(hipGetLastError());

    read_second_kernel<<<1, 1>>>(
        reinterpret_cast<const std::uint32_t*>(second),
        dResult);
    HIP_CHECK(hipGetLastError());

    HIP_CHECK(hipDeviceSynchronize());

    std::uint32_t separateBack = 0;
    HIP_CHECK(hipMemcpy(
        &separateBack,
        dResult,
        sizeof(separateBack),
        hipMemcpyDeviceToHost));

    const bool separateOk =
        (separateBack == separateProbe);

    std::printf("\n[device: separate kernels]\n");
    std::printf("  wrote=0x%08x  read=0x%08x  %s\n",
                static_cast<unsigned>(separateProbe),
                static_cast<unsigned>(separateBack),
                separateOk ? "OK" : "BROKEN");

    // ------------------------------------------------------------
    // Test 4: same kernel, normal pointers
    // ------------------------------------------------------------
    constexpr std::uint32_t sameProbe = 0x2468ACE0u;

    HIP_CHECK(hipMemset(
        dResult,
        0,
        sizeof(std::uint32_t)));

    same_kernel_alias_test<<<1, 1>>>(
        reinterpret_cast<std::uint32_t*>(first),
        reinterpret_cast<const std::uint32_t*>(second),
        dResult,
        sameProbe);
    HIP_CHECK(hipGetLastError());
    HIP_CHECK(hipDeviceSynchronize());

    std::uint32_t sameBack = 0;
    HIP_CHECK(hipMemcpy(
        &sameBack,
        dResult,
        sizeof(sameBack),
        hipMemcpyDeviceToHost));

    const bool sameOk =
        (sameBack == sameProbe);

    std::printf("\n[device: same kernel, normal pointers]\n");
    std::printf("  wrote=0x%08x  read=0x%08x  %s\n",
                static_cast<unsigned>(sameProbe),
                static_cast<unsigned>(sameBack),
                sameOk ? "OK" : "BROKEN");

    // ------------------------------------------------------------
    // Test 5: same kernel, volatile pointers
    // ------------------------------------------------------------
    constexpr std::uint32_t volatileProbe = 0xCAFEBABEu;

    HIP_CHECK(hipMemset(
        dResult,
        0,
        sizeof(std::uint32_t)));

    same_kernel_alias_test_volatile<<<1, 1>>>(
        reinterpret_cast<volatile std::uint32_t*>(first),
        reinterpret_cast<volatile std::uint32_t*>(second),
        dResult,
        volatileProbe);
    HIP_CHECK(hipGetLastError());
    HIP_CHECK(hipDeviceSynchronize());

    std::uint32_t volatileBack = 0;
    HIP_CHECK(hipMemcpy(
        &volatileBack,
        dResult,
        sizeof(volatileBack),
        hipMemcpyDeviceToHost));

    const bool volatileOk =
        (volatileBack == volatileProbe);

    std::printf("\n[device: same kernel, volatile pointers]\n");
    std::printf("  wrote=0x%08x  read=0x%08x  %s\n",
                static_cast<unsigned>(volatileProbe),
                static_cast<unsigned>(volatileBack),
                volatileOk ? "OK" : "BROKEN");

    // ------------------------------------------------------------
    // Summary
    // ------------------------------------------------------------
    std::printf("\n=== Summary ===\n");
    std::printf("host first -> second     : %s\n",
                hostForwardOk ? "PASS" : "FAIL");
    std::printf("host second -> first     : %s\n",
                hostReverseOk ? "PASS" : "FAIL");
    std::printf("separate kernels         : %s\n",
                separateOk ? "PASS" : "FAIL");
    std::printf("same kernel normal       : %s\n",
                sameOk ? "PASS" : "FAIL");
    std::printf("same kernel volatile     : %s\n",
                volatileOk ? "PASS" : "FAIL");

    std::printf("\nInterpretation:\n");
    std::printf("  - Host tests PASS: the two VAs observe the same backing allocation\n");
    std::printf("    across hipMemcpy operations.\n");
    std::printf("  - Separate kernels PASS: device-side aliasing works across kernel boundaries.\n");
    std::printf("  - Normal FAIL but volatile PASS: suspect virtual-alias cache/coherency behavior.\n");
    std::printf("  - Everything FAIL: suspect mapping size/granularity/access/VMM support first.\n");

    HIP_CHECK(hipFree(dResult));

    // Cleanup order:
    // unmap each virtual mapping -> release physical handle -> free VA reservation
    HIP_CHECK(hipMemUnmap(first, mappedBytes));
    HIP_CHECK(hipMemUnmap(second, mappedBytes));
    HIP_CHECK(hipMemRelease(handle));
    HIP_CHECK(hipMemAddressFree(base, reserveBytes));

    return (hostForwardOk &&
            hostReverseOk &&
            separateOk &&
            volatileOk)
           ? EXIT_SUCCESS
           : EXIT_FAILURE;
}
