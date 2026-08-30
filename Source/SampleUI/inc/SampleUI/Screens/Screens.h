#pragma once

#include <Platform/NumTypes.h>
#include <span>
#include <string_view>

namespace SampleUI {
	enum struct Screen : u8 { 
		Landing, 
		SampleTreePanel, 
		SampleParticles, 
		SampleSimpleMap, 
		SampleNav, 
		SampleGiantMap,
		SampleCombat
	};

    struct ScreenDefinition {
        Screen Id{};
        std::string_view Name{};
        const char* LandingLabel{nullptr};
        bool (*Initialize)() = nullptr;
        void (*ShutDown)() = nullptr;
        void (*Render)() = nullptr;
    };

    std::span<const ScreenDefinition> GetScreenDefinitions();
    const ScreenDefinition& GetScreenDefinition(Screen screen);

	namespace Screens {
		bool Initialize();
		void ShutDown();

		void Render();
		void SetActiveScreen(Screen screen);
		Screen GetActiveScreen();
	}
}
