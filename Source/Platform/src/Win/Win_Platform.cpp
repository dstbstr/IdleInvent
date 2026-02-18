#ifdef WIN32
#include "Platform/Platform.h"
#include "Platform/Graphics.h"

#define WIN32_LEAN_AND_MEAN
#include <fstream>
#include <Windows.h>
#include <libloaderapi.h>
#include <ShlObj.h>

#include "backends/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#ifdef DEBUG
#include <crtdbg.h>
#define DEBUG_BREAK __debugbreak();
#define CHECK_MEMORY _CrtCheckMemory()
#else
#define CHECK_MEMORY true
#define DEBUG_BREAK
#endif

void PlatformDebugBreak() {
    DEBUG_BREAK
}

void InitMemoryCheck() {
    #ifdef DEBUG
    _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
    #endif
}
void CheckMemory() {
    if(!CHECK_MEMORY) {
        DEBUG_BREAK
    }
}

namespace {
    //static constexpr size_t WindowWidth = 1080;
    //static constexpr size_t WindowHeight = 2220;
    static constexpr size_t WindowWidth = 540;
    static constexpr size_t WindowHeight = 1110;
    HWND hWnd;
    WNDCLASSEXW windowClass;

    std::filesystem::path GetExePath() {
        std::string buffer;
        buffer.resize(MAX_PATH);
        auto len = GetModuleFileNameA(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        if(len == 0 || len == buffer.size()) {
            abort();
        }
        if(len >= buffer.size()) {
            buffer.resize(32 * 1'024);
            len = GetModuleFileNameA(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
            if(len == 0 || len == buffer.size()) {
                abort();
            }
        }
        buffer.resize(len);
        return std::filesystem::path(buffer).parent_path();
    }

    LRESULT WINAPI WndProc(HWND hWndLocal, UINT msg, WPARAM wParam, LPARAM lParam) {
        if(ImGui_ImplWin32_WndProcHandler(hWndLocal, msg, wParam, lParam)) return true;
        switch(msg) {
        case WM_SIZE:
            Graphics::Resize(wParam, lParam);
            return 0;
        case WM_SYSCOMMAND:
            if((wParam & 0xFFF0) == SC_KEYMENU) return 0;
            break;
        case WM_CLOSE:
        case WM_QUIT: PostQuitMessage(0); return 0;
        }
        
        
        return ::DefWindowProcW(hWndLocal, msg, wParam, lParam);
    }

} // namespace
Platform::Platform(void*) { 
    windowClass = {
        sizeof(windowClass),
        CS_CLASSDC,
        WndProc,
        0L,
        0L,
        GetModuleHandle(nullptr),
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        L"DRandall Game",
        nullptr
    };
    ::RegisterClassExW(&windowClass);
    hWnd = ::CreateWindowW(
        windowClass.lpszClassName,
        L"DRandall Game DX12",
        WS_OVERLAPPEDWINDOW,
        100,
        100,
        WindowWidth,
        WindowHeight,
        nullptr,
        nullptr,
        windowClass.hInstance,
        nullptr
    );

    ::ShowWindow(hWnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hWnd);
}

Platform::~Platform() {
    ::DestroyWindow(hWnd);
    ::UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);
}

void* Platform::GetWindow() const { 
    return hWnd; 
}

DebugStream& Platform::GetDebugStream() const {
    static DebugStream stream;
    return stream;
}

std::pair<float, float> Platform::GetScreenSize() const {
    RECT rect;
    if(GetClientRect(hWnd, &rect)) {
        return {static_cast<float>(rect.right - rect.left), static_cast<float>(rect.bottom - rect.top)};
    }
    return {0.0f, 0.0f};
}

std::filesystem::path Platform::GetRootPath() const {
    PWSTR pathStr = nullptr;
    std::filesystem::path path;
    if(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &pathStr) == S_OK) {
        path = std::filesystem::path(pathStr);
    } else {
        abort();
    }
    CoTaskMemFree(pathStr);
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
    stream.read(reinterpret_cast<char*>(*outData), outSize);
    return true;
}

bool Platform::HandleInput() {
    MSG msg;
    while(PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if(msg.message == WM_QUIT) {
            return false;
        }
    }
    return true;
    // return msg.message != WM_QUIT;
}

#endif