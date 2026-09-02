#pragma once

#include "Ui/UiGeometry.h"

#include <GameState/GameTime.h>

#include <imgui.h>
#include <string>
#include <variant>
#include <vector>
#include <queue>
#include <optional>

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
		BaseTime Duration{};
        ImU32 Color{IM_COL32_WHITE};
		bool Fade{false};
	};

	class ToastManager {
    public:
		ToastManager(ToastManagerConfig config);

		void AddToast(Toast toast);
		void AddToast(const std::string& toast, BaseTime duration);
        void AddToast(ToastImage image, BaseTime duration);

		void Tick(BaseTime elapsed);
		void Render() const;

	private:
		struct ToastSlot {
			ImVec2 Position{};
			std::optional<Toast> Active{};
		};

        ToastManagerConfig m_Config{};
		std::queue<Toast> m_PendingToasts{};
		std::vector<ToastSlot> m_Slots{};

		void StartPendingToasts();
	};
}