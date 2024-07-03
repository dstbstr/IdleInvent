#include "InventLib/EntryPoint.h"

#include "InventLib/Achievements/Achievements.h"
#include "InventLib/GameState/GameState.h"
#include "InventLib/GameState/GameTime.h"
#include "InventLib/GameState/SaveState.h"
#include "InventLib/Mechanics/Achievement.h"
#include "InventLib/Mechanics/RandomEvent.h"
#include "InventLib/Mechanics/Unlockable.h"
#include "InventLib/RandomEvents/RandomEvents.h"
#include "InventLib/Resources/Resource.h"
#include "InventLib/Technology/Invention.h"
#include "InventLib/Technology/Technology.h"
#include "InventLib/Settings/GameSettings.h"

#include "Core/DesignPatterns/PubSub.h"
#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/Instrumentation/Logging.h"
#include "Core/Utilities/IRandom.h"

#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

namespace {
    Invent::GameState* gameState{nullptr};
    Invent::Life* life{nullptr};
    Invent::GameSettings* gameSettings{nullptr};
}

namespace Invent::EntryPoint {
    void Initialize() {
        Log::Info("Initializing InventLib");

        auto& services = ServiceLocator::Get();

        services.SetThisAsThat<DefaultRandom, IRandom>();

        services.CreateIfMissing<PubSub<Achievement>>();
        services.CreateIfMissing<PubSub<Advancement>>();
        //services.CreateIfMissing<PubSub<Death>>();
        //services.CreateIfMissing<PubSub<NearDeath>>();
        services.CreateIfMissing<PubSub<FileOperation>>();
        services.CreateIfMissing<PubSub<InventionLevel>>();
        services.CreateIfMissing<PubSub<RandomEvent>>();
        services.CreateIfMissing<PubSub<Storage>>();
        services.CreateIfMissing<PubSub<Unlockable>>();
        services.CreateIfMissing<PubSub<std::vector<Effect>>>();
        services.CreateIfMissing<std::unordered_map<std::string, RandomEvent>>();
        services.CreateIfMissing<std::unordered_map<std::string, Unlockable>>();
        services.CreateIfMissing<std::unordered_map<std::string, Purchasable>>();

        life = &services.GetOrCreate<Life>();
        life->Start(ResourceName::Money);
        gameState = &services.GetOrCreate<GameState>();
        gameSettings = &services.GetOrCreate<GameSettings>();
        auto& inventionPs = services.GetRequired<PubSub<InventionLevel>>();

        //Technologies::Initialize();
        Achievements::Initialize();
        RandomEvents::Initialize();
        
        inventionPs.Subscribe([](const InventionLevel& invention) { 
            ServiceLocator::Get().GetRequired<PubSub<std::vector<Effect>>>().Publish(invention.Effects); 
        });

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

    void ShutDown() { 
        ServiceLocator::Get().ResetAll();
    }

    void Load(const SaveState& saveState) {
        Log::Info("Loading InventLib");
		gameState->Load(saveState.GameStateSaveState);
        gameSettings->Load(saveState.GameSettingsSaveState);
		Achievements::Load(saveState.AchievementState);
		RandomEvents::Load(saveState.RandomEventState);
		//Technologies::Load(saveState.TechSaveState);
	}

    void Save(SaveState& saveState) {
        Log::Info("Saving InventLib");
		gameState->Save(saveState.GameStateSaveState);
        gameSettings->Save(saveState.GameSettingsSaveState);
		Achievements::Save(saveState.AchievementState);
		RandomEvents::Save(saveState.RandomEventState);
		//Technologies::Save(saveState.TechSaveState);
	}

    void Reset() {
        Log::Info("Resetting InventLib");

        auto& services = ServiceLocator::Get();
        if(auto* unlockables = services.Get<std::unordered_map<std::string, Unlockable>>()) {
			unlockables->clear();
		}
        if(auto* purchasables = services.Get<std::unordered_map<std::string, Purchasable>>()) {
            purchasables->clear();
        }
        if (auto* randomEvents = services.Get<std::unordered_map<std::string, RandomEvent>>()) {
            randomEvents->clear();
        }

        Inventions::Reset();
    }

    void Tick(BaseTime elapsed) {
        Unlockables::Tick();
        Purchasables::Tick();
        RandomEvents::Tick(elapsed);

        gameState->Tick(elapsed);
        life->Tick(elapsed);
    }
} // namespace Invent::EntryPoint