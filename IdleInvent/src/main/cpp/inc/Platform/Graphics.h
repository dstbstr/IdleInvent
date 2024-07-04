#pragma once

#include "Platform.h"
#include <utility>
#include "Ui/Ui.h"

struct Image;
namespace Graphics {
    bool Initialize(Platform& platform);
    void Shutdown();

    void Render(void(*rederFn)());

    void SetFont(const char* fontName, float fontSize);
    bool LoadImage(const char* file, Image& outImage);
    void UnloadImage(Image& image);

    std::pair<int, int> GetScreenSize();
};