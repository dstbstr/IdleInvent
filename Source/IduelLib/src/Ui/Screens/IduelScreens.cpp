#include "Iduel/Ui/Screens/IduelScreens.h"
#include <Instrumentation/Logging.h>

namespace {
    using namespace Iduel::IduelUi;
    constexpr std::array Definitions {
        Ui::ScreenDefinition<Screen>{
            .Id = Screen::Start,
			.Initialize = Screens::Start::Initialize,
			.ShutDown = Screens::Start::ShutDown,
			.Render = Screens::Start::Render
        },
        Ui::ScreenDefinition<Screen>{
			.Id = Screen::Match,
			.Initialize = Screens::Match::Initialize,
			.ShutDown = Screens::Match::ShutDown,
			.Render = Screens::Match::Render
        },
        Ui::ScreenDefinition<Screen>{
			.Id = Screen::Review,
			.Initialize = Screens::Review::Initialize,
			.ShutDown = Screens::Review::ShutDown,
			.Render = Screens::Review::Render
        },
		Ui::ScreenDefinition<Screen>{
			.Id = Screen::Editor,
			.Initialize = Screens::Editor::Initialize,
			.ShutDown = Screens::Editor::ShutDown,
			.Render = Screens::Editor::Render
		}
    };

    Ui::ScreenManager<Screen> Manager{Definitions};
} // namespace

namespace Iduel::IduelUi {
	Ui::ScreenManager<Screen>& GetScreenManager() {
		return Manager;
	}
} // namespace Iduel::IduelUi
