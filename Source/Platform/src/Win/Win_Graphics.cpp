#ifdef WIN32
#include "Platform/Graphics.h"
#include "D3dContext.h"

#include "backends/imgui_impl_win32.h"

#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>
#include <fstream>
#include <unordered_map>

// #include <iostream>

namespace {
    std::unique_ptr<D3dContext> g_Ctx{nullptr};
    std::unordered_map<std::string, ImFont*> g_Fonts{};
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

    bool TryLoadFont(const std::string& id, const char* fontName, float fontSize) { const auto& io = ImGui::GetIO();
        void* fontData;
        size_t fontDataSize;
        if(!g_Platform->TryGetAsset(fontName, &fontData, fontDataSize)) {
            return false;
        }

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

        void* fontData;
        size_t fontDataSize;
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

        void* fileData;
        size_t fileSize;
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

    ImTextureID GetImageHandle(const std::string& file) {
        IM_ASSERT(g_Ctx && g_Ctx->Images.contains(file));
        return g_Ctx->Images.at(file)->ToHandle();
    }

    void Render(RenderFn renderFn) {
        static ImVec4 clearColor = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
        static const float clearColorWithAlpha[4] = {
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

        g_Ctx->RenderFrame(clearColorWithAlpha);
    }
} // namespace Graphics
#endif