#ifdef WIN32
#include "Platform/Graphics.h"
#include "D3dContext.h"

#include "backends/imgui_impl_win32.h"

#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>
#include <fstream>
#include <unordered_map>
#include <stb_image.h>
#include <string_view>

// #include <iostream>

namespace {
    std::unique_ptr<D3dContext> g_Ctx{nullptr};
    std::unordered_map<std::string, ImFont*> g_Fonts{};
    std::unordered_map<std::string, Sprite> g_Sprites{};
    Platform* g_Platform = nullptr;
    
    bool InitializeDx(Platform& platform) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        
        if(!ImGui_ImplWin32_Init(platform.GetWindow())) {
            IM_ASSERT(false);
            return false;
        }
        auto& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        ImGui::StyleColorsDark();

        g_Ctx = std::make_unique<D3dContext>(platform);
        return true;
    }
}

namespace Graphics {
    float ScreenHeight = 0;
    float ScreenWidth = 0;

    bool Initialize(Platform& platform) {
        g_Platform = &platform;

        if(!InitializeDx(platform)) return false;

        auto [width, height] = platform.GetScreenSize();
        ScreenWidth = static_cast<float>(width);
        ScreenHeight = static_cast<float>(height);

        return true;
    }

    void Shutdown() { 
        g_Ctx.reset();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        CheckForLeaks();
    }

    void Resize(WPARAM wParam, LPARAM lParam) {
        if(g_Ctx && wParam != SIZE_MINIMIZED) {
            g_Ctx->Resize((UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
            auto [width, height] = g_Platform->GetScreenSize();
            ScreenWidth = width;
            ScreenHeight = height;
        }
    }

    bool TryLoadFont(const std::string& id, const char* fontName, float fontSize) { 
        constexpr std::string_view fontsDir = "Fonts/";

        auto resolvedName = std::string(fontName);
        if(!resolvedName.starts_with(fontsDir)) {
            resolvedName = std::string(fontsDir) + resolvedName;
        }

        void* fontData = nullptr;
        size_t fontDataSize = 0;
        if(!g_Platform->TryGetAsset(resolvedName.c_str(), &fontData, fontDataSize)) {
            return false;
        }

        const auto& io = ImGui::GetIO();
        auto* font = io.Fonts->AddFontFromMemoryTTF(fontData, static_cast<int>(fontDataSize), fontSize);
        if(font) {
            g_Fonts.emplace(id, font);
            return true;
        }
        return false;
    }

    ImFont* GetFont(const std::string& id) {
        IM_ASSERT(g_Fonts.contains(id));
        return g_Fonts.at(id);
    }

    void SetFont(const char* fontName, float fontSize) {
        ImGuiIO& io = ImGui::GetIO();

        void* fontData = nullptr;
        size_t fontDataSize = 0;
        if(!g_Platform->TryGetAsset(fontName, &fontData, fontDataSize)) {
            IM_ASSERT(false);
            abort();
        }

        ImFont* font = io.Fonts->AddFontFromMemoryTTF(fontData, static_cast<int>(fontDataSize), fontSize);
        if(!font) {
            IM_ASSERT(false);
            abort();
        }
    }

    bool TryLoadImageFile(const std::string& file) {
        if(g_Ctx->Images.contains(file)) return true;

        void* fileData = nullptr;
        size_t fileSize = 0;
        if(!g_Platform->TryGetAsset(file.c_str(), &fileData, fileSize)) {
            return false;
        }

        auto img = g_Ctx->TryLoadTextureFromMemory(fileData, fileSize);
        if(!img) {
            return false;
        }
        g_Ctx->Images.emplace(file, std::move(img));
        return true;
    }

    bool TryLoadSpriteRegions(const std::string& file, std::vector<SpriteRegion>& outRegions) {
        void* fileData = nullptr;
        size_t fileSize = 0;
        if(!g_Platform->TryGetAsset(file.c_str(), &fileData, fileSize)) return false;
        // returned buffer is not null terminated, so create with the known size
        std::string txtData(static_cast<const char*>(fileData), fileSize);
        IM_FREE(fileData);

        std::istringstream ss(txtData);
        std::string line;
        while(std::getline(ss, line)) {
            if(line.empty()) continue;
            std::istringstream lineStream(line);
            SpriteRegion region{};
            lineStream >> region.Name >> region.X >> region.Y >> region.Width >> region.Height;
            outRegions.push_back(region);
        }
        return true;
    }

    bool TryLoadSpriteSheet(const std::string& file) {
        auto txtName = file.substr(0, file.find_last_of('.')) + ".txt";
        std::vector<SpriteRegion> regions;
        if(!TryLoadSpriteRegions(txtName, regions)) return false;
        return TryLoadSpriteSheet(file, regions);
    }

    bool TryLoadSpriteSheet(const std::string& file, const std::vector<SpriteRegion>& regions) {
        void* fileData = nullptr;
        size_t fileSize = 0;
        if(!g_Platform->TryGetAsset(file.c_str(), &fileData, fileSize)) return false;

        if(!g_Ctx->Images.contains(file)) {
            auto sheet = g_Ctx->TryLoadTextureFromMemory(fileData, fileSize);
            if(!sheet) return false;
            g_Ctx->Images.emplace(file, std::move(sheet));
        }

        const auto& sheet = *g_Ctx->Images.at(file);
        const auto handle = sheet.ToHandle();
        const float w = static_cast<float>(sheet.Width);
        const float h = static_cast<float>(sheet.Height);

        for(const auto& r : regions) {
            const ImVec2 uvMin{static_cast<float>(r.X) / w, static_cast<float>(r.Y) / h};
            const ImVec2 uvMax{static_cast<float>(r.X + r.Width) / w, static_cast<float>(r.Y + r.Height) / h};
            g_Sprites[r.Name] = Sprite{handle, uvMin, uvMax};
        }

        return true;
    }

    Sprite GetSprite(const std::string& name) {
        IM_ASSERT(g_Sprites.contains(name));
        return g_Sprites.at(name);
    }

    bool IsImageValid(const std::string& name) {
        return g_Ctx && g_Ctx->Images.contains(name); 
    }

    bool IsSpriteValid(const std::string& name) { 
        return g_Sprites.contains(name); 
    }

    ImTextureID GetImageHandle(const std::string& file) {
        IM_ASSERT(g_Ctx && g_Ctx->Images.contains(file));
        return g_Ctx->Images.at(file)->ToHandle();
    }

    void Render(RenderFn renderFn) {
        static ImVec4 clearColor = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
        static const std::array<float, 4> clearColorWithAlpha = {
            clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w
        };

        if(!g_Ctx->NextFrame()) {
            // Sleep for a bit to avoid busy waiting
            ::Sleep(10);
            return;
        }
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        
        renderFn();
        
        //(void)renderFn;
        //static bool showWindow = true;
        //ImGui::ShowDemoWindow(&showWindow);

        ImGui::Render();

        g_Ctx->RenderFrame(clearColorWithAlpha.data());
    }
} // namespace Graphics
#endif