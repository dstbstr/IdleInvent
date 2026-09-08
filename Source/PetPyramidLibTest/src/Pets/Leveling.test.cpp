#include "Pets/Pets/Leveling.h"

namespace Pets {
	struct LevelingTest : public ::testing::Test {
		
	};

	TEST(LevelingTest, InitialLevel_NextXp_Is100) {
		auto nextXp = Leveling::NextLevelXp(1);

		ASSERT_EQ(100, nextXp);
	}

	TEST(LevelingTest, NextLevel_NextXp_IsDouble) {
		auto initial = Leveling::NextLevelXp(1);
        auto next = Leveling::NextLevelXp(2);

		ASSERT_EQ(initial * 2, next);
	}

	TEST(LevelingTest, GrantXp_WithNotEnough_AddsXp) {
        auto pet = OwnedPet{.Kind = PetKind::Bus, .Level = 1, .Experience = 0};
        auto result = Leveling::GrantXp(pet, 50);

		ASSERT_FALSE(result.LeveledUp());
        ASSERT_EQ(50, pet.Experience);
	}

	TEST(LevelingTest, GrantXp_ExactlyEnough_LevelsUp) {
        auto pet = OwnedPet{.Kind = PetKind::Bus, .Level = 1, .Experience = 0};

        auto result = Leveling::GrantXp(pet, Leveling::NextLevelXp(pet.Level));
        ASSERT_TRUE(result.LeveledUp());
        ASSERT_EQ(1, result.PreviousLevel);
        ASSERT_EQ(2, result.CurrentLevel);
        ASSERT_EQ(0, pet.Experience);
        ASSERT_EQ(2, pet.Level);
	}

	TEST(LevelingTest, GrantXp_ThreeLevels_LevelsThreeTimes) {
        auto pet = OwnedPet{.Kind = PetKind::Bus, .Level = 1, .Experience = 0};
		
		auto xp = Leveling::NextLevelXp(pet.Level) + 
				  Leveling::NextLevelXp(pet.Level + 1) +
                  Leveling::NextLevelXp(pet.Level + 2);
        auto result = Leveling::GrantXp(pet, xp);
        ASSERT_TRUE(result.LeveledUp());
        ASSERT_EQ(1, result.PreviousLevel);
        ASSERT_EQ(4, result.CurrentLevel);
        ASSERT_EQ(0, pet.Experience);
        ASSERT_EQ(4, pet.Level);
	}
}