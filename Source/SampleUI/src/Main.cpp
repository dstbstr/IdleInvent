#include <Engine.h>
#include <Game.h>
#include <Platform/Graphics.h>
#include <Platform/PlatformMain.h>
#include <SampleUI/Ui/Ui.h>

#include <memory>

namespace SampleUiApp {
	class SampleGame : public Game {
	public:
		explicit SampleGame(::Platform& platform) : Game(platform) {}

		bool Initialize() override { return SampleUI::Ui::Initialize(); }
		void ShutDown() override { SampleUI::Ui::ShutDown(); }

		void LoadGame() override {}
		void SaveGame() override {}
		void DeleteGame() override {}

		void Tick([[maybe_unused]] BaseTime elapsed) override {
			Graphics::Render(SampleUI::Ui::Render);
		}
	};

	class SampleGameState : public IGameState {
	public:
		explicit SampleGameState(::Platform& platform)
			: m_Platform(platform)
			, m_Game(platform)
			, m_Engine(m_Platform, m_Game) {
		}

		bool Initialize() override { return m_Engine.Initialize(); }

		void Run() override {
			while(m_Platform.HandleInput()) {
				m_Engine.Tick();
			}
		}

	private:
		::Platform& m_Platform;
		SampleGame m_Game;
		Engine m_Engine;
	};
} // namespace SampleUiApp

std::unique_ptr<IGameState> GetGameState(::Platform& platform) {
	return std::make_unique<SampleUiApp::SampleGameState>(platform);
}
