#include "Platform/Graphics.h"

#ifdef __ANDROID__
#include "Graphics/Image.h"

#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include "backends/imgui_impl_android.h"
#include "backends/imgui_impl_opengl3.h"
#include <android_native_app_glue.h>

#include "imgui.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Core/Instrumentation/Logging.h"

#ifdef DEBUG
#define GL_CHECK(stmt)                                              \
    do {                                                            \
        stmt;                                                       \
        while(auto err = glGetError() != GL_NO_ERROR) {             \
            Log::Error(std::format("{} OpenGL error: 0x{:04X}", #stmt, err)); \
            return false;                                           \
        }                                                           \
    } while(0)

#define EGL_CHECK(stmt)                                             \
    do {                                                            \
        stmt;                                                       \
        while(auto err = eglGetError() != EGL_SUCCESS) {             \
            Log::Error(std::format("{} EGL error: 0x{:04X}", #stmt, err)); \
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

    bool BindTexture(stbi_uc* data, Image& outImage) {
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

    bool InitializeEgl() {
        Log::Info("Initializing EGL");

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
        
        Log::Info("EGL initialized successfully");
        return true;
    }

    void InitializeImGui() {
        Log::Info("Initializing ImGui");

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

        Graphics::SetFont("DroidSans.ttf", 32.0f);

        Log::Info("ImGui initialized successfully");
        ImGui::GetStyle().ScaleAllSizes(3.0f);
    }
}

namespace Graphics {
    bool Initialize(Platform& platform) {
        Window = static_cast<ANativeWindow*>(platform.GetWindow());
        PlatformPtr = &platform;

        ANativeWindow_acquire(Window);
        ScreenSize = ImVec2(ANativeWindow_getWidth(Window), ANativeWindow_getHeight(Window));

        if(!InitializeEgl()) return false;

        InitializeImGui();

        return true;
    }

    void Shutdown() {
        Log::Info("Tearing down ImGui");
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplAndroid_Shutdown();
        ImGui::DestroyContext();

        Log::Info("Tearing down EGL");
        if(display != EGL_NO_DISPLAY) {
            eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

            if(context != EGL_NO_CONTEXT) { eglDestroyContext(display, context); }
            if(surface != EGL_NO_SURFACE) { eglDestroySurface(display, surface); }
            eglTerminate(display);
        }

        display = EGL_NO_DISPLAY;
        context = EGL_NO_CONTEXT;
        surface = EGL_NO_SURFACE;

        Log::Info("Releasing window");
        ANativeWindow_release(Window);
    }

    std::pair<int, int> GetScreenSize() {
        return std::make_pair(static_cast<int>(ScreenSize.x), static_cast<int>(ScreenSize.y));
    }

    void SetFont(const char* fontName, float fontSize) {
        Log::Info(std::format("Initializing font {}", fontName));

        ImGuiIO& io = ImGui::GetIO();
        
        void* fontData;
        int fontDataSize = PlatformPtr->GetAsset(fontName, &fontData);
        if(fontDataSize == 0) {
            Log::Error("Failed to load font data");
            return;
        }

        ImFont* font = io.Fonts->AddFontFromMemoryTTF(fontData, fontDataSize, fontSize);
        IM_ASSERT(font != nullptr);
        
        Log::Info("Font initialized successfully");
    }

    bool LoadImage(const char* file, Image& outImage) {
        stbi_uc* fileData;
        auto dataSize = PlatformPtr->GetAsset(file, (void**)&fileData);

        auto* data = stbi_load_from_memory(fileData, dataSize, &outImage.Width, &outImage.Height, &outImage.Channels, 0);
        if(!data) return false;

        auto success = BindTexture(data, outImage);

        stbi_image_free(data);

        return success;
    }

    void Render() {
        ImGuiIO& io = ImGui::GetIO();
        static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplAndroid_NewFrame();
        ImGui::NewFrame();

        Ui::Render();

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
            //ImGui::Image((void*)(intptr_t)image.TextureId, ImVec2(image.Width, image.Height));
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
