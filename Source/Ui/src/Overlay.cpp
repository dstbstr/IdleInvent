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
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoInputs;
 
    struct Placement {
        ImVec2 Position{};
        ImVec2 Pivot{};
    };

    Placement GetPlacement(Ui::Overlay::Anchor anchor) {
        auto* viewport = ImGui::GetMainViewport();
        auto left = viewport->WorkPos.x + OverlayPadding.x;
        auto right = viewport->WorkPos.x + viewport->WorkSize.x - OverlayPadding.x;
        auto top = viewport->WorkPos.y + OverlayPadding.y;
        auto bottom = viewport->WorkPos.y + viewport->WorkSize.y - OverlayPadding.y;
        switch(anchor) {
            using enum Ui::Overlay::Anchor;
            case TopLeft: return {{left, top}, {0.f, 0.f}};
            case TopRight: return {{right, top}, {1.f, 0.f}};
            case BottomLeft: return {{left, bottom}, {0.f, 1.f}};
            case BottomRight: return {{right, bottom}, {1.f, 1.f}};
        }
        
        return {};
    }
}

namespace Ui::Overlay {
	void DrawFps() {
        Draw("##FpsOverlay", Anchor::TopRight, [] {
            ImGui::Text("FPS: %.0f", static_cast<double>(ImGui::GetIO().Framerate));
        });
	}

	void Draw(const char* id, Anchor anchor, const OverlayRenderFn& renderFn) {
        auto placement = GetPlacement(anchor);

		ImGui::SetNextWindowPos(placement.Position, ImGuiCond_Always, placement.Pivot);
        ImGui::SetNextWindowBgAlpha(0.35f);

		if(ImGui::Begin(id, nullptr, OverlayFlags)) renderFn();
        ImGui::End();
    }
}
