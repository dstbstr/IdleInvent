#pragma once
#include <imgui.h>
#include <GL/gl.h>
#include <utility>

struct GlImage {
    GlImage(int width, int height, int channels, GLuint textureId)
        : Width(width)
        , Height(height)
        , Channels(channels)
        , TextureId(textureId) {}

    GlImage(const GlImage&) = delete;
    GlImage& operator=(const GlImage&) = delete;
    GlImage(GlImage&& other) noexcept
        : Width(other.Width)
        , Height(other.Height)
        , Channels(other.Channels)
        , TextureId(std::exchange(other.TextureId, 0)) {}
    GlImage& operator=(GlImage&& other) noexcept {
        std::swap(Width, other.Width);
        std::swap(Height, other.Height);
        std::swap(Channels, other.Channels);
        std::swap(TextureId, other.TextureId);
        return *this;
    }

    ~GlImage() {
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
