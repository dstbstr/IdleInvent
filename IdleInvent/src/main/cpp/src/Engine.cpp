#include "Engine.h"
#include "EngineState.h"

#include "Platform/Graphics.h"
#include "Platform/Platform.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/Instrumentation/Logging.h"
#include "Core/NumTypes.h"

#include <format>
#include <chrono>
#include <memory>

namespace {
    using namespace std::chrono_literals;

    std::chrono::time_point<std::chrono::steady_clock> LastTickTime = std::chrono::steady_clock::now();
}

Engine::Engine(Platform& platform) {
    Log::Info("Initializing engine");
    if(!Graphics::Initialize(platform)) abort();    

    ServiceLocator::Get().CreateIfMissing<EngineState>();
}

Engine::~Engine() {
    Log::Info("Tearing down engine");

    Graphics::Shutdown();

    Log::Info("Shutdown complete");
}

void Engine::Tick() const {
    auto& engineState = ServiceLocator::Get().GetRequired<EngineState>();
    auto now = std::chrono::steady_clock::now();
    engineState.FrameNum++;
    engineState.ElapsedTime += std::chrono::duration_cast<std::chrono::milliseconds>(now - LastTickTime);
    LastTickTime = now;

    Graphics::Render();
}