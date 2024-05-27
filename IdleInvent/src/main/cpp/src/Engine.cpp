#include "Engine.h"
#include "EngineState.h"

#include "Platform/Graphics.h"
#include "Platform/Platform.h"
#include "Platform/GameLog.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/Instrumentation/Logging.h"
#include "Core/NumTypes.h"

#include "InventLib/EntryPoint.h"
#include "InventLib/GameState/GameTime.h"

#include <format>
#include <chrono>
#include <memory>

namespace {
    std::chrono::time_point<std::chrono::steady_clock> LastTickTime = std::chrono::steady_clock::now();

    auto log = GameLog{Log::Filter{}};
}

Engine::Engine(Platform& platform) {
    Log::Initialize();
#ifdef DEBUG
    Log::Info("Debug Mode");
#else
    Log::Info("Release Mode");
#endif

    Log::Info("Initializing engine");
    if(!Graphics::Initialize(platform)) abort();    

    ServiceLocator::Get().CreateIfMissing<EngineState>();
    Invent::EntryPoint::Initialize();
}

Engine::~Engine() {
    Log::Info("Tearing down engine");

    Graphics::Shutdown();

    Log::Info("Shutdown complete");
}

void Engine::Tick() const {
    Log::Flush();

    static auto& engineState = ServiceLocator::Get().GetRequired<EngineState>();
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<Invent::BaseTime>(now - LastTickTime);
    engineState.FrameNum++; 
    engineState.ElapsedTime += elapsed;
    LastTickTime = now;

    Invent::EntryPoint::Tick(elapsed);
    Graphics::Render();
}