#ifdef WIN32
#include "D3dContext.h"
#include "backends/imgui_impl_dx12.h"

#define _CRT_SECURE_NO_WARNINGS // NOLINT
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifdef DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

constexpr void EnsureOk([[maybe_unused]] HRESULT hr) {
    #ifdef DEBUG
    IM_ASSERT(SUCCEEDED(hr));
    #endif
}
/*
#ifdef DEBUG
#define EnsureOk(hr) IM_ASSERT(SUCCEEDED(hr))
#else
#define EnsureOk(hr) (void)hr
#endif
*/

void SetDebugObjectName(ID3D12DeviceChild* resource, std::string_view name) {
    resource->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.size()), name.data());
}

D3dContext::D3dContext(Platform& platform) {
    auto sd = DXGI_SWAP_CHAIN_DESC1{
        .Width = 0,
        .Height = 0,
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .Stereo = false,
        .SampleDesc = {.Count = 1, .Quality = 0},
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = BackBufferCount,
        .Scaling = DXGI_SCALING_STRETCH,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
        .Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT
    };
#ifdef DX12_ENABLE_DEBUG_LAYER
    ComPtr<ID3D12Debug> debugLayer;
    if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer)))) {
        debugLayer->EnableDebugLayer();
    }
#endif
    auto featureLevel = D3D_FEATURE_LEVEL_11_0;
    auto hr = D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(&Device));
    EnsureOk(hr);
#ifdef DX12_ENABLE_DEBUG_LAYER
    if(debugLayer) {
        ComPtr<ID3D12InfoQueue> infoQueue;
        Device->QueryInterface(IID_PPV_ARGS(&infoQueue));
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
    }
#endif

    {
        auto desc = D3D12_DESCRIPTOR_HEAP_DESC{
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            .NumDescriptors = BackBufferCount,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
            .NodeMask = 0
        };
        hr = Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&RtvHeap));
        EnsureOk(hr);

        auto rtvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        auto rtvHandle = RtvHeap->GetCPUDescriptorHandleForHeapStart();
        for(auto i = 0u; i < BackBufferCount; i++) {
            Descriptors.at(i) = rtvHandle;
            rtvHandle.ptr += rtvDescriptorSize;
        }
    }

    {
        auto desc = D3D12_DESCRIPTOR_HEAP_DESC{
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            .NumDescriptors = AppHeapSize,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            .NodeMask = 0
        };
        hr = Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&SrvHeap));
        EnsureOk(hr);
        Alloc = std::make_unique<D3dAllocator>(Device.Get(), SrvHeap);
    }

    {
        auto desc = D3D12_COMMAND_QUEUE_DESC{
            .Type = D3D12_COMMAND_LIST_TYPE_DIRECT, 
            .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
            .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE, 
            .NodeMask = 1
        };
        hr = Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&CommandQueue));
        EnsureOk(hr);
    }

    for(auto i = 0u; i < FramesInFlightCount; i++) {
        hr = Device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT, 
            IID_PPV_ARGS(&FrameContext.at(i).CommandAllocator)
        );
        EnsureOk(hr);
    }

    hr = Device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        FrameContext[0].CommandAllocator.Get(),
        nullptr,
        IID_PPV_ARGS(&CommandList)
    );
    EnsureOk(hr);
    hr = CommandList->Close();
    EnsureOk(hr);

    hr = Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence));
    EnsureOk(hr);

    auto h = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    IM_ASSERT(h != NULL);
    if(h == NULL) abort();

    FenceEvent.Attach(h);
    if(!FenceEvent) {
        abort();
    }

    {
        ComPtr<IDXGIFactory4> dxgiFactory;
        ComPtr<IDXGISwapChain1> swapChain1;
        hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
        EnsureOk(hr);

        hr = dxgiFactory->CreateSwapChainForHwnd(
            CommandQueue.Get(),
            static_cast<HWND>(platform.GetWindow()),
            &sd,
            nullptr,
            nullptr,
            &swapChain1
        );
        EnsureOk(hr);
        hr = swapChain1->QueryInterface(IID_PPV_ARGS(&SwapChain));
        EnsureOk(hr);

        SwapChain->SetMaximumFrameLatency(BackBufferCount);
        SwapChainWaitableObject.Attach(SwapChain->GetFrameLatencyWaitableObject());
    }

    CreateRenderTarget();

    [[maybe_unused]] auto success = ImGui_ImplDX12_Init(
        Device.Get(),
        FramesInFlightCount,
        DXGI_FORMAT_R8G8B8A8_UNORM, // rtv format
        SrvHeap.Get(), // cbv_srv_heap
        SrvHeap->GetCPUDescriptorHandleForHeapStart(),
        SrvHeap->GetGPUDescriptorHandleForHeapStart()
    );
    IM_ASSERT(success);
}

D3dContext::~D3dContext() {
    WaitForLastSubmittedFrame();
    Images.clear();

    ImGui_ImplDX12_Shutdown();
}

void D3dContext::CreateRenderTarget() {
    for(auto i = 0u; i < BackBufferCount; i++) {
        ComPtr<ID3D12Resource> backBuffer = nullptr;
        SwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer));
        Device->CreateRenderTargetView(backBuffer.Get(), nullptr, Descriptors.at(i));
        Resources.at(i) = backBuffer;
    }
}

void D3dContext::WaitForLastSubmittedFrame() {
    auto& frameCtx = FrameContext.at(FrameIndex % FramesInFlightCount);
    auto fenceVal = frameCtx.FenceVal;
    if(fenceVal == 0) return; // no fence was signaled

    frameCtx.FenceVal = 0;
    if(Fence->GetCompletedValue() >= fenceVal) return;

    Fence->SetEventOnCompletion(fenceVal, FenceEvent);
    WaitForSingleObject(FenceEvent, INFINITE);
}

FrameContext* D3dContext::WaitForNextFrameResources() {
    auto nextFrameIdx = FrameIndex + 1;
    FrameIndex = nextFrameIdx;
    std::array<HANDLE, 2> waitableObjects{SwapChainWaitableObject, nullptr};
    DWORD numWaitableObjects = 1;

    auto& frameCtx = FrameContext.at(nextFrameIdx % FramesInFlightCount);
    auto fenceVal = frameCtx.FenceVal;
    if(fenceVal != 0) {
        frameCtx.FenceVal = 0;
        Fence->SetEventOnCompletion(fenceVal, FenceEvent);
        waitableObjects.at(1) = FenceEvent;
        numWaitableObjects++;
    }

    WaitForMultipleObjects(numWaitableObjects, waitableObjects.data(), TRUE, INFINITE);

    return &frameCtx;
}

std::unique_ptr<DX12Image> D3dContext::TryLoadTextureFromMemory(const void* data, size_t dataSize) {
    int width{0};
    int height{0};
    const unsigned char* charData = static_cast<const unsigned char*>(data);
    auto imageData = stbi_load_from_memory(charData, static_cast<int>(dataSize), &width, &height, NULL, 4);
    if(imageData == NULL) return nullptr;

    ///////////
    D3D12_HEAP_PROPERTIES props;
    memset(&props, 0, sizeof(D3D12_HEAP_PROPERTIES));
    props.Type = D3D12_HEAP_TYPE_DEFAULT;
    props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    D3D12_RESOURCE_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Alignment = 0;
    desc.Width = width;
    desc.Height = height;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    ID3D12Resource* tex = nullptr;
    Device->CreateCommittedResource(
        &props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS(&tex)
    );

    auto uploadPitch =
        (width * 4 + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u);
    auto uploadSize = height * uploadPitch;
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Alignment = 0;
    desc.Width = uploadSize;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    props.Type = D3D12_HEAP_TYPE_UPLOAD;
    props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    ComPtr<ID3D12Resource> uploadBuf;
    auto hr = Device->CreateCommittedResource(
        &props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&uploadBuf)
    );
    EnsureOk(hr);

    // Write pixels into the upload resource
    void* mapped = nullptr;
    D3D12_RANGE range = {0, uploadSize};
    hr = uploadBuf->Map(0, &range, &mapped);
    EnsureOk(hr);

    auto mappedBytes = static_cast<unsigned char*>(mapped);
    for(auto y = 0; y < height; y++) {
        auto pitchOffset = static_cast<size_t>(y) * uploadPitch;
        memcpy(mappedBytes + pitchOffset, imageData + y * width * 4, width * 4);
    }
    uploadBuf->Unmap(0, &range);

    auto srcLocation = D3D12_TEXTURE_COPY_LOCATION{
        .pResource = uploadBuf.Get(),
        .Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
        .PlacedFootprint =
            {
            .Offset = 0,
            .Footprint =
                 {
                  .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                  .Width = static_cast<unsigned int>(width),
                  .Height = static_cast<unsigned int>(height),
                  .Depth = 1,
                  .RowPitch = uploadPitch
                 }
            }
        };

    auto destLocation = D3D12_TEXTURE_COPY_LOCATION{
        .pResource = tex, .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, .SubresourceIndex = 0
    };

    auto barrier = D3D12_RESOURCE_BARRIER{
        .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
        .Transition = {
            .pResource = tex,
            .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
            .StateBefore = D3D12_RESOURCE_STATE_COPY_DEST,
            .StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE}
    };

    // Temp command queue
    ComPtr<ID3D12Fence> fence;
    hr = Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    EnsureOk(hr);

    auto queueDesc = D3D12_COMMAND_QUEUE_DESC{
        .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
        .NodeMask = 1
    };

    ComPtr<ID3D12CommandQueue> cmdQueue;
    hr = Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&cmdQueue));
    EnsureOk(hr);

    ComPtr<ID3D12CommandAllocator> cmdAlloc;
    hr = Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAlloc));
    EnsureOk(hr);

    ComPtr<ID3D12GraphicsCommandList> cmdList;
    hr = Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAlloc.Get(), NULL, IID_PPV_ARGS(&cmdList));
    EnsureOk(hr);

    cmdList->CopyTextureRegion(&destLocation, 0, 0, 0, &srcLocation, NULL);
    cmdList->ResourceBarrier(1, &barrier);
    hr = cmdList->Close();
    EnsureOk(hr);

    // execute the copy
    cmdQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)cmdList.GetAddressOf());
    hr = cmdQueue->Signal(fence.Get(), 1);
    EnsureOk(hr);

    auto event = CreateEvent(0, 0, 0, 0);
    IM_ASSERT(event != NULL);
    CHandle eventHandle(event);

    fence->SetEventOnCompletion(1, eventHandle);
    WaitForSingleObject(eventHandle, INFINITE);

    // Create Shader Resource View (SRV) for texture
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
    Alloc->Alloc(&cpuHandle, &gpuHandle);

    Device->CreateShaderResourceView(tex, &srvDesc, cpuHandle);

    return std::make_unique<DX12Image>(width, height, static_cast<long long>(gpuHandle.ptr), tex);
}

void D3dContext::Resize(UINT width, UINT height) {
    if(width == 0 || height == 0) return;
    WaitForLastSubmittedFrame();
    //CleanupRenderTarget();
    for(auto& buffer : Resources) {
        if(buffer) { 
            buffer.Reset();
        }
    }
    auto hr = SwapChain->ResizeBuffers(
        BackBufferCount, width, height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT
    );
    EnsureOk(hr);
    CreateRenderTarget();
}

bool D3dContext::NextFrame() { 
    if(SwapChainOccluded && SwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) {
        return false;
    }
    SwapChainOccluded = false;
    ImGui_ImplDX12_NewFrame();

    return true;
}

void D3dContext::RenderFrame(const float* clearColorWithAlpha) {
    auto frameCtx = WaitForNextFrameResources();
    auto bbIdx = SwapChain->GetCurrentBackBufferIndex();
    frameCtx->CommandAllocator->Reset();

    auto barrier = D3D12_RESOURCE_BARRIER{
        .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
        .Transition =
            {.pResource = Resources[bbIdx].Get(),
             .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
             .StateBefore = D3D12_RESOURCE_STATE_PRESENT,
             .StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET}
    };

    CommandList->Reset(frameCtx->CommandAllocator.Get(), nullptr);
    CommandList->ResourceBarrier(1, &barrier);
    CommandList->ClearRenderTargetView(Descriptors[bbIdx], clearColorWithAlpha, 0, nullptr);
    CommandList->OMSetRenderTargets(1, &Descriptors[bbIdx], FALSE, nullptr);
    CommandList->SetDescriptorHeaps(1, SrvHeap.GetAddressOf());

    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), CommandList.Get());

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    CommandList->ResourceBarrier(1, &barrier);
    CommandList->Close();

    CommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)CommandList.GetAddressOf());

    auto hr = SwapChain->Present(1, 0); // Present with vsync
    SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);

    auto fenceValue = FenceLastSignaledValue + 1;
    CommandQueue->Signal(Fence.Get(), fenceValue);
    FenceLastSignaledValue = fenceValue;
    frameCtx->FenceVal = fenceValue;
}

void CheckForLeaks() {
    #ifdef DX12_ENABLE_DEBUG_LAYER
        IDXGIDebug1* debug = nullptr;
        if(SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
            debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
            debug->Release();
        }
    #endif
}

#undef EnsureOk
#endif