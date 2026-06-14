#pragma once

#include "Platform.h"
#include <imgui.h> // Do I want this dependency, or change ImTextureID to void*?
#include <utility>

struct SpriteRegion {
    std::string Name;
    size_t X, Y, Width, Height;
};

struct Sprite {
    ImTextureID Texture{};
    ImVec2 UvMin{0.0f, 0.0f};
    ImVec2 UvMax{1.0f, 1.0f};
};

namespace Graphics {
    using RenderFn = void (*)();
    bool Initialize(Platform& platform);
    void Shutdown();

    void Render(RenderFn renderFunc);

    bool TryLoadFont(const std::string& id, const char* fontName, float fontSize);
    ImFont* GetFont(const std::string& id);

    bool TryLoadImageFile(const std::string& file);
    bool TryLoadSpriteRegions(const std::string& file, std::vector<SpriteRegion>& outRegions);
    bool TryLoadSpriteSheet(const std::string& file); // assumes fileName.txt for details
    bool TryLoadSpriteSheet(const std::string& file, const std::vector<SpriteRegion>& regions);

    bool IsImageValid(const std::string& name);
    bool IsSpriteValid(const std::string& name);
    Sprite GetSprite(const std::string& name);
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