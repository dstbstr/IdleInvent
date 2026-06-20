#include <SampleUI/Ui/Ui.h>
#include <SampleUI/Ui/Screens/Screens.h>

#include <Ui/UiUtil.h>

namespace SampleUI::Ui {
	bool Initialize() {
		InitializeFonts("DroidSans.ttf");
		return Screens::Initialize();
	}

	void Render() {
		Screens::Render();
	}

	void ShutDown() {
		Screens::ShutDown();
	}
} // namespace SampleUI::Ui
