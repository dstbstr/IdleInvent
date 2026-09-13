#pragma once

#include <Platform/Audio.h>
#include <Platform/Platform.h>
#include <Platform/Graphics.h>
#include <memory>

struct IGameState {
    virtual ~IGameState() = default;
    IGameState() = default;
    IGameState(const IGameState&) = delete;
    IGameState& operator=(const IGameState&) = delete;
    IGameState(IGameState&&) = delete;
    IGameState& operator=(IGameState&&) = delete;

    virtual bool Initialize() = 0;
    virtual void Run() = 0;
};

std::unique_ptr<IGameState> GetGameState(Platform& platform);

#if defined(WIN32) || (defined(__linux__) && !defined(__ANDROID__))
int main(int, char**) {
    InitMemoryCheck();

    auto platform = Platform{nullptr};
    if(!Graphics::Initialize(platform)) return -1;
    if(!Audio::Initialize(platform)) {
        Graphics::Shutdown();
        return -1;
    }

    auto gameState = GetGameState(platform);
    auto initialized = gameState->Initialize();
    if(initialized) {
        gameState->Run();
    }

    gameState.reset();
    Audio::Shutdown();
    Graphics::Shutdown();
    return initialized ? 0 : -1;
}

#elif defined(__ANDROID__)
#include <android_native_app_glue.h>

void android_main(android_app* app) {
    auto platform = Platform{app};
    if(!platform.HandleInput()) return;

    if(!Graphics::Initialize(platform)) return;
    if(!Audio::Initialize(platform)) {
        Graphics::Shutdown();
    }

    auto gameState = GetGameState(platform);
    if(gameState->Initialize()) {
        gameState->Run();
    }

    gameState.reset();
    Audio::Shutdown();
    Graphics::Shutdown();
}

#else
#error "Unsupported platform"
#endif
