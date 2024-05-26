#pragma once

#include <utility>

#include "Platform/Platform.h"

struct Engine {
    Engine(Platform& platform);
    ~Engine();

    void Tick() const;
};
