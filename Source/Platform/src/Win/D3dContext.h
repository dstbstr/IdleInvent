#pragma once

#include "D3dAllocator.h"
#include "Platform/Platform.h"
#include "DX12Image.h"

#include <wrl/client.h>
#include <atlbase.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <array>
#include <unordered_map>

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

struct FrameContext {
    ComPtr<ID3D12CommandAllocator> CommandAllocator{nullptr};
    UINT64 FenceVal{0};
};

struct D3dContext {
    D3dContext(Platform& platform);
    ~D3dContext();
    FrameContext* WaitForNextFrameResources();
    std::unique_ptr<DX12Image> TryLoadTextureFromMemory(const void* data, size_t dataSize);
    void Resize(UINT width, UINT height);
    bool NextFrame();
    void RenderFrame(const float* clearColorWithAlpha);

    static constexpr UINT FramesInFlightCount = 2;

    std::unordered_map<std::string, std::unique_ptr<DX12Image>> Images{};

private:
    static constexpr UINT BackBufferCount = 2;
    static constexpr UINT AppHeapSize = 64;

    ComPtr<ID3D12Device> Device{nullptr};
    ComPtr<ID3D12DescriptorHeap> RtvHeap{nullptr};
    ComPtr<ID3D12DescriptorHeap> SrvHeap{nullptr};
    ComPtr<ID3D12CommandQueue> CommandQueue{nullptr};
    ComPtr<ID3D12GraphicsCommandList> CommandList{nullptr};
    ComPtr<ID3D12Fence> Fence{nullptr};
    CHandle FenceEvent{nullptr};
    ComPtr<IDXGISwapChain3> SwapChain{nullptr};
    CHandle SwapChainWaitableObject{nullptr};
    std::array<ComPtr<ID3D12Resource>, BackBufferCount> Resources{};
    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, BackBufferCount> Descriptors{};
    std::array<FrameContext, FramesInFlightCount> FrameContext{};
    
    UINT FrameIndex{};
    UINT64 FenceLastSignaledValue{0};
    bool SwapChainOccluded{false};
    std::unique_ptr<D3dAllocator> Alloc{nullptr};

    void CreateRenderTarget();
    void CleanupRenderTarget();
    void WaitForLastSubmittedFrame();
};

void CheckForLeaks();