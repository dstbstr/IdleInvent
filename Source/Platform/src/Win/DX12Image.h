#pragma once

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
    UINT64 TextureId{0}; 
    
    ImTextureID ToHandle() const { return static_cast<ImTextureID>(TextureId); }
    constexpr int GetSize() const { return Width * Height * Channels; }

    ID3D12Resource* Texture{nullptr};
};
