#if defined(__linux__) && !defined(__ANDROID__)
#include "Platform/Platform.h"
#include "Platform/Graphics.h"

#include <GLFW/glfw3.h>

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <fstream>

void PlatformDebugBreak() {
#ifdef DEBUG
    std::raise(SIGTRAP);
#endif
}

// The CRT debug heap has no portable counterpart; rely on sanitizers instead (see the clang-debug preset).
void InitMemoryCheck() {}
void CheckMemory() {}

namespace {
    static constexpr int WindowWidth = 540;
    static constexpr int WindowHeight = 1110;

    GLFWwindow* g_Window = nullptr;

    void OnGlfwError(int error, const char* description) {
        std::fprintf(stderr, "GLFW error %d: %s\n", error, description);
    }

    std::filesystem::path GetExePath() {
        std::error_code ec;
        auto exe = std::filesystem::read_symlink("/proc/self/exe", ec);
        if(ec) {
            abort();
        }
        return exe.parent_path();
    }
} // namespace

Platform::Platform(void*) {
    glfwSetErrorCallback(OnGlfwError);
    if(!glfwInit()) {
        abort();
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    g_Window = glfwCreateWindow(WindowWidth, WindowHeight, "DRandall Game", nullptr, nullptr);
    if(!g_Window) {
        glfwTerminate();
        abort();
    }
}

Platform::~Platform() noexcept {
    if(g_Window) {
        glfwDestroyWindow(g_Window);
        g_Window = nullptr;
    }
    glfwTerminate();
}

void* Platform::GetWindow() const {
    return g_Window;
}

DebugStream& Platform::GetDebugStream() const {
    static DebugStream stream;
    return stream;
}

std::pair<float, float> Platform::GetScreenSize() const {
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(g_Window, &width, &height);
    return {static_cast<float>(width), static_cast<float>(height)};
}

std::filesystem::path Platform::GetRootPath() const {
    std::filesystem::path path;
    if(const char* dataHome = std::getenv("XDG_DATA_HOME"); dataHome && dataHome[0] != '\0') {
        path = std::filesystem::path(dataHome);
    } else if(const char* home = std::getenv("HOME"); home && home[0] != '\0') {
        path = std::filesystem::path(home) / ".local" / "share";
    } else {
        abort();
    }
    path /= "Durandal";
    return path;
}

bool Platform::TryGetAsset(const char* filename, void** outData, size_t& outSize) const {
    auto assetPath = GetExePath() / "assets" / filename;
    auto stream = std::ifstream(assetPath, std::ios::binary | std::ios::in);
    if(!stream.is_open()) {
        return false;
    }
    outSize = std::filesystem::file_size(assetPath);
    *outData = IM_ALLOC(outSize);
    stream.read(static_cast<char*>(*outData), static_cast<std::streamsize>(outSize));
    return true;
}

bool Platform::HandleInput() {
    glfwPollEvents();
    return !glfwWindowShouldClose(g_Window);
}

#endif
