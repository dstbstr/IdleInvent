#pragma once

#include "Invent/Character/GameCharacter.h"
#include "Invent/Resources/InventStorage.h"
#include "Invent/Character/Life.h"

#include <GameState/GameState.h>
#include <Utilities/Handle.h>

namespace Invent {
	struct InventGameStateSave : public GameStateSave {
        GameCharacterSave CharacterSave;
        StorageSave StorageSave;
	};
	struct InventGameState : public GameState {
        InventGameState();

        void Save(InventGameStateSave& save) const;
        void Load(const InventGameStateSave& save);
        void Tick(BaseTime elapsed) override ;
        InventStorageCollection Storages;
        //GameCharacter Character;
        //Life CurrentLife;
    private:
        void SetupSubscriptions();

        std::vector<ScopedHandle> m_PsHandles{};
	};
}