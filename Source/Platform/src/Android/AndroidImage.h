#pragma once
#include <imgui.h>
#include <GLES3/gl3.h>

struct AndroidImage {
    AndroidImage(int width, int height, int channels)
        : Width(width), Height(height), Channels(channels), TextureId(0) {}

    ~AndroidImage() {
        /*
        if(TextureId) {
            glDeleteTextures(1, &TextureId);
            TextureId = 0;
        }
        */
    }
    int Width{0};
    int Height{0};
    int Channels{0};
    GLuint TextureId{0};

    constexpr int GetSize() const { return Width * Height * Channels; }
    ImTextureID ToHandle() const { return reinterpret_cast<ImTextureID>(TextureId); }
};