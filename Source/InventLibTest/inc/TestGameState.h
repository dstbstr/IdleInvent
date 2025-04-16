#pragma once

#include "GameState/GameState.h"

#include "Utilities/IRandom.h"
#include "DesignPatterns/ServiceLocator.h"

namespace Invent {
    inline GameState& GenerateTestGameState() {
        if (!ServiceLocator::Get().Get<IRandom>()) {
            ServiceLocator::Get().SetThisAsThat<DefaultRandom, IRandom>();
        }
        ServiceLocator::Get().Set<GameState>();
        ServiceLocator::Get().Reset<IRandom>();

        return ServiceLocator::Get().GetRequired<GameState>();
    }
}