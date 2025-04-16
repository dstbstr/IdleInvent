#pragma once
#include "Platform/Image.h"

struct DX12Image {
    DX12Image(int width, int height, long long textureId, ID3D12Resource* tex) 
        : Width(width)
        , Height(height)
        , Channels(4)
        , TextureId(textureId)
        , Texture(tex) {}

    ~DX12Image() {
        if(Texture) {
            Texture->Release();
            Texture = nullptr;
        }
    }

    int Width{0};
    int Height{0};
    int Channels{0};
    long long TextureId{0}; // reinterpret cast as needed
    
    ImTextureID ToHandle() const { return reinterpret_cast<ImTextureID>(static_cast<unsigned __int64>(TextureId)); }
    constexpr int GetSize() const { return Width * Height * Channels; }

    ID3D12Resource* Texture{nullptr};
};
