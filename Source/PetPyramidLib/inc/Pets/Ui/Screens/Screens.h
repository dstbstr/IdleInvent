#pragma once
#include <string>
#include "Platform/NumTypes.h"

namespace Pets::Ui {
	enum struct Screen : u8 { Pets, Combat, Bestiary, Rebirth, Settings };

	std::string ToString(Screen screen);

	namespace Screens {
		bool Initialize();
		void ShutDown();

		void Render();
		void SetActiveScreen(Screen screen);
		Screen GetActiveScreen();
	} // namespace Screens
} // namespace Pets::Ui
