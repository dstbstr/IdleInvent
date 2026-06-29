#include <SampleUI/Ui.h>
#include <SampleUI/Screens/Screens.h>

#include <Ui/UiUtil.h>

namespace SampleUI {
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
