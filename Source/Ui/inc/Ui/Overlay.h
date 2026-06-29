#pragma once

// Diagnostic overlays drawn over whatever screen is active. Each function creates its own
// ImGui window with NoInputs flags, so they sit purely on top of existing UI without
// stealing clicks. Anchor positions are computed against the main viewport.
//
// Add new overlays here as they're needed (memory, draw call counts, particle/animation
// counts, etc.). Keep them cheap -- they run every frame on every screen.
namespace Ui::Overlay {
	// Top-right FPS counter with smoothed framerate from ImGui::GetIO().Framerate.
	void DrawFps();
}
