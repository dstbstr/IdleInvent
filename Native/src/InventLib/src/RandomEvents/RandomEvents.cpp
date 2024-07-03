#include "InventLib/RandomEvents/RandomEvents.h"

#include "InventLib/GameState/GameTime.h"
#include "InventLib/Mechanics/RandomEvent.h"
#include "InventLib/Technology/Invention.h"

#include "Core/DesignPatterns/PubSub.h"
#include "Core/DesignPatterns/ServiceLocator.h"

namespace {
    using namespace Invent::RandomEvents;

    u8 Specialty{0};

    Invent::BaseTime MoreCommon(Invent::BaseTime chance) {
        return chance == Rarity::Common      ? Rarity::Regular
               : chance == Rarity::Uncommon  ? Rarity::Common
               : chance == Rarity::Rare      ? Rarity::Uncommon
               : chance == Rarity::VeryRare  ? Rarity::Rare
               : chance == Rarity::Legendary ? Rarity::VeryRare
               : chance == Rarity::Mythic    ? Rarity::Legendary
                                             : chance / 2;
    }

    Invent::BaseTime LessCommon(Invent::BaseTime chance) {
        return chance == Rarity::Regular     ? Rarity::Common
               : chance == Rarity::Common    ? Rarity::Uncommon
               : chance == Rarity::Uncommon  ? Rarity::Rare
               : chance == Rarity::Rare      ? Rarity::VeryRare
               : chance == Rarity::VeryRare  ? Rarity::Legendary
               : chance == Rarity::Legendary ? Rarity::Mythic
                                             : chance * 2;
    }

} // namespace

namespace Invent {
    namespace RandomEvents {
        void Initialize() {
            Log::Info("Initializing Random Events");
            ServiceLocator::Get().GetOrCreate<PubSub<InventionLevel>>().Alarm([](const InventionLevel& invention) {
                Specialty = static_cast<u8>(invention.Resource);
            });
        }

        void Load(const SaveState& save) {
            /*
            Log::Info("Loading Random Events");
			Specialty = save.Specialty;
            NaturalEvents::RegisterAll();
            switch(static_cast<ResourceName>(save.Specialty)) {
            case ResourceName::Influence: InfluenceEvents::RegisterAll(); break;
            case ResourceName::Knowledge: KnowledgeEvents::RegisterAll(); break;
            case ResourceName::Labor: LaborEvents::RegisterAll(); break;
            case ResourceName::Magic: MagicEvents::RegisterAll(); break;
            case ResourceName::Wealth: WealthEvents::RegisterAll(); break;
            case ResourceName::Unset: break;
            }

            */
            Specialty = save.Specialty;
        }

        void Save(SaveState& save) {
            Log::Info("Saving Random Events");
            save.Specialty = Specialty;
		}

        void MakeMoreCommon(RandomEvent event) {
            if(!IsEventRegistered(event)) return;
            auto rarity = GetEventChance(event);
            UnregisterEvent(event);
            RegisterEvent(event, MoreCommon(rarity));
        }

        void MakeLessCommon(RandomEvent event) {
            if(!IsEventRegistered(event)) return;
            auto rarity = GetEventChance(event);
            UnregisterEvent(event);
            RegisterEvent(event, LessCommon(rarity));
        }
    } // namespace RandomEvents
} // namespace Invent