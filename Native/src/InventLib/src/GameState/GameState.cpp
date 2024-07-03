#include "InventLib/GameState/GameState.h"

#include "InventLib/Character/Death.h"
#include "InventLib/Mechanics/Achievement.h"
#include "InventLib/RandomEvents/RandomEvents.h"
#include "InventLib/Technology/Technology.h"
#include "InventLib/Technology/TechAge.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"


namespace {
	using namespace Invent;

	void ApplyEffect(GameState&, const Effect& effect) {
		if (effect.OtherEffects) effect.OtherEffects();
        if(effect.Resource == ResourceName::Unset) return;

        // TODO: Move this to Life
		//auto mod = Modifier{.Add = effect.Add, .Mul = effect.Mul, .Duration = effect.Duration};
		//auto& resource = gameState.Character.CurrentResources.at(effect.Resource);
        //resource.Progress.AddModifier(mod);
	}

	size_t BaseLifeSpan = 25;

	auto DecayTimer = BaseTime::zero();
	auto DecayFrequency = OneGameYear;
	auto DecayAmount = 0.9;

	void TickDecay(BaseTime elapsed, StorageCollection& storages) {
        DecayTimer += elapsed;
        if(DecayTimer < DecayFrequency) return;
        DecayTimer -= DecayFrequency;

        for(auto& [name, storage]: storages) {
            storage.Stored = static_cast<size_t>(storage.Stored * DecayAmount);
        }
	}
}

namespace Invent {
	GameState::GameState() : CurrentLife() {
        SetupSubscriptions();
	}


	void GameState::Load(const GameStateSave& save) {
        //Character.Load(save.CharacterSave);
		TimeElapsed = save.ElapsedSeconds * OneSecond;
		CurrentRunElapsed = save.RunElapsedSeconds * OneSecond;

		Storages = {};
        Storages.Load(save.StorageSave);
	}

	GameState::~GameState() {
        auto& services = ServiceLocator::Get();
        if(auto* ps = services.Get<PubSub<std::vector<Effect>>>()) {
			ps->Unsubscribe(m_EffectHandle);
		}
        if(auto* ps = services.Get<PubSub<Storage>>()) {
            ps->Unsubscribe(m_StorageHandle);
		}
        if(auto* ps = services.Get<PubSub<TechAge>>()) {
            ps->Unsubscribe(m_AgeHandle);
        }
		if(auto* ps = services.Get<PubSub<Death>>()) {
			ps->Unsubscribe(m_DeathHandle);
		}
    }

    void GameState::Save(GameStateSave& save) const {
		save.ElapsedSeconds = static_cast<u32>(TimeElapsed / OneSecond);
        save.RunElapsedSeconds = static_cast<u32>(CurrentRunElapsed / OneSecond);
        Storages.Save(save.StorageSave);
        //Character.Save(save.CharacterSave);
    }

	void GameState::SetupSubscriptions() {
        auto& services = ServiceLocator::Get();

        m_EffectHandle =
            services.GetOrCreate<PubSub<std::vector<Effect>>>().Subscribe([this](const std::vector<Effect>& effects) {
                for(const auto& effect: effects) {
                    ApplyEffect(*this, effect);
                }
            });

        auto& storegeEvents = services.GetOrCreate<PubSub<InventionLevel>>();
        m_StorageHandle = storegeEvents.Subscribe([this](const InventionLevel& invention) {
            if(invention.Name.find("Storage") == std::string::npos) return;
            Storages.at(invention.Resource).UpgradeCapacity(invention.Level);
        });

        auto& ageEvents = services.GetOrCreate<PubSub<TechAge>>();
        m_AgeHandle = ageEvents.Subscribe([](const TechAge& age) {
            Log::Info(std::format("Age advanced to {}", age.Name));
            BaseLifeSpan += 5;
        });

        auto& deathEvents = services.GetOrCreate<PubSub<Death>>();
        m_DeathHandle = deathEvents.Subscribe([this](const Death&) {
            for(const auto& name: AllResources) {
                auto& resource = CurrentLife.Resources.at(name);
                resource.Current = Storages.at(name).Stored;
                Storages.at(name).Stored = 0;
            }
            //Character.CharacterDeath = GenerateDeath(BaseLifeSpan);
            //Character.CurrentAge = 16 * OneGameYear;
        });
	}

	void GameState::Tick(BaseTime elapsed) {
        TimeElapsed += elapsed;
		CurrentRunElapsed += elapsed;
		
        CurrentLife.Tick(elapsed);
        TickDecay(elapsed, Storages);
	}
}