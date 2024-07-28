#include "CommonTest.h"

#include "InventLib/Character/Society.h"
#include "InventLib/Settings/GameSettings.h"

namespace Invent {
    TEST(SocietyTest, StartLife_WithPower_SetsPrimaryProgress) {
        Society society{};
        GameSettings settings{};

        society.Start(ResourceName::Power, settings);

        ASSERT_EQ(2.0, society.CurrentLife.ResourceProgressions[ResourceName::Primary].GetProgress(OneSecond));
    }
}