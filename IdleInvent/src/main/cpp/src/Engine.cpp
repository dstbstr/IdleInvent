#include "Engine.h"
#include "EngineState.h"

#include "Platform/Graphics.h"
#include "Platform/Platform.h"
#include "Platform/GameLog.h"
#include "Ui/Splash.h"
#include "Ui/StartLife.h"
#include "Ui/Ui.h"

#include "InventLib/EntryPoint.h"
#include "InventLib/GameState/SaveState.h"
#include "InventLib/GameState/GameTime.h"
#include "InventLib/GameState/GameState.h"
#include "InventLib/GameState/FileOperation.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"
#include "Core/Instrumentation/Logging.h"
#include "Core/NumTypes.h"

#include <format>
#include <chrono>
#include <memory>

namespace {
    auto LastTickTime = std::chrono::steady_clock::now();
    auto LastSaveTime = std::chrono::steady_clock::now();

    EngineState* engineState{nullptr};
    Invent::SaveState* saveState;
    Platform* platform{nullptr};
    ServiceLocator* services{nullptr};

    std::unique_ptr<GameLog> gameLog {nullptr};
}

Engine::Engine(Platform& inPlatform, Invent::SaveState& inSaveState) {
    //std::regex logFileRegex = std::regex(R"(^(Technology.cpp)|(Invention.cpp))");
    //gameLog = std::make_unique<GameLog>(Log::Filter{}.WithFilePattern(logFileRegex));
    gameLog = std::make_unique<GameLog>(Log::Filter{}.WithLevel(Log::Level::Info));
#ifdef DEBUG
    Log::Info("Debug Mode");
#else
    Log::Info("Release Mode");
#endif
    platform = &inPlatform;

    saveState = &inSaveState;
    // clean start
    platform->DeleteGame();
    *saveState = Invent::SaveState{};

    services = &ServiceLocator::Get();

    Invent::EntryPoint::Initialize();
    Invent::EntryPoint::Load(inSaveState);

    Log::Info("Initializing engine");
    DR_ASSERT_MSG(Graphics::Initialize(inPlatform), "Failed to initialize platform");

    engineState = &ServiceLocator::Get().GetOrCreate<EngineState>();
    
    DR_ASSERT_MSG(Ui::Splash::Initialize(), "Failed to initialize splash screen");
    Graphics::Render(Ui::Splash::Render);

    DR_ASSERT_MSG(Ui::StartLife::Initialize(), "Failed to initialize start life screen");
    DR_ASSERT_MSG(Ui::Initialize(), "Failed to initialize UI");
    // TODO: Set Life started based on save data

    services->GetOrCreate<PubSub<Invent::FileOperation>>().Subscribe([](const Invent::FileOperation& request) {
        switch(request) {
            case Invent::FileOperation::Save:
                Invent::EntryPoint::Save(*saveState);
                platform->SaveGame(*saveState);
                break;
            case Invent::FileOperation::Load:
                *saveState = platform->LoadGame().value_or(Invent::SaveState{});
                Invent::EntryPoint::Reset();
                Invent::EntryPoint::Load(*saveState);
                break;
            case Invent::FileOperation::Delete:
                platform->DeleteGame();
                *saveState = Invent::SaveState{};
                Invent::EntryPoint::Reset();
                Invent::EntryPoint::Load(*saveState);
                break;
        }
    });
    // Cheating
    
}

Engine::~Engine() {
    Log::Info("Tearing down engine");

    Ui::ShutDown();
    Ui::Splash::ShutDown();
    Ui::StartLife::ShutDown();
    Graphics::Shutdown();
    Log::Flush();

    Invent::EntryPoint::ShutDown();

    gameLog.reset();
}

void Engine::Tick() const {
    using namespace std::chrono_literals;

    Log::Flush();

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<Invent::BaseTime>(now - LastTickTime);
    engineState->FrameNum++;
    engineState->ElapsedTime += elapsed;
    LastTickTime = now;

    Invent::EntryPoint::Tick(elapsed);
    Ui::StartLife::IsDone() ? Graphics::Render(Ui::Render) : Graphics::Render(Ui::StartLife::Render);

    if(now - LastSaveTime > 30s) {
        LastSaveTime = now;
        //services->GetRequired<PubSub<Invent::FileOperation>>().Publish(Invent::FileOperation::Save);
    }
    // TODO: Sleep to set FPS
}