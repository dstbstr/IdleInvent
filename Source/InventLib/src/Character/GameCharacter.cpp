#include "InventLib/Character/GameCharacter.h"

#include <DesignPatterns/PubSub.h>
#include <DesignPatterns/ServiceLocator.h>
#include <GameState/GameTime.h>
#include <Instrumentation/Logging.h>
#include <Platform/NumTypes.h>
#include <Resources/Resource.h>

namespace Invent {
    GameCharacter::GameCharacter(size_t baseLifeSpan) {
        CharacterDeath = GenerateDeath(baseLifeSpan);
    }

    void GameCharacter::Load(const GameCharacterSave& saveData) {
        Log::Info("Loading GameCharacter");

		CurrentAge = saveData.CurrentAge * OneGameYear;
		CharacterDeath = LoadDeath(saveData.DeathData);
		CurrentResources = LoadResources(saveData.ResourceData);
	}

    void GameCharacter::Save(GameCharacterSave& saveData) const {
        Log::Info("Saving GameCharacter");

    	saveData.CurrentAge = static_cast<u8>(CurrentAge / OneGameYear);
		CharacterDeath.Save(saveData.DeathData);
		CurrentResources.Save(saveData.ResourceData);
    }

    void GameCharacter::Tick(BaseTime elapsed) {
        CurrentAge += elapsed;
        if (CurrentAge >= CharacterDeath.TimeOfDeath) {
            ServiceLocator::Get().GetRequired<PubSub<Death>>().Publish(CharacterDeath);
        } else if(CurrentAge >= CharacterDeath.NearDeathData.WarningTime && !CharacterDeath.NearDeathData.HasWarned) {
            CharacterDeath.NearDeathData.HasWarned = true;
            Log::Info("Near death warning: " + CharacterDeath.NearDeathData.WarningMessage);
			ServiceLocator::Get().GetRequired<PubSub<NearDeath>>().Publish(CharacterDeath.NearDeathData);
		}

        //CurrentResources.Tick(elapsed);
    }
} // namespace Invent