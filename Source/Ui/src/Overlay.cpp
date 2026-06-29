#include "Ui/Overlay.h"

#include <imgui.h>

namespace {
	// Top-right corner padding from the viewport edge.
	constexpr ImVec2 OverlayPadding{8.f, 8.f};

	// Reused for every overlay window so they share the same look.
	constexpr ImGuiWindowFlags OverlayFlags =
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoInputs;
}

namespace Ui::Overlay {
	void DrawFps() {
		const auto* viewport = ImGui::GetMainViewport();
		const auto anchor = ImVec2{
			viewport->WorkPos.x + viewport->WorkSize.x - OverlayPadding.x,
			viewport->WorkPos.y + OverlayPadding.y
		};
		// Pivot (1, 0) anchors the window's top-right corner at our anchor point.
		ImGui::SetNextWindowPos(anchor, ImGuiCond_Always, ImVec2{1.f, 0.f});
		ImGui::SetNextWindowBgAlpha(0.35f);

		if(ImGui::Begin("##FpsOverlay", nullptr, OverlayFlags)) {
			ImGui::Text("FPS: %.0f", static_cast<double>(ImGui::GetIO().Framerate));
		}
		ImGui::End();
	}
}
