#ifdef WIN32
#include "D3dAllocator.h"

using namespace Microsoft::WRL;

D3dAllocator::D3dAllocator(
    ID3D12Device* device, ComPtr<ID3D12DescriptorHeap> heap
) {
    IM_ASSERT(Heap == nullptr && FreeIndicies.empty());
    Heap = heap;
    auto desc = heap->GetDesc();
    HeapType = desc.Type;
    HeapStartCpu = Heap->GetCPUDescriptorHandleForHeapStart();
    HeapStartGpu = Heap->GetGPUDescriptorHandleForHeapStart();
    HeapHandleIncrement = device->GetDescriptorHandleIncrementSize(HeapType);
    FreeIndicies.reserve(static_cast<int>(desc.NumDescriptors));
    for(int n = desc.NumDescriptors; n > 0; n--) {
        FreeIndicies.push_back(n - 1);
    }
}

void D3dAllocator::Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* outCpuDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE* outGpuDescHandle) {
    IM_ASSERT(FreeIndicies.Size > 0);
    auto idx = FreeIndicies.back();
    FreeIndicies.pop_back();
    outCpuDescHandle->ptr = HeapStartCpu.ptr + (idx * HeapHandleIncrement);
    outGpuDescHandle->ptr = HeapStartGpu.ptr + (idx * HeapHandleIncrement);
}

void D3dAllocator::Free(D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandle) {
    auto cpuIdx = (int)((cpuDescHandle.ptr - HeapStartCpu.ptr) / HeapHandleIncrement);
    [[maybe_unused]] auto gpuIdx = (int)((gpuDescHandle.ptr - HeapStartGpu.ptr) / HeapHandleIncrement);
    IM_ASSERT(cpuIdx == gpuIdx);
    FreeIndicies.push_back(cpuIdx);
}
#endif