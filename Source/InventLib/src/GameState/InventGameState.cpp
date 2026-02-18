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
        services.GetOrCreate<PubSub<InventionLevel>>().Subscribe(m_PsHandles, [](const InventionLevel& invention) {
            if(invention.Name.find("Death") == std::string::npos) return;
            BaseLifeSpan += invention.Level * 5;
        });

        services.GetOrCreate<PubSub<TechAge>>().Subscribe(m_PsHandles, [](const TechAge& age) {
            Log::Info(std::format("Age advanced to {}", age.Name));
            BaseLifeSpan += 5;
        });
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