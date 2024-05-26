#pragma once

#include <GLES3/gl3.h>

struct Image {
    int Width{0}; 
    int Height{0};
    int Channels{0};
    GLuint TextureId{0};

    constexpr int GetSize() const {
        return Width * Height * Channels;
    }
};