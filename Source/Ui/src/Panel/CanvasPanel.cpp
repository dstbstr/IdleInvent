#include "Ui/Panel/CanvasPanel.h"

#include <imgui.h>

namespace Ui {
	void CanvasPanel::RenderImpl() {
		// We're inside Panel::Render()'s BeginChild, so cursor positions are child-local.
		// The InvisibleButton fills the child's content region exactly.
		const ImVec2 size = GetSize();
		ImGui::SetCursorPos(ImVec2{0.f, 0.f});

		ImGui::PushID(static_cast<const void*>(this));
		ImGui::InvisibleButton("##CanvasPanel", size);
		ImGui::PopID();

		m_Input.IsHovered = ImGui::IsItemHovered();

		// Claim ownership of the press only when it actually starts on this button.
		// IsItemActivated is true for exactly one frame -- the frame of the click. We hold
		// that ownership until the mouse fully releases. This prevents inheriting an
		// in-progress press from a widget that disappeared (e.g. a modal closing while the
		// mouse is still down), which would otherwise spuriously trip IsItemActive on us.
		if(ImGui::IsItemActivated()) {
			m_PressOwnedByCanvas = true;
		}
		if(!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			m_PressOwnedByCanvas = false;
		}
		m_Input.IsActivate = m_PressOwnedByCanvas && ImGui::IsItemActive();

		m_Input.MouseScreen  = ImGui::GetIO().MousePos;
		m_Input.MouseLocal   = ScreenToLocal(m_Input.MouseScreen);
		m_Input.MouseContent = LocalToContent(m_Input.MouseLocal);
	}

	ImVec2 CanvasPanel::ScreenToLocal(ImVec2 screen) const {
		// Panel-local origin in screen space = panel BeginChild origin. That's the cursor
		// position at the start of RenderImpl, which is ToScreenSpace({0,0}) inside the
		// child. ImGui's GetWindowPos() inside the child returns the same thing.
		const auto windowPos = ImGui::GetWindowPos();
		return ImVec2{screen.x - windowPos.x, screen.y - windowPos.y};
	}

	ImVec2 CanvasPanel::LocalToContent(ImVec2 local) const {
		// Inverse of the BuildLayout transform used in TreePanel:
		//   local = content * zoom + contentOrigin + pan
		// => content = (local - contentOrigin - pan) / zoom
		const auto pan = GetPanOffset();
		const f32  zoom = GetZoom();
		const f32  invZoom = (zoom != 0.f) ? (1.f / zoom) : 1.f;
		// ContentOrigin isn't exposed by Panel; consumers that set it via SetContentOrigin
		// know its value. For canvas use we treat origin as (0,0) since CanvasPanel doesn't
		// set ContentOrigin itself. If a future caller does set it, expose a getter on Panel.
		return ImVec2{(local.x - pan.x) * invZoom, (local.y - pan.y) * invZoom};
	}

	ImVec2 CanvasPanel::ScreenToContent(ImVec2 screen) const {
		return LocalToContent(ScreenToLocal(screen));
	}

	ImVec2 CanvasPanel::ContentToLocal(ImVec2 content) const {
		const auto pan = GetPanOffset();
		const f32  zoom = GetZoom();
		return ImVec2{content.x * zoom + pan.x, content.y * zoom + pan.y};
	}

	ImVec2 CanvasPanel::LocalToScreen(ImVec2 local) const {
		const auto windowPos = ImGui::GetWindowPos();
		return ImVec2{windowPos.x + local.x, windowPos.y + local.y};
	}

	ImVec2 CanvasPanel::ContentToScreen(ImVec2 content) const {
		return LocalToScreen(ContentToLocal(content));
	}
}
