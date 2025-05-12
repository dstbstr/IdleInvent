#include <Platform/PlatformMain.h>
#include <Engine.h>
//#include <InventLib/InventGame.h>
#include <TechMaze/TechMazeGame.h>

#include <memory>

template<typename TGame>
class GameState : public IGameState {
public:
    GameState(Platform& platform) 
        : m_Platform(platform)
        , m_Game(platform)
        , m_Engine(m_Platform, m_Game)
    {}

    bool Initialize() override { return m_Engine.Initialize(); }
    void Run() override {
        while(m_Platform.HandleInput()) {
            m_Engine.Tick();
        }
    }
private:
    Platform& m_Platform{nullptr};
    TGame m_Game;
    Engine m_Engine;
};

std::unique_ptr<IGameState> GetGameState(Platform& platform) {
    using CurrentGame = GameState<TechMaze::TechMazeGame>;
    return std::make_unique<CurrentGame>(platform);
}