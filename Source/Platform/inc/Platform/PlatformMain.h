#pragma once

#include <Platform/Platform.h>
#include <Platform/Graphics.h>
#include <memory>

struct IGameState {
    virtual ~IGameState() = default;
    virtual bool Initialize() = 0;
    virtual void Run() = 0;
};

std::unique_ptr<IGameState> GetGameState(Platform& platform);

#ifdef WIN32
int main(int, char**) {
    InitMemoryCheck();

    auto platform = Platform{nullptr};
    if(!Graphics::Initialize(platform)) {
        return -1;
    }
    auto gameState = GetGameState(platform);
    if(!gameState->Initialize()) return -1;
    gameState->Run();

    return 0;
}
#elif defined(__ANDROID__)
#include <android_native_app_glue.h>


void android_main(android_app* app) {
    auto platform = Platform{app};
    platform.HandleInput();

    auto gameState = GetGameState(platform);
    if(Graphics::Initialize(platform) && gameState->Initialize(platform)) {
        gameState->Run();
    }
}
#else
#error "Unsupported platform"
#endif
