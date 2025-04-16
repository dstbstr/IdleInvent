#include "CommonTest.h"
#include "InventLib/Technology/Technology.h"
#include "InventLib/Technology/Invention.h"
#include "TestGameState.h"

#include <Constexpr/ConstexprStrUtils.h>
#include <DesignPatterns/PubSub.h>
#include <DesignPatterns/ServiceLocator.h>
#include <GameState/GameState.h>
#include <Utilities/IRandom.h>

#include <unordered_set>
#include <vector>


/*
namespace Invent {
    struct TechnologyListener {
        TechnologyListener() {
            handle = ServiceLocator::Get().GetRequired<PubSub<InventionLevel>>().Subscribe(
                [this](const InventionLevel& invention) { ReceivedInventions.push_back(invention); }
            );
        }
        ~TechnologyListener() { ServiceLocator::Get().GetRequired<PubSub<InventionLevel>>().Unsubscribe(handle); }

        std::vector<InventionLevel> ReceivedInventions;
        size_t handle{0};
    };

    struct TechnologyTest : public ::testing::Test {
        void SetUp() override {
            auto& services = ServiceLocator::Get();
            GenerateTestGameState();
            purchasables = &services.GetOrCreate<std::unordered_map<std::string, Purchasable>>();
            unlockables = &services.GetOrCreate<std::unordered_map<std::string, Unlockable>>();
            services.CreateIfMissing<PubSub<InventionLevel>>();

            for(const auto& tech: Technologies::StoneAgeTechs) {
                auto unlockable = tech.MakeUnlockable(0);
                unlockables->emplace(unlockable.Name, unlockable);
            }

            gameState = &services.GetOrCreate<GameState>();
        }

        void TearDown() override { ServiceLocator::Get().ResetAll(); }

        void AddResources() {
            for(auto& [name, resource]: gameState->Character.CurrentResources) {
                resource.Current = 1'000;
            }
        }
        std::unordered_map<std::string, Unlockable>* unlockables;
        std::unordered_map<std::string, Purchasable>* purchasables;
        GameState* gameState{nullptr};
    };

    TEST_F(TechnologyTest, Unlockables_AfterInitialization_ContainsStoneAgeTechs) {
        ASSERT_EQ(unlockables->size(), Technologies::StoneAgeTechs.size());
    }

    TEST_F(TechnologyTest, Purchasables_AfterInitialization_ContainsNoElements) { ASSERT_TRUE(purchasables->empty()); }

    TEST_F(TechnologyTest, Unlockables_AfterUnlockingAnInvention_StillContainsOtherUnlockables) {
        AddResources();

        Unlockables::Tick();

        ASSERT_EQ(unlockables->size(), Technologies::StoneAgeTechs.size()); // Removed Tribes 1, added Tribes 2
    }

    TEST_F(TechnologyTest, Unlockables_AfterPurchasingStoneAgeTech_RemovesOtherStoneAgeTechs) {
        AddResources();

        std::vector<std::string> unlockableNames =
            *unlockables | std::views::keys | std::ranges::to<std::vector<std::string>>();
        Unlockables::Tick();

        Purchasables::TryPurchase(purchasables->begin()->first);
        Purchasables::Tick();

        ASSERT_EQ(1ull, unlockables->size());
        ASSERT_TRUE(purchasables->empty());
    }

    TEST_F(TechnologyTest, Unlockables_AfterUnlockingLevel2Tech_AddsLevel3Tech) {
        AddResources();
        Unlockables::Tick();
        Purchasables::TryPurchase("Tribes I");
        Purchasables::Tick();

        AddResources();
        Unlockables::Tick();
        ASSERT_TRUE(purchasables->contains("Tribes II"));
        ASSERT_TRUE(unlockables->contains("Tribes III"));
    }

    TEST_F(TechnologyTest, Purchasables_AfterPurchasingLevel2Tech_AddsLevel3Tech) {
        AddResources();
        Unlockables::Tick();
        Purchasables::TryPurchase("Tribes I");
        Purchasables::Tick();

        AddResources();
        Unlockables::Tick();
        Purchasables::TryPurchase("Tribes II");
        Purchasables::Tick();

        Unlockables::Tick();

        ASSERT_TRUE(purchasables->contains("Tribes III"));
    }

    TEST_F(TechnologyTest, Purchasables_AfterPurchase_DoesNotHaveMultplePurchasablesAvailable) {
        AddResources();
        Unlockables::Tick();
        Purchasables::TryPurchase("Tribes I");
        Purchasables::Tick();

        std::unordered_set<std::string> purchasableNames;
        for(const auto& [name, purchasable]: *purchasables) {
            auto split = Constexpr::Split(name, " ");
            ASSERT_FALSE(purchasableNames.contains(std::string(split[0])));
            purchasableNames.insert(std::string(split[0]));
        }
    }

    struct TechLoadTest : public ::testing::Test {
        void SetUp() override {
            auto& services = ServiceLocator::Get();
            unlockables = &services.GetOrCreate<std::unordered_map<std::string, Unlockable>>();
            Technologies::Initialize();
        }
        void TearDown() override { ServiceLocator::Get().ResetAll(); }

        std::unordered_map<std::string, Unlockable>* unlockables;
        Technologies::SaveState save{};
    };

    TEST_F(TechLoadTest, Unlockables_WithNewGame_IncludesStoneAgeTechs) {
        Technologies::Load(save);
        for(const auto& tech: Technologies::StoneAgeTechs) {
            ASSERT_TRUE(unlockables->contains(tech.Levels[0].Name));
        }
    }

    TEST_F(TechLoadTest, Unlockables_WithNewGame_IncludesStorageTechs) {
        Technologies::Load(save);
        for(const auto& tech: Technologies::StorageTechs) {
            ASSERT_TRUE(unlockables->contains(tech.Levels[0].Name));
        }
    }

    TEST_F(TechLoadTest, Unlockables_AfterPurchasingFirstStoneAgeTech_ContainsSecondLevelOfStoneAgeTech) {
        save.Specialty = static_cast<u8>(ResourceName::Influence);
        save.StoneAgeLevel = 1;

        Technologies::Load(save);
        ASSERT_TRUE(unlockables->contains("Tribes II"));
    }

    TEST_F(TechLoadTest, Unlockables_AfterPurchasingLastStoneAgeTech_ContainsNoStoneAgeTech) {
        save.Specialty = static_cast<u8>(ResourceName::Influence);
        save.StoneAgeLevel = 3;

        Technologies::Load(save);
        for(const auto& tech: Technologies::StoneAgeTechs) {
            for (const auto& [name, u] : *unlockables) {
                ASSERT_THAT(name, Not(StartsWith(tech.Name)));
            }
        }
    }

    TEST_F(TechLoadTest, Unlockables_WithoutBuyingStorage_ContainsAllStorages) {
        save.Specialty = static_cast<u8>(ResourceName::Influence);

        Technologies::Load(save);
        for(const auto& tech: Technologies::StorageTechs) {
            ASSERT_TRUE(unlockables->contains(tech.Levels[0].Name));
        }
    }

    TEST_F(TechLoadTest, Unlockables_AfterBuyingFirstInfluenceTech_ContainsSecondLevel) {
        save.Specialty = static_cast<u8>(ResourceName::Influence);
        save.ResourceLevels1and2 = 0b0001;

        Technologies::Load(save);
        ASSERT_TRUE(unlockables->contains(Technologies::InfluenceTechs[0].Levels[1].Name));
    }

    TEST_F(TechLoadTest, Unlockables_AfterBuyingSecondInfluenceTech_ContainsThirdLevel) {
        save.Specialty = static_cast<u8>(ResourceName::Influence);
        save.ResourceLevels1and2 = 0b0010;

        Technologies::Load(save);

        ASSERT_TRUE(unlockables->contains(Technologies::InfluenceTechs[0].Levels[2].Name));
    }

    TEST_F(TechLoadTest, Unlockables_AfterBuyingThirdInfluenceTechLevel_DoesNotContainFirstTech) {
        save.Specialty = static_cast<u8>(ResourceName::Influence);
        save.ResourceLevels1and2 = 0b0011;

        Technologies::Load(save);

        for(const auto& [name, unlockable]: *unlockables) {
            ASSERT_THAT(name, Not(HasSubstr(Technologies::InfluenceTechs[0].Name)));
        }
    }

    TEST_F(TechLoadTest, Unlockables_BuyingFirstOfEveryTech_IncludesSecondLevelOfEveryTech) {
        save.Specialty = static_cast<u8>(ResourceName::Influence);
        save.ResourceLevels1and2 = 0b0101;
        save.ResourcesLevels3to18 = 0b01010101010101010101010101010101;

        Technologies::Load(save);

        for(const auto& tech: Technologies::InfluenceTechs) {
            ASSERT_TRUE(unlockables->contains(tech.Levels[1].Name));
        }
    }

    TEST_F(TechLoadTest, Unlockables_AfterBuyingFirstStorageLevel_ContainsSecondStorageLevel) {
        save.Specialty = static_cast<u8>(ResourceName::Influence);
		save.SpecialtyStorageLevel = 1;

		Technologies::Load(save);

		ASSERT_TRUE(unlockables->contains(Technologies::StorageTechs[save.Specialty - 1].Levels[1].Name));
    }

    TEST_F(TechLoadTest, Unlockables_AfterBuyingSecondStorageLevel_ContainsThirdStorageLevel) {
        save.Specialty = static_cast<u8>(ResourceName::Influence);
        save.SpecialtyStorageLevel = 2;

        Technologies::Load(save);

        ASSERT_TRUE(unlockables->contains(Technologies::StorageTechs[save.Specialty - 1].Levels[2].Name));
    }

    TEST_F(TechLoadTest, Unlockables_AfterBuyingLastStorageLevel_ContainsNoStorage) {
        save.Specialty = static_cast<u8>(ResourceName::Influence);
        save.SpecialtyStorageLevel = 7;

        Technologies::Load(save);

        for(const auto& tech: Technologies::StorageTechs[save.Specialty - 1].Levels) {
            ASSERT_FALSE(unlockables->contains(tech.Name));
        }
    }

    TEST_F(TechLoadTest, Unlockables_AfterBuyingFirstInfluenceStorage_ContainsOtherStorageTechs) {
        save.Specialty = static_cast<u8>(ResourceName::Influence);
        save.SpecialtyStorageLevel = 1;

        Technologies::Load(save);

        for(const auto& tech: Technologies::StorageTechs) {
            if(tech.Resource == ResourceName::Influence) continue;
            ASSERT_TRUE(unlockables->contains(tech.Levels[0].Name));
        }
    }

    TEST_F(TechLoadTest, Unlockables_AfterBuyingAllFirstStorageTechs_ContainsNoOtherStorages) {
        save.Specialty = static_cast<u8>(ResourceName::Influence);
        save.OtherStorageLevels = 0b1111;

        Technologies::Load(save);

        for (const auto& tech : Technologies::StorageTechs) {
            if(tech.Resource == ResourceName::Influence) continue;
            for(const auto& level: tech.Levels) {
                ASSERT_FALSE(unlockables->contains(level.Name));
            }
        }
    }

    TEST_F(TechLoadTest, OtherStorages_AreStored_RightToLeft) {
        save.Specialty = static_cast<u8>(ResourceName::Influence);
		save.OtherStorageLevels = 0b1101;

		Technologies::Load(save);

        // StorageTech[0] is Influence
		ASSERT_FALSE(unlockables->contains(Technologies::StorageTechs[1].Levels[0].Name));
		ASSERT_TRUE(unlockables->contains(Technologies::StorageTechs[2].Levels[0].Name));
		ASSERT_FALSE(unlockables->contains(Technologies::StorageTechs[3].Levels[0].Name));
		ASSERT_FALSE(unlockables->contains(Technologies::StorageTechs[4].Levels[0].Name));
    }

    TEST_F(TechLoadTest, ResourceTechs_AreStored_RightToLeft) {
        save.Specialty = static_cast<u8>(ResourceName::Influence);
        save.ResourceLevels1and2 = 0b01'00;
        save.ResourcesLevels3to18 = 0b01'00'11'10'01'00'11'10'01'00'11'10'01'00'11'10;

        Technologies::Load(save);

        size_t expectedLevel = 0;
        for(const auto& tech: Technologies::InfluenceTechs) {
            if(expectedLevel == 3) {
                for(const auto& level: tech.Levels) {
					ASSERT_FALSE(unlockables->contains(level.Name));
				}
            } else {
                ASSERT_TRUE(unlockables->contains(tech.Levels[expectedLevel].Name));
            }
            expectedLevel = (expectedLevel + 1) % 4;
		}
    }

    struct TechSaveTest : public ::testing::Test {
        void SetUp() override {
            auto& services = ServiceLocator::Get();
            unlockables = &services.GetOrCreate<std::unordered_map<std::string, Unlockable>>();
            inventionPs = &services.GetOrCreate<PubSub<InventionLevel>>();

            Technologies::Initialize();
            Technologies::Load(save);
        }

        void TearDown() override { 
            ServiceLocator::Get().ResetAll(); 
        }

        std::unordered_map<std::string, Unlockable>* unlockables;
        PubSub<InventionLevel>* inventionPs;
        Technologies::SaveState save{};
    };

    TEST_F(TechSaveTest, Save_WithDefaultSave_MatchesDefault) {
        Technologies::SaveState result{};
        Technologies::Save(result);

        ASSERT_EQ(save.StoneAgeLevel, result.StoneAgeLevel);
        ASSERT_EQ(save.Specialty, result.Specialty);
        ASSERT_EQ(save.SpecialtyStorageLevel, result.SpecialtyStorageLevel);
        ASSERT_EQ(save.OtherStorageLevels, result.OtherStorageLevels);
        ASSERT_EQ(save.ResourceLevels1and2, result.ResourceLevels1and2);
        ASSERT_EQ(save.ResourcesLevels3to18, result.ResourcesLevels3to18);
    }

    TEST_F(TechSaveTest, Save_AfterUnlockingFirstStoneAgeTech_SetsSpecialty) {
        auto inventionLevel = Technologies::StoneAgeTechs[0].Levels[0];
        inventionPs->Publish(inventionLevel);

        Technologies::SaveState result{};
        Technologies::Save(result);

        ASSERT_EQ(static_cast<u8>(inventionLevel.Resource), result.Specialty);
    }

    TEST_F(TechSaveTest, Save_AfterUnlockingFirstStoneAgeTech_SetsStoneAgeLevel) {
        auto inventionLevel = Technologies::StoneAgeTechs[0].Levels[0];
        inventionPs->Publish(inventionLevel);

        Technologies::SaveState result{};
        Technologies::Save(result);

        ASSERT_EQ(inventionLevel.Level + 1, result.StoneAgeLevel);
    }

    TEST_F(TechSaveTest, Save_AfterUnlockingLastStoneAgeTech_SetsStoneAgeLeve) {
        auto inventionLevel = Technologies::StoneAgeTechs[0].Levels[2];
        inventionPs->Publish(inventionLevel);

        Technologies::SaveState result{};
        Technologies::Save(result);

        ASSERT_EQ(inventionLevel.Level + 1, result.StoneAgeLevel);
    }

    TEST_F(TechSaveTest, Save_AfterUnlockingLastStoneAgeTech_SetsStoneAgeLevel) {
        auto inventionLevel = Technologies::StoneAgeTechs[0].Levels[2];
        inventionPs->Publish(inventionLevel);

        Technologies::SaveState result{};
        Technologies::Save(result);

        ASSERT_EQ(inventionLevel.Level + 1, result.StoneAgeLevel);
    }

    TEST_F(TechSaveTest, Save_AfterUnlockingFirstStorageTech_SetsSpecialtyStorageLevel) {
        auto inventionLevel = Technologies::StorageTechs[0].Levels[0];
        inventionPs->Publish(inventionLevel);

        Technologies::SaveState result{};
        Technologies::Save(result);

        ASSERT_EQ(inventionLevel.Level + 1, result.SpecialtyStorageLevel);
    }

    TEST_F(TechSaveTest, Save_AfterUnlockingOtherStorage_SetsOtherStorageLevel) {
        for(auto i = 0u; i < Technologies::StorageTechs.size(); i++) {
			auto inventionLevel = Technologies::StorageTechs[i].Levels[0];
			inventionPs->Publish(inventionLevel);
		}

        Technologies::SaveState result{};
        Technologies::Save(result);

        ASSERT_EQ(0b1111, result.OtherStorageLevels);
    }

    TEST_F(TechSaveTest, Save_AfterUnlockingOtherStorage_LoadsSameStorageLevels) {
        unlockables->clear();
        inventionPs->Publish(Technologies::StorageTechs[0].Levels[0]);
        inventionPs->Publish(Technologies::StorageTechs[2].Levels[0]);
        inventionPs->Publish(Technologies::StorageTechs[3].Levels[0]);
        inventionPs->Publish(Technologies::StorageTechs[4].Levels[0]);

        Technologies::SaveState result{};
        Technologies::Save(result);

        ASSERT_EQ(0b1110, result.OtherStorageLevels);

        Technologies::Load(result);

        ASSERT_TRUE(unlockables->contains(Technologies::StorageTechs[1].Levels[0].Name));
        ASSERT_FALSE(unlockables->contains(Technologies::StorageTechs[2].Levels[0].Name));
        ASSERT_FALSE(unlockables->contains(Technologies::StorageTechs[3].Levels[0].Name));
        ASSERT_FALSE(unlockables->contains(Technologies::StorageTechs[4].Levels[0].Name));
    }

    TEST_F(TechSaveTest, Save_WithNoneZeroSpecialty_SkipsBitCorrectly) {
        inventionPs->Publish(Technologies::StorageTechs[3].Levels[0]);
        inventionPs->Publish(Technologies::StorageTechs[2].Levels[0]);
        inventionPs->Publish(Technologies::StorageTechs[4].Levels[0]);

        Technologies::SaveState result{};
        Technologies::Save(result);
        ASSERT_EQ(0b1100, result.OtherStorageLevels);
    }

    TEST_F(TechSaveTest, Save_Age1_Level1_SetsLevel) {
        inventionPs->Publish(Technologies::InfluenceTechs[0].Levels[0]);

        Technologies::SaveState result{};
        Technologies::Save(result);

        ASSERT_EQ(0b0001, result.ResourceLevels1and2);
    }

    TEST_F(TechSaveTest, Save_Age1_Level2_SetsLevel) {
        inventionPs->Publish(Technologies::InfluenceTechs[0].Levels[1]);

        Technologies::SaveState result{};
        Technologies::Save(result);

        ASSERT_EQ(0b0010, result.ResourceLevels1and2);
    }

    TEST_F(TechSaveTest, Save_Age1_Level1And2_SetsLevel) {
        inventionPs->Publish(Technologies::InfluenceTechs[0].Levels[0]);
        inventionPs->Publish(Technologies::InfluenceTechs[0].Levels[1]);

        Technologies::SaveState result{};
        Technologies::Save(result);

        ASSERT_EQ(0b0010, result.ResourceLevels1and2);
    }

    TEST_F(TechSaveTest, Save_Age1_Level123_SetsLevel) {
        inventionPs->Publish(Technologies::InfluenceTechs[0].Levels[0]);
        inventionPs->Publish(Technologies::InfluenceTechs[0].Levels[1]);
        inventionPs->Publish(Technologies::InfluenceTechs[0].Levels[2]);

        Technologies::SaveState result{};
        Technologies::Save(result);

        ASSERT_EQ(0b0011, result.ResourceLevels1and2);
    }

    TEST_F(TechSaveTest, Save_Age2_Level1_SetsLevel) {
        inventionPs->Publish(Technologies::InfluenceTechs[1].Levels[0]);

        Technologies::SaveState result{};
        Technologies::Save(result);

        ASSERT_EQ(0b0100, result.ResourceLevels1and2);
    }

    TEST_F(TechSaveTest, Save_Age1And2_Level3_SetsLevel) {
        inventionPs->Publish(Technologies::InfluenceTechs[0].Levels[2]);
        inventionPs->Publish(Technologies::InfluenceTechs[1].Levels[2]);

        Technologies::SaveState result{};
        Technologies::Save(result);

        ASSERT_EQ(0b1111, result.ResourceLevels1and2);
    }

    TEST_F(TechSaveTest, Save_WithAge3_Level1_SetsLevel) {
        inventionPs->Publish(Technologies::InfluenceTechs[2].Levels[0]);

        Technologies::SaveState result{};
        Technologies::Save(result);

        ASSERT_EQ(0b0001, result.ResourcesLevels3to18);
    }

} // namespace Invent
*/