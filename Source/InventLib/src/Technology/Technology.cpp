#include "InventLib/Technology/Technology.h"

#include <GameState/GameState.h>
#include <Mechanics/Purchasable.h>
#include <Mechanics/Unlockable.h>
#include <Resources/Resource.h>
#include <Instrumentation/Logging.h>
#include <DesignPatterns/PubSub.h>
#include <DesignPatterns/ServiceLocator.h>
#include <Constexpr/ConstexprStrUtils.h>

#include <ranges>
/*
namespace {
    using namespace Invent;

    Technologies::SaveState saveCopy;

    auto AddUnlockables = [](const std::vector<Invention>& techs) {
        auto& unlockables = ServiceLocator::Get().GetRequired<std::unordered_map<std::string, Unlockable>>();
        auto unlocks = techs | std::views::transform([](const Invention& tech) { return tech.MakeUnlockable(0); });

        for(const auto& unlock: unlocks) {
            unlockables.emplace(unlock.Name, unlock);
        }
    };

    void LoadSavedGame(const Technologies::SaveState& saveState) {
        Log::Info("Loading Technology Save");

        auto& unlockables = ServiceLocator::Get().GetRequired<std::unordered_map<std::string, Unlockable>>();
        auto specialty = static_cast<ResourceName>(saveState.Specialty);
        if(saveState.Specialty == 0) {
            Log::Debug("No specialty set, adding all unlockables");
            AddUnlockables(Technologies::StoneAgeTechs);
            AddUnlockables(Technologies::StorageTechs);

            ServiceLocator::Get().GetRequired<PubSub<InventionLevel>>().Alarm([=](const InventionLevel& level) {
                switch(level.Resource) {
                case ResourceName::Influence: AddUnlockables(Technologies::InfluenceTechs); break;
                case ResourceName::Knowledge: AddUnlockables(Technologies::KnowledgeTechs); break;
                case ResourceName::Labor: AddUnlockables(Technologies::LaborTechs); break;
                case ResourceName::Magic: AddUnlockables(Technologies::MagicTechs); break;
                case ResourceName::Wealth: AddUnlockables(Technologies::WealthTechs); break;
                default: break;
                }
            });

            return;
        }

        for(auto i = 0u, storageIndex = 0u; i < AllResources.size(); i++, storageIndex++) {
            auto& storageTech = Technologies::StorageTechs[i];
            if(storageTech.Resource == specialty) {
                storageIndex--;
                continue;
            }

            auto level = saveState.OtherStorageLevels & (1 << storageIndex);
            if(level == 0) {
                Log::Debug(std::format("Adding Non-specialty Storage Unlockable {}", ToString(storageTech.Resource)));
                auto unlockableStorage = storageTech.MakeUnlockable(0);
                unlockables.emplace(unlockableStorage.Name, unlockableStorage);
            }
        }
        if(saveState.SpecialtyStorageLevel < 7) {
            Log::Debug(std::format("Storage level for {} is {}", ToString(specialty), saveState.SpecialtyStorageLevel));
            auto unlockableStorage =
                Technologies::StorageTechs[saveState.Specialty - 1].MakeUnlockable(saveState.SpecialtyStorageLevel);
            unlockables.emplace(unlockableStorage.Name, unlockableStorage);
        }

        if(saveState.StoneAgeLevel < 3) {
            Log::Debug(std::format("Stone Age level is {}", saveState.StoneAgeLevel));
            auto unlockableStoneAge =
                Technologies::StoneAgeTechs[saveState.Specialty - 1].MakeUnlockable(saveState.StoneAgeLevel);
            unlockables.emplace(unlockableStoneAge.Name, unlockableStoneAge);
        } else {
            Log::Info("Stone Age is complete");
        }

        const auto& techs = [&]() -> const std::vector<Invention>& {
            switch(specialty) {
            case ResourceName::Influence: return Technologies::InfluenceTechs;
            case ResourceName::Knowledge: return Technologies::KnowledgeTechs;
            case ResourceName::Labor: return Technologies::LaborTechs;
            case ResourceName::Magic: return Technologies::MagicTechs;
            case ResourceName::Wealth: return Technologies::WealthTechs;
            default: DR_ASSERT_MSG(false, "Unexpected speciality"); return Technologies::InfluenceTechs;
            }
        }();

        std::array<u8, 18> levels{};
        for(size_t i = 0; i < 2; ++i) {
            levels[i] = (saveState.ResourceLevels1and2 >> (i * 2)) & 0b11;
        }
        for(size_t i = 0; i < 16; ++i) {
            levels[i + 2] = (saveState.ResourcesLevels3to18 >> (i * 2)) & 0b11;
        }

        auto levelStrs = Constexpr::Join(" ", levels);
        Log::Debug(std::format("Resource levels: {}", levelStrs));
        
        for(auto i = 0u; i < techs.size(); i++) {
            if(levels[i] == 3) continue;
            auto unlockable = techs[i].MakeUnlockable(levels[i]);
            unlockables.emplace(unlockable.Name, unlockable);
        }
    }

    void UpdateSave(const InventionLevel& invention) {
        Log::Info("Updating technology save state");

        if(saveCopy.Specialty == 0) {
            saveCopy.Specialty = static_cast<u8>(invention.Resource);
        }
        auto specialty = static_cast<ResourceName>(saveCopy.Specialty);

        auto isStorage = invention.Name.find("Storage") != std::string::npos;
        if(isStorage) {
            if(specialty == invention.Resource) {
                saveCopy.SpecialtyStorageLevel = static_cast<u8>(invention.Level + 1);
            } else {
                size_t bitIndex = 0;
                for(auto resource: AllResources) {
                    if(resource == specialty) continue;
                    if(resource == invention.Resource) {
                        saveCopy.OtherStorageLevels |= 1 << bitIndex;
                        break;
                    }
                    bitIndex++;
                }
            }
        } else {
            auto level = invention.Level + 1;
            auto shift = invention.Age < 3 ? (invention.Age - 1) : (invention.Age - 3);
            shift *= 2;
            auto mask = ~(0b11 << shift);
            if(invention.Age == 0) {
                saveCopy.StoneAgeLevel = level;
            } else if (invention.Age < 3) {
                saveCopy.ResourceLevels1and2 &= mask;
                saveCopy.ResourceLevels1and2 |= level << shift;
            } else {
                saveCopy.ResourcesLevels3to18 &= mask;
                saveCopy.ResourcesLevels3to18 |= level << shift;
            }
        }
    }
} // namespace

namespace Invent::Technologies {
    void Initialize() {
        Log::Info("Initializing Technologies");
        auto& services = ServiceLocator::Get();
        services.CreateIfMissing<std::unordered_map<std::string, Unlockable>>();
        services.CreateIfMissing<std::unordered_map<std::string, Purchasable>>();
        auto& inventionPs = services.GetOrCreate<PubSub<InventionLevel>>();

        inventionPs.Subscribe(UpdateSave);
    }

    void Load(const Technologies::SaveState& saveState) {
        saveCopy = saveState;
        LoadSavedGame(saveState);
    }

    void Save(Technologies::SaveState& saveState) { saveState = saveCopy; }
} // namespace Invent::Technologies

*/