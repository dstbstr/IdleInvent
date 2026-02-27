#ifdef WIN32
#include "D3dAllocator.h"
#include <utility>

using namespace Microsoft::WRL;

D3dAllocator::D3dAllocator(ID3D12Device* device, ComPtr<ID3D12DescriptorHeap> heap) 
:Heap(std::move(heap)) {
    IM_ASSERT(Heap == nullptr && FreeIndicies.empty());
    auto desc = Heap->GetDesc();
    HeapType = desc.Type;
    HeapStartCpu = Heap->GetCPUDescriptorHandleForHeapStart();
    HeapStartGpu = Heap->GetGPUDescriptorHandleForHeapStart();
    HeapHandleIncrement = device->GetDescriptorHandleIncrementSize(HeapType);
    FreeIndicies.reserve(static_cast<int>(desc.NumDescriptors));
    for(auto n = desc.NumDescriptors; n > 0; n--) {
        FreeIndicies.push_back(static_cast<int>(n));
    }
}

void D3dAllocator::Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* outCpuDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE* outGpuDescHandle) {
    IM_ASSERT(FreeIndicies.Size > 0);
    auto idx = static_cast<UINT>(FreeIndicies.back());
    FreeIndicies.pop_back();
    outCpuDescHandle->ptr = HeapStartCpu.ptr + static_cast<SIZE_T>(idx * HeapHandleIncrement);
    outGpuDescHandle->ptr = HeapStartGpu.ptr + static_cast<SIZE_T>(idx * HeapHandleIncrement);
}

void D3dAllocator::Free(D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandle) {
    auto cpuIdx = (int)((cpuDescHandle.ptr - HeapStartCpu.ptr) / HeapHandleIncrement);
    [[maybe_unused]] auto gpuIdx = (int)((gpuDescHandle.ptr - HeapStartGpu.ptr) / HeapHandleIncrement);
    IM_ASSERT(cpuIdx == gpuIdx);
    FreeIndicies.push_back(cpuIdx);
}
#endif