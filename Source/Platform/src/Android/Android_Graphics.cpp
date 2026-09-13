#ifdef __ANDROID__
#include "Platform/Graphics.h"
#include "AndroidImage.h"

#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include "backends/imgui_impl_android.h"
#include "backends/imgui_impl_opengl3.h"
#include <android_native_app_glue.h>

#include "imgui.h"

#include <optional>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <string>
#include <string_view>
#include <unordered_map>

#ifdef DEBUG
#define GL_CHECK(stmt)                                              \
    do {                                                            \
        stmt;                                                       \
        while(auto err = glGetError() != GL_NO_ERROR) {             \
            IM_ASSERT(false);                          \
            return false;                                           \
        }                                                           \
    } while(0)

#define EGL_CHECK(stmt)                                             \
    do {                                                            \
        stmt;                                                       \
        while(auto err = eglGetError() != EGL_SUCCESS) {             \
            IM_ASSERT(false);                       \
            return false;                                           \
        }                                                           \
    } while(0)
#else
#define GL_CHECK(stmt) stmt
#define EGL_CHECK(stmt) stmt
#endif


namespace {
    EGLDisplay display{EGL_NO_DISPLAY};
    EGLSurface surface{EGL_NO_SURFACE};
    EGLContext context{EGL_NO_CONTEXT};
    ANativeWindow* Window;
    Platform* PlatformPtr;
    ImVec2 ScreenSize{0, 0};
    std::unordered_map<std::string, AndroidImage> Images{};
    std::unordered_map<std::string, ImFont*> Fonts{};
    std::unordered_map<std::string, Sprite> Sprites{};

    std::optional<AndroidImage> TryLoadTextureFromMemory(const void* data, size_t dataSize) {
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
        return AndroidImage{width, height, 4, textureId};
    }
    /*
    bool BindTexture(stbi_uc* data, AndroidImage& outImage) {
        // Create OpenGL texture id
        GL_CHECK(glGenTextures(1, &outImage.TextureId));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, outImage.TextureId));

        GLuint pixelUnpackBuffer;
        GL_CHECK(glGenBuffers(1, &pixelUnpackBuffer));
        GL_CHECK(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pixelUnpackBuffer));
        GL_CHECK(glBufferData(GL_PIXEL_UNPACK_BUFFER, outImage.GetSize(), data, GL_STATIC_DRAW));

        // Setup filtering
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
        GL_CHECK(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));
#endif

        auto imageFormat = outImage.Channels == 4 ? GL_RGBA : GL_RGB;
        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, outImage.Width, outImage.Height, 0, imageFormat, GL_UNSIGNED_BYTE, 0));

        GL_CHECK(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

        return true;
    }
    */

    bool InitializeEgl() {
        EGL_CHECK(display = eglGetDisplay(EGL_DEFAULT_DISPLAY));
        EGL_CHECK(eglInitialize(display, nullptr, nullptr));
        
        const EGLint attributes[] = {EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE};
        EGLint configCount = 0;
        EGL_CHECK(eglChooseConfig(display, attributes, nullptr, 0, &configCount));

        EGLConfig config;
        EGL_CHECK(eglChooseConfig(display, attributes, &config, 1, &configCount));

        EGLint format;
        EGL_CHECK(eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format));

        ANativeWindow_setBuffersGeometry(Window, 0, 0, format);

        const EGLint ctxAttributes[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
        EGL_CHECK(context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctxAttributes));
        EGL_CHECK(surface = eglCreateWindowSurface(display, config, Window, nullptr));
        EGL_CHECK(eglMakeCurrent(display, surface, surface, context));
        
        return true;
    }

    void InitializeImGui() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        //iniFile = std::string(platform.GetInternalDataPath) + "/imgui.ini";
        //io.IniFilename = iniFile.c_str();

        ImGui::StyleColorsDark();
        ImGui::GetStyle().FrameRounding = 10.0f;

        ImGui_ImplAndroid_Init(Window);
        ImGui_ImplOpenGL3_Init();

        io.DisplaySize = ScreenSize;
        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

        ImGui::GetStyle().ScaleAllSizes(3.0f);
    }
}

namespace Graphics {
    float ScreenHeight = 0;
    float ScreenWidth = 0;

    bool Initialize(Platform& platform) {
        Window = static_cast<ANativeWindow*>(platform.GetWindow());
        PlatformPtr = &platform;

        ANativeWindow_acquire(Window);
        ScreenSize = ImVec2(ANativeWindow_getWidth(Window), ANativeWindow_getHeight(Window));
        ScreenHeight = ScreenSize.y;
        ScreenWidth = ScreenSize.x;

        if(!InitializeEgl()) return false;

        InitializeImGui();

        return true;
    }

    void Shutdown() {
        Fonts.clear();
        Sprites.clear();
        Images.clear();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplAndroid_Shutdown();
        ImGui::DestroyContext();

        if(display != EGL_NO_DISPLAY) {
            eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

            if(context != EGL_NO_CONTEXT) { eglDestroyContext(display, context); }
            if(surface != EGL_NO_SURFACE) { eglDestroySurface(display, surface); }
            eglTerminate(display);
        }

        display = EGL_NO_DISPLAY;
        context = EGL_NO_CONTEXT;
        surface = EGL_NO_SURFACE;

        ANativeWindow_release(Window);
    }

    bool TryLoadFont(const std::string& id, const char* fontName, float fontSize) {
        constexpr std::string_view fontsDir = "Fonts/";
        
        auto resolvedName = std::string(fontName);
        if(!resolvedName.starts_with(fontsDir)) {
            resolvedName = std::string(fontsDir) + resolvedName;
        }

        void* fontData = nullptr;
        size_t fontDataSize = 0;
        if(!PlatformPtr->TryGetAsset(resolvedName.c_str(), &fontData, fontDataSize)) {
            return false;
        }

        auto& io = ImGui::GetIO();
        auto* font = io.Fonts->AddFontFromMemoryTTF(fontData, static_cast<int>(fontDataSize), fontSize);
        if(!font) return false;

        Fonts.emplace(id, font);
        return true;
    }

    ImFont* GetFont(const std::string& id) {
        IM_ASSERT(Fonts.contains(id));
        return Fonts.at(id);
    }

    bool TryLoadImageFile(const std::string& file) {
        if(Images.contains(file)) return true;

        void* fileData = nullptr;
        size_t fileSize = 0;

        if(!PlatformPtr->TryGetAsset(file.c_str(), &fileData, fileSize)) {
            IM_ASSERT(false);
            return false;
        }

        auto img = TryLoadTextureFromMemory(fileData, fileSize);
        IM_FREE(fileData);
        if(!img) return false;

        Images.emplace(file, std::move(*img));
        return true;
    }

    // TODO: Untested
    bool TryLoadSpriteRegions(const std::string& file, std::vector<SpriteRegion>& outRegions) {
        void* fileData = nullptr;
        size_t fileSize = 0;
        if(!PlatformPtr->TryGetAsset(file.c_str(), &fileData, fileSize)) return false;
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

        const auto& sheet = Images.at(file);
        const auto handle = sheet.ToHandle();
        const float w = static_cast<float>(sheet.Width);
        const float h = static_cast<float>(sheet.Height);

        for(const auto& r: regions) {
            const ImVec2 uvMin{static_cast<float>(r.X) / w, static_cast<float>(r.Y) / h};
            const ImVec2 uvMax{static_cast<float>(r.X + r.Width) / w, static_cast<float>(r.Y + r.Height) / h};
            Sprites[r.Name] = Sprite{handle, uvMin, uvMax};
        }

        return true;
    }

    Sprite GetSprite(const std::string& name) {
        IM_ASSERT(Sprites.contains(name));
        return Sprites.at(name);
    }

    bool IsImageValid(const std::string& name) { return Images.contains(name); }

    bool IsSpriteValid(const std::string& name) { return Sprites.contains(name); }

    ImTextureID GetImageHandle(const std::string& file) {
        IM_ASSERT(Images.contains(file));
        return Images.at(file).ToHandle();
    }

    void Render(void (*renderFn)()) {
        ImGuiIO& io = ImGui::GetIO();
        static ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplAndroid_NewFrame();
        ImGui::NewFrame();

        renderFn();

        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        eglSwapBuffers(display, surface);
    }
}
#endif

/*
            // Show the ImGui demo window
            static bool show_demo_window = false;
            if(show_demo_window) { ImGui::ShowDemoWindow(&show_demo_window); }

            // ImGui::Begin("Texture Test");
            //ImGui::Begin("Texture Test", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            // ImGui::Text("Size: %d x %d", image.width, image.height);
            //ImGui::Image(image.ToHandle(), ImVec2(image.Width, image.Height));
            //ImGui::End();

            // create window in code
            {
            // static bool show_another_window = false;
            // static float f = 0.0f;
            // static int counter = 0;

            // ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

            // ImGui::Text("This is some useful text"); // Display some text (you can use a format strings too)
            // ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
            // ImGui::Checkbox("Another Window", &show_another_window);

            // ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Single float slider
            // ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            // counter += ImGui::Button("Button"); // Buttons return true when clicked (most widgets return true when edited/activated)
            // ImGui::SameLine();
            // ImGui::Text("counter = %d", counter);

            // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            // ImGui::End();
            }

            // another sample window
            // if(show_another_window) {
            // ImGui::Begin("Another Window", &show_another_window);
            // ImGui::Text("Hello from another window!");
            // show_another_window = ImGui::Button("Close Me");
            // ImGui::End();
            }
*/
