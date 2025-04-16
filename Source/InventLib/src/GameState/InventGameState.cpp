#include "InventLib/GameState/InventGameState.h"

#include "InventLib/Character/Death.h"
#include "InventLib/RandomEvents/RandomEvents.h"
#include "InventLib/Technology/TechAge.h"
#include "InventLib/Technology/Technology.h"

#include <DesignPatterns/PubSub.h>
#include <DesignPatterns/ServiceLocator.h>

namespace {
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
    InventGameState::InventGameState() { 
        SetupSubscriptions(); 
    }

    void InventGameState::SetupSubscriptions() {
        auto& services = ServiceLocator::Get();

        /*
        m_EffectHandle =
            services.GetOrCreate<PubSub<std::vector<Effect>>>().Subscribe([this](const std::vector<Effect>& effects) {
                for(const auto& effect: effects) {
                    ApplyEffect(*this, effect);
                }
            });
            */

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
    }

    InventGameState::~InventGameState() {
        auto& services = ServiceLocator::Get();
        if(auto* ps = services.Get<PubSub<Storage>>()) {
            ps->Unsubscribe(m_StorageHandle);
        }
        if(auto* ps = services.Get<PubSub<TechAge>>()) {
            ps->Unsubscribe(m_AgeHandle);
        }

        // if(auto* ps = services.Get<PubSub<std::vector<Effect>>>()) {
        //	ps->Unsubscribe(m_EffectHandle);
        // }
        // if(auto* ps = services.Get<PubSub<Death>>()) {
        //	ps->Unsubscribe(m_DeathHandle);
        // }
    }
    void InventGameState::Save(InventGameStateSave& save) const {
        GameState::Save(save);
        //Character.Save(save.CharacterSave);
        Storages.Save(save.StorageSave);

    }
    void InventGameState::Load(const InventGameStateSave& save) {
        GameState::Load(save);
        //Character.Load(save.CharacterSave);
        Storages.Load(save.StorageSave);
    }

    void InventGameState::Tick(BaseTime elapsed) {
        GameState::Tick(elapsed);
        // CurrentLife.Tick(elapsed);
        TickDecay(elapsed, Storages);
    }
} // namespace Invent