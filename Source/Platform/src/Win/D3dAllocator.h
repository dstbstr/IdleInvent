#pragma once
#include <d3d12.h>
#include <imgui.h>
#include <wrl/client.h>

struct D3dAllocator {
    D3dAllocator(ID3D12Device* device, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap);

    void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* outCpuDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE* outGpuDescHandle);
    void Free(D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandle);

private:
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Heap{nullptr};
    D3D12_DESCRIPTOR_HEAP_TYPE HeapType{D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES};
    D3D12_CPU_DESCRIPTOR_HANDLE HeapStartCpu{};
    D3D12_GPU_DESCRIPTOR_HANDLE HeapStartGpu{};
    UINT HeapHandleIncrement{};
    ImVector<int> FreeIndicies;
};
