#pragma once

#include "Platform.h"
#include <utility>

namespace Graphics {
    using RenderFn = void (*)();
    bool Initialize(Platform& platform);
    void Shutdown();

    void Render(RenderFn renderFunc);

    void SetFont(const char* fontName, float fontSize);
    bool TryLoadImageFile(const std::string& file);
    void* GetImageHandle(const std::string& file);

    extern float ScreenHeight;
    extern float ScreenWidth;

// Platform specifics
#ifdef WIN32
    using WPARAM = unsigned __int64;
    using LPARAM = __int64;
    void Resize(WPARAM wParam, LPARAM lParam);
#endif

};