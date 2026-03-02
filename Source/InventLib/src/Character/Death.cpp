#include "InventLib/Character/Death.h"

#include <DesignPatterns/ServiceLocator.h>
#include <GameState/GameTime.h>
#include <Instrumentation/Logging.h>
#include <Platform/NumTypes.h>
#include <Utilities/IRandom.h>

#include <format>
namespace {
    constexpr double SuddenWarning = 0.95;
    constexpr double IllnessWarning = 0.85;
    constexpr double SlowWarning = 0.7;

    constexpr auto MinimumLifespan = OneGameYear * 20;
    
    constexpr double SuddenToD(double change) {
        // lifespan reduced by 30-50%
        const auto base = 0.5f;
        const auto range = 0.2f;
        return base + change * range;
    }
    constexpr double IllnessToD(double change) {
        // lifespan increased or decreased by 20%
        const auto base = 1.0f;
        const auto range = 0.4f;
        return base - range / 2 + change * range;
    }
    constexpr double SlowToD(double change) {
        // lifespan increased by 25-40%
        const auto base = 1.25f;
        const auto range = 0.15f;
        return base + change * range;
    }
}
namespace Invent {
    Death GenerateDeath(u32 baseLifeSpan) {
        IRandom& random = ServiceLocator::Get().GetRequired<IRandom>();
        Death death;
        NearDeath& nearDeath = death.NearDeathData;

        death.Type = static_cast<DeathType>(random.GetNext(0, 3));
        auto change = random.GetNextFloat();

        double tod = 1.0;
        double warning = 1.0;
        switch(death.Type) {
        case DeathType::Sudden: {
            tod = SuddenToD(change);
            warning = SuddenWarning;
            nearDeath.WarningMessage = "You saw an ill omen, that can't be good";
            break;
        }
        case DeathType::Illness: {
            tod = IllnessToD(change);
            warning = IllnessWarning;
            nearDeath.WarningMessage = "You're feeling a bit under the weather";
            break;
        }
        case DeathType::Slow: {
            tod = SlowToD(change);
            warning = SlowWarning;
            nearDeath.WarningTime = OneMinute * random.GetNext(10, 20);
            nearDeath.WarningMessage = "You're healthy as an ox";
            break;
        }
        }

        death.TimeOfDeath = static_cast<s64>(baseLifeSpan * tod) * OneGameYear;
        death.TimeOfDeath = std::max(death.TimeOfDeath, MinimumLifespan);
        nearDeath.WarningTime = BaseTime(static_cast<s64>(static_cast<double>(death.TimeOfDeath.count()) * warning));

        Log::Info(std::format("Cause of death will be {} at age {}", ToString(death), (death.TimeOfDeath.count() / OneGameYear.count())));
        return death;
    }

    void Death::Save(DeathSave& save) const {
        Log::Info("Saving death");
		save.DeathYear = static_cast<u8>(TimeOfDeath / OneGameYear);
		save.NearDeathYear = static_cast<u8>(NearDeathData.WarningTime / OneGameYear);
		save.TypeOfDeath = static_cast<u8>(Type);
		save.HasWarned = NearDeathData.HasWarned;
	}

    Death LoadDeath(const DeathSave& save) {
        Log::Info("Loading death");
		Death death;
		death.TimeOfDeath = BaseTime(save.DeathYear * OneGameYear.count());
		death.NearDeathData.HasWarned = save.HasWarned;
		death.NearDeathData.WarningTime = BaseTime(save.NearDeathYear * OneGameYear.count());
		death.Type = static_cast<DeathType>(save.TypeOfDeath);
		return death;
	}

    std::string ToString(const Death& death) {
        switch(death.Type) {
        case DeathType::Sudden: return "You have died suddenly";
        case DeathType::Illness: return "You have died of an illness";
        case DeathType::Slow: return "You have died of old age";
        default: return "You have died";
        }
    }
} // namespace Invent