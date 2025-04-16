#pragma once

#include <imgui.h>

struct Image {
    virtual ~Image() = default;
    int Width{0};
    int Height{0};
    int Channels{0};
    long long TextureId{0}; // reinterpret cast as needed

    constexpr int GetSize() const { return Width * Height * Channels; }

    ImTextureID ToHandle() const;
};