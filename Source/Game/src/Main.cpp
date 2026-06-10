#include <Platform/PlatformMain.h>
#include <Engine.h>
#include GAME_HEADER // defined by CMakeLists.txt

#include <memory>

template<typename TGame>
class GameState : public IGameState {
public:
    GameState(Platform& platform) 
        : m_Platform(platform)
        , m_Game(platform)
        , m_Engine(m_Platform, m_Game)
    {
    }

    bool Initialize() override { return m_Engine.Initialize(); }
    void Run() override {
        while(m_Platform.HandleInput()) {
            m_Engine.Tick();
        }
    }
private:
    Platform& m_Platform;
    TGame m_Game;
    Engine m_Engine;
};

std::unique_ptr<IGameState> GetGameState(Platform& platform) {
    using CurrentGame = GameState<GAME_TYPE>; // defined by CMakeLists.txt
    return std::make_unique<CurrentGame>(platform);
}