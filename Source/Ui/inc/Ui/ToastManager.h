#pragma once

#include "Ui/UiGeometry.h"

#include <GameState/GameTime.h>

#include <imgui.h>
#include <string>
#include <variant>
#include <vector>

namespace Ui {
	struct ToastManagerConfig {
		std::vector<ImVec2> ToastPositions{};
		ImVec2 ToastVelocity{};
		ImFont* ToastFont{nullptr};
	};

	struct ToastImage {
		ImTextureID Texture{};
		ImVec2 Size{};
		ImVec2 UvMin{0.f, 0.f};
		ImVec2 UvMax{1.f, 1.f};
	};

	struct Toast {
		std::variant<std::string, ToastImage> Content;
		ImVec2 Position{};
		BaseTime Lifetime{};
	};

	class ToastManager {
    public:
		ToastManager(ToastManagerConfig config) 
            : m_Config(config) {
			DR_ASSERT_MSG(!config.ToastPositions.empty(), "Must have at least 1 toast position");
			if(config.ToastPositions.empty()) {
				config.ToastPositions.push_back({0, 0});
			}
		}

		void AddToast(const std::string& toast, BaseTime duration);
        void AddToast(ToastImage image, BaseTime duration);

		void Tick(BaseTime elapsed);
		void Render() const;

	private:
        ToastManagerConfig m_Config{};
        std::vector<Toast> m_Toasts{};
		size_t m_PositionIndex{0};

		ImVec2 GetNextPosition();
	};
}