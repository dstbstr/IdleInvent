#include "InventLib/Character/Death.h"

#include <DesignPatterns/ServiceLocator.h>
#include <GameState/GameTime.h>
#include <Instrumentation/Logging.h>
#include <Platform/NumTypes.h>
#include <Utilities/IRandom.h>

#include <format>

namespace Invent {
    Death GenerateDeath(size_t baseLifeSpan) {
        IRandom& random = ServiceLocator::Get().GetRequired<IRandom>();
        Death death;
        NearDeath& nearDeath = death.NearDeathData;

        death.Type = static_cast<DeathType>(random.GetNext(0, 3));
        auto change = random.GetNextFloat();

        double tod = 1.0;
        double warning = 1.0;
        switch(death.Type) {
        case DeathType::Sudden: {
            // lifespan reduced by 30-50%
            tod = 0.5f + change * 0.2f;
            warning = 0.95;
            nearDeath.WarningMessage = "You saw an ill omen, that can't be good";
            break;
        }
        case DeathType::Illness: {
            // lifespan increased or decreased by 20%
            tod = 0.8f + change * 0.4f;
            warning = 0.85;
            nearDeath.WarningMessage = "You're feeling a bit under the weather";
            break;
        }
        case DeathType::Slow: {
            // lifespan increased by 25-40%
            tod = 1.25f + change * 0.15f;
            warning = 0.7;
            nearDeath.WarningTime = OneMinute * random.GetNext(10, 20);
            nearDeath.WarningMessage = "You're healthy as an ox";
            break;
        }
        }

        death.TimeOfDeath = static_cast<size_t>(baseLifeSpan * tod) * OneGameYear;
        death.TimeOfDeath = std::max(death.TimeOfDeath, OneGameYear * 20); // minimum lifespan of 20 years
        nearDeath.WarningTime = BaseTime(static_cast<size_t>(death.TimeOfDeath.count() * warning));

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