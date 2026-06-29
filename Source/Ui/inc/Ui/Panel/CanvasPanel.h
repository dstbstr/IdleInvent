#pragma once

#include "Ui/Panel/Panel.h"
#include "Platform/NumTypes.h"

#include <imgui.h>

namespace Ui {
	// Snapshot of pointer state captured during the last Render() call. All values are
	// valid every frame; IsHovered / IsActivate gate whether the mouse data is meaningful
	// to the caller. Future fields (IsDragging, DragDelta, IsInterrogate, etc.) will
	// appear here when their tracking is built.
	struct CanvasInput {
		bool   IsHovered{false};   // mouse is over the canvas and no higher window blocks it
		bool   IsActivate{false};  // primary press held (left mouse / short tap), began on canvas

		ImVec2 MouseScreen{};      // raw screen pixels (matches ImGui::GetIO().MousePos)
		ImVec2 MouseLocal{};       // panel-local pixels (top-left = origin), pre-pan/zoom
		ImVec2 MouseContent{};     // content-space (pan/zoom undone)
	};

	// CanvasPanel: a Panel-derived interactive region. Place it like any other Panel;
	// each frame, call Render() then read GetInput() to drive your gameplay/input logic.
	//
	// Coordinate spaces:
	//   Screen  -- raw pixel coords from ImGui.
	//   Local   -- panel-local; (0,0) is the panel's top-left, regardless of pan/zoom.
	//   Content -- panel-local with pan/zoom undone; what your "world" coords are.
	//
	// Right-click pan and wheel zoom are inherited from Panel and remain active. The
	// canvas only consumes the LEFT mouse button via an internal InvisibleButton.
	class CanvasPanel : public Panel {
	public:
		explicit CanvasPanel(const PanelConfig& config) : Panel(config) {}

		// Input state captured during the most recent Render() call.
		[[nodiscard]] const CanvasInput& GetInput() const { return m_Input; }

		// Coordinate-space conversions exposed so callers can map non-mouse points too
		// (e.g. "where would this content-space rect appear on screen?").
		[[nodiscard]] ImVec2 ScreenToLocal(ImVec2 screen) const;
		[[nodiscard]] ImVec2 LocalToContent(ImVec2 local) const;
		[[nodiscard]] ImVec2 ScreenToContent(ImVec2 screen) const;
		[[nodiscard]] ImVec2 ContentToLocal(ImVec2 content) const;
		[[nodiscard]] ImVec2 LocalToScreen(ImVec2 local) const;
		[[nodiscard]] ImVec2 ContentToScreen(ImVec2 content) const;

	protected:
		void RenderImpl() override;

	private:
		CanvasInput m_Input{};
		// True from the frame IsItemActivated() fires until the mouse fully releases.
		// Distinguishes "press started on me" from "ActiveId inherited because another
		// widget disappeared mid-press," which is a real edge case when modals close.
		bool m_PressOwnedByCanvas{false};
	};
}
