#include "Iduel/IduelGame.h"
#include "Iduel/Ui/IduelLayout.h"

#include "DesignPatterns/ServiceLocator.h"
#include "Manage/TickManager.h"
#include "Platform/Graphics.h"

#include <DesignPatterns/PubSub.h>
#include <Utilities/IRandom.h>

#include <algorithm>

namespace Iduel {
    bool IduelGame::Initialize() {
        auto& services = ServiceLocator::Get();
        services.CreateIfMissing<TickManager>();
        services.SetThisAsThat<DefaultRandom, IRandom>();

        return IduelUi::Layout::Initialize();
    }

    void IduelGame::ShutDown() {
        IduelUi::Layout::ShutDown();
    }

    void IduelGame::LoadGame() {}

    void IduelGame::SaveGame() {}

    void IduelGame::DeleteGame() {}

    void IduelGame::Tick(BaseTime elapsed) {
        IduelUi::Layout::Tick(elapsed);
        Graphics::Render(IduelUi::Layout::Render);
    }
}
