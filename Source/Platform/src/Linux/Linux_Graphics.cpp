#if defined(__linux__) && !defined(__ANDROID__)
#include "Platform/Graphics.h"
#include "GlImage.h"

#include <GLFW/glfw3.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace {
    // Matches the core profile requested in Linux_Platform.cpp
    constexpr const char* GlslVersion = "#version 330";

    GLFWwindow* g_Window = nullptr;
    Platform* g_Platform = nullptr;
    std::unordered_map<std::string, ImFont*> g_Fonts{};
    std::unordered_map<std::string, Sprite> g_Sprites{};
    std::unordered_map<std::string, GlImage> g_Images{};

    std::optional<GlImage> TryLoadTextureFromMemory(const void* data, size_t dataSize) {
        int width = 0;
        int height = 0;
        int channels = 0;
        auto* pixels = stbi_load_from_memory(
            static_cast<const stbi_uc*>(data), static_cast<int>(dataSize), &width, &height, &channels, 4
        );
        if(!pixels) return std::nullopt;

        GLuint textureId = 0;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(pixels);

        // stbi reports the channel count of the source, but we always request RGBA
        return GlImage{width, height, 4, textureId};
    }

    bool InitializeGl(Platform& platform) {
        g_Window = static_cast<GLFWwindow*>(platform.GetWindow());

        glfwMakeContextCurrent(g_Window);
        glfwSwapInterval(1); // vsync

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        if(!ImGui_ImplGlfw_InitForOpenGL(g_Window, true)) {
            IM_ASSERT(false);
            return false;
        }
        if(!ImGui_ImplOpenGL3_Init(GlslVersion)) {
            IM_ASSERT(false);
            return false;
        }

        auto& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        ImGui::StyleColorsDark();

        return true;
    }
} // namespace

namespace Graphics {
    float ScreenHeight = 0;
    float ScreenWidth = 0;

    bool Initialize(Platform& platform) {
        g_Platform = &platform;

        if(!InitializeGl(platform)) return false;

        auto [width, height] = platform.GetScreenSize();
        ScreenWidth = width;
        ScreenHeight = height;

        return true;
    }

    void Shutdown() {
        g_Images.clear();
        g_Sprites.clear();
        g_Fonts.clear();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        g_Window = nullptr;
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

        // ImGui takes ownership of fontData and frees it with the atlas
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

    bool TryLoadImageFile(const std::string& file) {
        if(g_Images.contains(file)) return true;

        void* fileData = nullptr;
        size_t fileSize = 0;
        if(!g_Platform->TryGetAsset(file.c_str(), &fileData, fileSize)) {
            return false;
        }

        auto img = TryLoadTextureFromMemory(fileData, fileSize);
        IM_FREE(fileData);
        if(!img) {
            return false;
        }
        g_Images.emplace(file, std::move(*img));
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
        if(!TryLoadImageFile(file)) return false;

        const auto& sheet = g_Images.at(file);
        const auto handle = sheet.ToHandle();
        const float w = static_cast<float>(sheet.Width);
        const float h = static_cast<float>(sheet.Height);

        for(const auto& r: regions) {
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
        return g_Images.contains(name);
    }

    bool IsSpriteValid(const std::string& name) {
        return g_Sprites.contains(name);
    }

    ImTextureID GetImageHandle(const std::string& file) {
        IM_ASSERT(g_Images.contains(file));
        return g_Images.at(file).ToHandle();
    }

    void Render(RenderFn renderFn) {
        static constexpr ImVec4 clearColor = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(g_Window, &width, &height);
        if(width == 0 || height == 0) {
            // Minimized/occluded: nothing to draw, and ImGui dislikes a zero display size
            return;
        }
        ScreenWidth = static_cast<float>(width);
        ScreenHeight = static_cast<float>(height);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        renderFn();

        ImGui::Render();

        glViewport(0, 0, width, height);
        glClearColor(
            clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w
        );
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(g_Window);
    }
} // namespace Graphics
#endif
