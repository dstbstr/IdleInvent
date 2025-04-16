#pragma once

#include "InventLib/Character/GameCharacter.h"
#include "InventLib/Resources/InventStorage.h"
#include "InventLib/Character/Life.h"

#include <GameState/GameState.h>

namespace Invent {
	struct InventGameStateSave : public GameStateSave {
        GameCharacterSave CharacterSave;
        StorageSave StorageSave;
	};
	struct InventGameState : public GameState {
        InventGameState();
        ~InventGameState();

        void Save(InventGameStateSave& save) const;
        void Load(const InventGameStateSave& save);
        void Tick(BaseTime elapsed);
        InventStorageCollection Storages;
        //GameCharacter Character;
        //Life CurrentLife;
    private:
        void SetupSubscriptions();

        size_t m_StorageHandle{0};
        size_t m_AgeHandle{0};
		// size_t m_EffectHandle{0};
        // size_t m_DeathHandle{0};
	};
}