#pragma once

#include "Platform/Platform.h"
#include "Game.h"

#include <utility>

class Engine {
public:
    Engine(Platform& platform, Game& game) : m_Platform(platform), m_Game(game) {}
    ~Engine();

    bool Initialize();
    void Tick() const;

private:
    Platform& m_Platform;
    Game& m_Game;
};
