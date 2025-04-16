#include "InventLib/InventGame.h"
#include "InventLib/Achievements/Achievements.h"
#include "InventLib/Character/Society.h"
#include "InventLib/GameState/GameSettings.h"
#include "InventLib/GameState/InventGameState.h"
#include "InventLib/GameState/SaveState.h"
#include "InventLib/Projects/Expeditions.h"
#include "InventLib/RandomEvents/RandomEvents.h"
#include "InventLib/Technology/Invention.h"
#include "InventLib/Technology/Technology.h"
#include "InventLib/Ui/Splash.h"
#include "InventLib/Ui/StartLife.h"
#include "InventLib/Ui/Ui.h"

#include <DesignPatterns/PubSub.h>
#include <DesignPatterns/ServiceLocator.h>
#include <Instrumentation/Logging.h>
#include <Platform/Graphics.h>
#include <Mechanics/Achievement.h>
#include <Mechanics/Advancement.h>
#include <Mechanics/Purchasable.h>
#include <Mechanics/RandomEvent.h>
#include <Mechanics/Unlockable.h>
#include <Resources/Resource.h>
#include <Resources/ResourceConverters.h>
#include <Resources/Storage.h>
#include <Utilities/IRandom.h>

#include <vector>
#include <unordered_map>
#include <string>

namespace {
    //Invent::SaveState saveState{};
    Invent::InventGameState* gameState{nullptr};
    Invent::Society* society{nullptr};
    Invent::GameSettings* gameSettings{nullptr};

}

namespace Invent {

    bool InventGame::Initialize() {
        // clean start
        DeleteGame();

        auto& services = ServiceLocator::Get();

        services.SetThisAsThat<DefaultRandom, IRandom>();
        services.SetThisAsThat<InventGameState, GameState>();
        gameState = static_cast<InventGameState*>(&services.GetRequired<GameState>());

        services.CreateIfMissing<PubSub<Achievement>>();
        services.CreateIfMissing<PubSub<Advancement>>();
        services.CreateIfMissing<PubSub<FileOperation>>();
        services.CreateIfMissing<PubSub<InventionLevel>>();
        services.CreateIfMissing<PubSub<RandomEvent>>();
        services.CreateIfMissing<PubSub<Storage>>();
        services.CreateIfMissing<PubSub<Unlockable>>();
        services.CreateIfMissing<PubSub<std::vector<Effect>>>();
        services.CreateIfMissing<PubSub<ExpeditionOutcome>>();
        services.CreateIfMissing<std::unordered_map<std::string, RandomEvent>>();
        services.CreateIfMissing<std::unordered_map<std::string, Unlockable>>();
        services.CreateIfMissing<std::unordered_map<std::string, Purchasable>>();

        society = &services.GetOrCreate<Society>();
        gameSettings = &services.GetOrCreate<GameSettings>();
        auto& inventionPs = services.GetRequired<PubSub<InventionLevel>>();

        Achievements::Initialize();
        RandomEvents::Initialize();

        inventionPs.Subscribe([](const InventionLevel& invention) {
            ServiceLocator::Get().GetRequired<PubSub<std::vector<Effect>>>().Publish(invention.Effects);
        });

        // TODO: The first loaded image is garbage
        // so we load a throw-away one
        Graphics::TryLoadImageFile("BugFix.png");

        DR_ASSERT_MSG(Ui::Splash::Initialize(), "Failed to initialize splash screen");
        Graphics::Render(Ui::Splash::Render);

        DR_ASSERT_MSG(Ui::StartLife::Initialize(), "Failed to initialize start life screen");
        DR_ASSERT_MSG(Ui::Initialize(), "Failed to initialize UI");

        return true;
        /*
        inventionPs.Alarm([](const InventionLevel& invention) {
            if(invention.Age > 0 || invention.Level > 1) return;
            auto& services = ServiceLocator::Get();
            auto& unlockables = services.GetRequired<std::unordered_map<std::string, Unlockable>>();
            auto& purchasables = services.GetRequired<std::unordered_map<std::string, Purchasable>>();

            for(const auto& tech: Technologies::StoneAgeTechs) {
                auto target = tech.Levels[0].Name;
                purchasables.erase(target);
                unlockables.erase(target);
            }

            switch(invention.Resource) {
            case ResourceName::Influence: RandomEvents::InfluenceEvents::RegisterAll(); break;
            case ResourceName::Wealth: RandomEvents::WealthEvents::RegisterAll(); break;
            case ResourceName::Knowledge: RandomEvents::KnowledgeEvents::RegisterAll(); break;
            case ResourceName::Labor: RandomEvents::LaborEvents::RegisterAll(); break;
            case ResourceName::Magic: RandomEvents::MagicEvents::RegisterAll(); break;
            default: break;
            }
        });
        */
    }

    void InventGame::ShutDown() {
        Ui::ShutDown();
        Ui::Splash::ShutDown();
        Ui::StartLife::ShutDown();
    }

    void InventGame::LoadGame() {
        Log::Info("Loading InventLib");
        /*
        gameState->Load(*saveState.GameStateSaveState);
        gameSettings->Load(*saveState.GameSettingsSaveState);
        Achievements::Load(*saveState.AchievementState);
        RandomEvents::Load(*saveState.RandomEventState);
        // Technologies::Load(*saveState.TechSaveState);
        */
    }

    void InventGame::SaveGame() {
        Log::Info("Saving InventLib");
        /*
        gameState->Save(*saveState.GameStateSaveState);
        gameSettings->Save(*saveState.GameSettingsSaveState);
        Achievements::Save(*saveState.AchievementState);
        RandomEvents::Save(*saveState.RandomEventState);
        // Technologies::Save(*saveState.TechSaveState);
        */
    }

    void InventGame::DeleteGame() {
        //auto path = Platform.GetRootPath() / "InventLib" / "Save.dat";
        //std::remove(path);
    }

    void InventGame::Tick(BaseTime elapsed) {
        Unlockables::Tick();
        Purchasables::Tick();
        //RandomEvents::Tick(elapsed);

        gameState->Tick(elapsed);
        society->Tick(elapsed);

        Ui::StartLife::IsDone() ? Graphics::Render(Ui::Render) : Graphics::Render(Ui::StartLife::Render);
    }
}
/*

void Reset() {
    Log::Info("Resetting InventLib");

    auto& services = ServiceLocator::Get();
    if(auto* unlockables = services.Get<std::unordered_map<std::string, Unlockable>>()) {
        unlockables->clear();
    }
    if(auto* purchasables = services.Get<std::unordered_map<std::string, Purchasable>>()) {
        purchasables->clear();
    }
    if(auto* randomEvents = services.Get<std::unordered_map<std::string, RandomEvent>>()) {
        randomEvents->clear();
    }

    // Inventions::Reset();
}

*/