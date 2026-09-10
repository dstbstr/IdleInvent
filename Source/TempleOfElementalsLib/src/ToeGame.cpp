#include "TOE/ToeGame.h"
#include "TOE/Hero/Hero.h"
//#include "TOE/Ui/Ui.h"

#include "DesignPatterns/ServiceLocator.h"
#include "Manage/TickManager.h"
#include "Platform/Graphics.h"
#include "Animation/Animation.h"

namespace TOE {
    bool ToeGame::Initialize() {
        auto& services = ServiceLocator::Get();
        services.CreateIfMissing<TickManager>();
        services.CreateIfMissing<std::unordered_map<std::string, Animation>>();
        auto& hero = services.GetOrCreate<Hero>();

        return true; //Ui::Initialize();
    }

    void ToeGame::ShutDown() {
        // Ui::ShutDown();
    }

    void ToeGame::LoadGame() {
    }

    void ToeGame::SaveGame() {
    }

    void ToeGame::DeleteGame() {
    }

    void ToeGame::Tick(BaseTime elapsed) {
        TickManager::Get().Tick(elapsed);
        // Graphics::Render(Ui::Render);
    }
} // namespace Toe
