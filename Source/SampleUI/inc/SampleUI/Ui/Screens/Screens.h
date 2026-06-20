#pragma once

#include <Platform/NumTypes.h>
#include <string>

namespace SampleUI::Ui {
	enum struct Screen : u8 { Landing, SampleTreePanel };

	std::string ToString(Screen screen);

	namespace Screens {
		bool Initialize();
		void ShutDown();

		void Render();
		void SetActiveScreen(Screen screen);
		Screen GetActiveScreen();
	} // namespace Screens
} // namespace SampleUI::Ui
