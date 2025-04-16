#include <Platform/PlatformMain.h>
#include <Engine.h>
#include <InventLib/InventGame.h>

#include <memory>

namespace {
    std::unique_ptr<Engine> engine{nullptr};
    std::unique_ptr<Invent::InventGame> game{nullptr};
    Platform* ThePlatform{nullptr};
}

bool Initialize(Platform& platform) {
    ThePlatform = &platform;
    game = std::make_unique<Invent::InventGame>(platform);
    engine = std::make_unique<Engine>(platform, *game);

    return engine->Initialize();
}

void Run() {
    while(true) {
        if(!ThePlatform->HandleInput()) break;
        engine->Tick();
    }
    engine.reset();
}