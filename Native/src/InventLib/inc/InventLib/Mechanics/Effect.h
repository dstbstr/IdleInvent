#pragma once

#include "Core/NumTypes.h"

#include "InventLib/Mechanics/Modifier.h"

#include <string>

namespace Invent {
    enum struct EffectTarget {
        AllProjectEfficiency,
        AllProjectRate,
        BuildRate,
        BuildEfficiency,
        ExploreRate,
        ExploreEfficiency,
        ExploreSuccessRate,
        ResearchRate,
        ResearchEfficiency,
        PrimaryRate,
        PrimaryCap,
        AllSecondaryRate,
        AllSecondaryCap,
        FollowerRate,
        FollowerCap,
        KnowledgeRate,
        KnowledgeCap,
        MoneyRate,
        MoneyCap,
        PowerRate,
        PowerCap,
        PopulationRate,
        PopulationCap,
        ConversionPower,
        TimeShardCap,
        TickPower,
        WorkerDensity
    };

    std::string ToString(EffectTarget target);

    struct Effect {
        EffectTarget Target;
        Modifier Modifier;
    };

    std::string Describe(const Effect& effect);

    namespace Effects {
        namespace Increase {
            namespace Add {
                constexpr Modifier None = {0, 1.0f};
                constexpr Modifier Small = {1, 1.0f};
                constexpr Modifier Medium = {3, 1.0f};
                constexpr Modifier Large = {5, 1.0f};
                constexpr Modifier XLarge = {10, 1.0f};
                constexpr Modifier XXLarge = {25, 1.0f};
            }
            namespace Mul {
                constexpr Modifier None = {0, 1.0f};
                constexpr Modifier Small = {0, 1.1f};
                constexpr Modifier Medium = {0, 1.3f};
                constexpr Modifier Large = {0, 1.5f};
                constexpr Modifier XLarge = {0, 2.f};
                constexpr Modifier XXLarge = {0, 3.f};
            }

            constexpr Modifier Next(Modifier current) {
                if(current == Increase::Add::None) return Increase::Add::Small;
                if(current == Increase::Add::Small) return Increase::Add::Medium;
                if(current == Increase::Add::Medium) return Increase::Add::Large;
                if(current == Increase::Add::Large) return Increase::Add::XLarge;
                if(current == Increase::Add::XLarge) return Increase::Add::XXLarge;
                if(current == Increase::Mul::None) return Increase::Mul::Small;
                if(current == Increase::Mul::Small) return Increase::Mul::Medium;
                if(current == Increase::Mul::Medium) return Increase::Mul::Large;
                if(current == Increase::Mul::Large) return Increase::Mul::XLarge;
                if(current == Increase::Mul::XLarge) return Increase::Mul::XXLarge;

                return current;
            }
        }

        namespace Decrease {
            namespace Add {
                constexpr Modifier Small = {-1, 1.0f};
                constexpr Modifier Medium = {-2, 1.0f};
                constexpr Modifier Large = {-5, 1.0f};
                constexpr Modifier XLarge = {-10, 1.0f};
            }
            namespace Mul {
                constexpr Modifier Small = {0, 0.9f};
                constexpr Modifier Medium = {0, 0.75f};
                constexpr Modifier Large = {0, 0.5f};
                constexpr Modifier XLarge = {0, 0.25f};
            }
        }
    }
}