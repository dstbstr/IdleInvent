#pragma once
#include "Platform/NumTypes.h"
#include "Instrumentation/Logging.h"

#include <ranges>
#include <span>
#include <string_view>

namespace Ui {
    template<typename TScreen>
    struct ScreenDefinition {
        TScreen Id{};
        std::string_view Name{};
        const char* LandingLabel{ nullptr };
        bool (*Initialize)() = nullptr;
        void (*ShutDown)() = nullptr;
        void (*Render)() = nullptr;
    };

    template<typename TScreen>
	class ScreenManager {
    public:
		explicit ScreenManager(std::span<const ScreenDefinition<TScreen>> screens)
			: m_Screens(screens)
		{}

        bool Initialize() {
			bool success = true;
			for (const auto& def : m_Screens) {
				success &= def.Initialize ? def.Initialize() : true;
			}
			DR_ASSERT(success);
            return success;
        }

        void ShutDown() {
            for(const auto& def : m_Screens | std::views::reverse) {
				if (def.ShutDown) def.ShutDown();
			}
        }

        void Render() const {
			DR_ASSERT_MSG(m_ActiveIndex < m_Screens.size(), "Active screen index is out of bounds");
			m_Screens[m_ActiveIndex].Render();
        }

        void SetActiveScreen(TScreen screen) {
			for (size_t i = 0; i < m_Screens.size(); ++i) {
				if (m_Screens[i].Id == screen) {
					m_ActiveIndex = i;
					return;
				}
			}
			DR_ASSERT_MSG(false, "Unable to find screen");
        }

        TScreen GetActiveScreen() const {
			if (m_ActiveIndex >= m_Screens.size()) {
			    DR_ASSERT_MSG(false, "Unknown screen");
				return TScreen{};
			}
            return m_Screens[m_ActiveIndex].Id;
        }

    private:
		std::span<const ScreenDefinition<TScreen>> m_Screens;
		size_t m_ActiveIndex{ 0 };
	};
}