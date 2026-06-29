#pragma once

#include <Platform/NumTypes.h>
#include <string>

namespace SampleUI {
	enum struct Screen : u8 { Landing, SampleTreePanel, SampleParticles };

	std::string ToString(Screen screen);

	namespace Screens {
		bool Initialize();
		void ShutDown();

		void Render();
		void SetActiveScreen(Screen screen);
		Screen GetActiveScreen();
	}
}
