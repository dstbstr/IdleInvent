#pragma once

#include "InventLib/GameState/GameTime.h"
#include "InventLib/Resources/Resource.h"
#include "InventLib/Resources/Storage.h"
//#include "InventLib/Character/GameCharacter.h"
#include "InventLib/Character/Life.h"

namespace Invent {
    struct GameStateSave {
        //GameCharacterSave CharacterSave;
        StorageSave StorageSave;
        u32 ElapsedSeconds{0}; // max value = 4294967295 seconds = 1193 hours = 49 days
        u32 RunElapsedSeconds{0};
    };

	struct GameState {
        GameState();
        ~GameState();

        void Load(const GameStateSave& save);
        void Save(GameStateSave& save) const;

		void Tick(BaseTime elapsed);
        //GameCharacter Character;
        //Life CurrentLife;

		StorageCollection Storages;
		BaseTime TimeElapsed{};
		BaseTime CurrentRunElapsed{};

	private:
        void SetupSubscriptions();

        size_t m_EffectHandle{0};
		size_t m_StorageHandle{0};
        size_t m_AgeHandle{0};
        size_t m_DeathHandle{0};
	};
}