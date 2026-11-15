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
		SampleCombat,
		SampleAudio,
		SampleBigNum,
		SampleChart
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

		namespace SampleAudio {
			bool Initialize();
			void ShutDown();

			void Render();
		}
		namespace SampleBigNum {
			bool Initialize();
			void ShutDown();
			void Render();
		}
		namespace SampleChart {
			bool Initialize();
			void ShutDown();
			void Render();
		}
		namespace SampleCombat {
			bool Initialize();
			void ShutDown();
			void Render();
		}
		namespace SampleGiantMap {
			bool Initialize();
			void ShutDown();
			void Render();
		}
		namespace SampleNav {
			bool Initialize();
			void ShutDown();
			void Render();
		}
		namespace SampleParticles {
			bool Initialize();
			void ShutDown();
			void Render();
		}
		namespace SampleSimpleMap {
			bool Initialize();
			void ShutDown();
			void Render();
		}
		namespace SampleTreePanel {
			bool Initialize();
			void ShutDown();
			void Render();
		}
	}
}
