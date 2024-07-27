#include "CommonTest.h"

#include "InventLib/Character/Society.h"

namespace Invent {
    TEST(SocietyTest, StartLife_WithPower_SetsPrimaryProgress) {
        Society society{};
        society.Start(ResourceName::Power);

        ASSERT_EQ(2.0, society.CurrentLife.ResourceProgressions[ResourceName::Primary].GetProgress(OneSecond));
    }
}