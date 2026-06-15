#pragma once

#include "InventLib/Character/GameCharacter.h"
#include "InventLib/Resources/InventStorage.h"
#include "InventLib/Character/Life.h"

#include <GameState/GameState.h>
#include <Utilities/Handle.h>

namespace Invent {
	struct InventGameStateSave : public GameStateSave {
        GameCharacterSave CharacterSave;
        StorageSave StorageSave;
	};
	struct InventGameState : public GameState {
        InventGameState();
        //~InventGameState() = default;
        //InventGameState(const InventGameState& other) = delete;
        //InventGameState& operator=(const InventGameState& other) = delete;
        //InventGameState(InventGameState&& other) noexcept = delete;
        //InventGameState& operator=(InventGameState&& other) noexcept = delete;

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