#pragma once

struct Dims {
    int Width;
    int Height;

    constexpr int Area() const { return Width * Height; }
};

struct DX12Image {
    DX12Image(Dims dims, UINT64 textureId, ID3D12Resource* tex) 
        : Dims(dims)
        , Channels(4)
        , TextureId(textureId)
        , Texture(tex) {}

    DX12Image(const DX12Image&) = default;
    DX12Image& operator=(const DX12Image&) = default;
    DX12Image(DX12Image&&) noexcept = default;
    DX12Image& operator=(DX12Image&&) noexcept = default;

    ~DX12Image() {
        if(Texture) {
            Texture->Release();
            Texture = nullptr;
        }
    }

    Dims Dims{};
    int Channels{0};
    UINT64 TextureId{0}; 
    
    ImTextureID ToHandle() const { return static_cast<ImTextureID>(TextureId); }
    constexpr int GetSize() const { return Dims.Area() * Channels; }

    ID3D12Resource* Texture{nullptr};
};
