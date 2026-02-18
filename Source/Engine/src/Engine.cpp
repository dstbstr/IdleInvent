#include "Engine.h"
#include "EngineState.h"
#include "Game.h"

#include "GameState/GameTime.h"
#include "GameState/GameState.h"
#include "GameState/FileOperation.h"

#include "DesignPatterns/ServiceLocator.h"
#include "DesignPatterns/PubSub.h"
#include "Instrumentation/Logging.h"
#include "Instrumentation/ISink.h"

#include <Platform/NumTypes.h>
#include <Platform/Graphics.h>
#include <Platform/Platform.h>
#include <Platform/DebugStream.h>

#include <format>
#include <chrono>
#include <memory>

namespace {
    struct GameLog : public Log::ISink {
        GameLog(const Log::Filter& filter) : Log::ISink(filter) {}
        void Write(const Log::Entry& entry) override {
            std::string msg;
            if(entry.LogLevel < Log::Level::Warning) {
                msg = std::format("{}", entry.Message);
            } else {
                msg = std::format(
                    "[{}] {}::{}:{} - {}",
                    ToString(entry.LogLevel),
                    entry.Context.FileName,
                    entry.Context.Function,
                    entry.Context.LineNumber,
                    entry.Message
                );
            }

            stream << msg << std::endl << std::flush;
            if(entry.LogLevel >= Log::Level::Error) {
                PlatformDebugBreak();
                abort();
            }

        }

        DebugStream stream{};
    };

    auto LastTickTime = std::chrono::steady_clock::now();
    auto LastSaveTime = std::chrono::steady_clock::now();

    EngineState* engineState{nullptr};
    //Invent::SaveState* saveState;
    //Platform* platform{nullptr};
    ServiceLocator* services{nullptr};

    std::unique_ptr<GameLog> gameLog {nullptr};
}

Engine::~Engine() { 
    m_Game.ShutDown(); 
    services->ResetAll();
    Graphics::Shutdown();
    gameLog.reset();
}

bool Engine::Initialize() {
    //Log::Level logLevel = Log::Level::Error;
    auto logLevel = Log::Level::Info;
#ifdef DEBUG
    logLevel = Log::Level::Info;
#endif
    gameLog = std::make_unique<GameLog>(Log::Filter{}.WithLevel(logLevel));
#ifdef DEBUG
    Log::Info("Debug Mode");
#endif

    services = &ServiceLocator::Get();
    services->Set<Platform>(m_Platform);
    engineState = &services->GetOrCreate<EngineState>();

    if(!m_Game.Initialize()) {
        Log::Error("Failed to initialize game");
        m_Game.ShutDown();
        return false;
    }

    services->GetOrCreate<PubSub<FileOperation>>().Subscribe(m_Handles, [&](const FileOperation& request) {
        switch(request) {
            using enum FileOperation;
            break; case Save: m_Game.SaveGame(); 
            break; case Load: m_Game.LoadGame(); 
            break; case Delete: m_Game.DeleteGame();
        }
    });
    return true;
}

void Engine::Tick() const {
    using namespace std::chrono_literals;
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<BaseTime>(now - LastTickTime);
    engineState->FrameNum++;
    engineState->ElapsedTime += elapsed;
    LastTickTime = now;

    m_Game.Tick(elapsed);
    if(now - LastSaveTime > 30s) {
        LastSaveTime = now;
        services->GetRequired<PubSub<FileOperation>>().Publish(FileOperation::Save);
    }
    // TODO: Sleep to set FPS
}
/*
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
    auto elapsed = std::chrono::duration_cast<BaseTime>(now - LastTickTime);
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
*/