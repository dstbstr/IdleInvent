#pragma once

#include "Platform.h"
#include <imgui.h> // Do I want this dependency, or change ImTextureID to void*?
#include <utility>

namespace Graphics {
    using RenderFn = void (*)();
    bool Initialize(Platform& platform);
    void Shutdown();

    void Render(RenderFn renderFunc);

    bool TryLoadFont(const std::string& id, const char* fontName, float fontSize);
    ImFont* GetFont(const std::string& id);

    bool TryLoadImageFile(const std::string& file);
    ImTextureID GetImageHandle(const std::string& file);

    extern float ScreenHeight;
    extern float ScreenWidth;

// Platform specifics
#ifdef WIN32
    using WPARAM = unsigned __int64;
    using LPARAM = __int64;
    void Resize(WPARAM wParam, LPARAM lParam);
#endif

};