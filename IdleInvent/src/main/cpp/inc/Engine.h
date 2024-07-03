#pragma once

#include "Platform/Platform.h"

#include <utility>

namespace Invent {
    struct SaveState;
}

struct Engine {
    Engine(Platform& platform, Invent::SaveState& saveState);
    ~Engine();

    void Tick() const;
    void SaveState(Invent::SaveState& outState) const;
};
