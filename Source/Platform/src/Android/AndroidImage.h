#pragma once
#include <imgui.h>
#include <GLES3/gl3.h>
#include <utility>

struct AndroidImage {
    AndroidImage(int width, int height, int channels, GLuint textureId)
        : Width(width), Height(height), Channels(channels), TextureId(textureId) {}

    AndroidImage(const AndroidImage&) = delete;
    AndroidImage& operator=(const AndroidImage&) = delete;
    AndroidImage(AndroidImage&& other) noexcept
        : Width(other.Width)
        , Height(other.Height)
        , Channels(other.Channels)
        , TextureId(std::exchange(other.TextureId, 0)) {}
    AndroidImage& operator=(AndroidImage&& other) noexcept {
        std::swap(Width, other.Width);
        std::swap(Height, other.Height);
        std::swap(Channels, other.Channels);
        std::swap(TextureId, other.TextureId);
        return *this;
    }

    ~AndroidImage() {
        if(TextureId) {
            glDeleteTextures(1, &TextureId);
            TextureId = 0;
        }
    }

    int Width{0};
    int Height{0};
    int Channels{0};
    GLuint TextureId{0};

    ImTextureID ToHandle() const { return static_cast<ImTextureID>(TextureId); }
    constexpr int GetSize() const { return Width * Height * Channels; }
};