#include "Pets/Ui/MainContent.h"
#include "Pets/Ui/Ui.h"
#include "Pets/Ui/Screens/Screens.h"

namespace Pets::Ui::Screens::MainContent {
	bool Initialize() {
		return Screens::Initialize();
	}

	void Render() {
		Screens::Render();
	}

	void ShutDown() {
		Screens::ShutDown();
	}
} // namespace Pets::Ui::Screens::MainContent
