#pragma once

#include <Platform/NumTypes.h>
#include <Ui/UiUtil.h>

#include <imgui.h>

#include <string>

// Modal-style text dialog with animated reveal. Single active dialog at a time -- calling
// Show() while one is already active is a programmer bug (DR_ASSERT). For sequential
// dialog (e.g. conversation trees), poll IsActive() and only Show() the next one when
// the current one is hidden.
//
// Long text is pre-split at Show() time into pages that fit the configured box size.
// Each page reveals independently and advances on click.
//
// Per-page lifecycle:
//   Hidden    -> Show()  -> Revealing (page 0)
//   Revealing -> click   -> Revealed   (snap; mouse must be released since Show/advance)
//   Revealing -> tick    -> Revealed   (when reveal time elapsed)
//   Revealed  -> click   -> Revealing (next page) OR Hidden (last page)
//
// Update is driven by TickManager (registered in Initialize). Render is called from the
// main render path AFTER screen content so the dialog sits on top.
namespace Ui::Dialog {
	struct Config {
		std::string Text;
		f32 CharsPerSecond{60.f};

		// Box size as a fraction of MainViewport WorkSize. Defaults sit comfortably on
		// mobile portrait (70% wide, quarter of the screen tall above the bottom edge).
		ImVec2 SizeFrac{0.7f, 0.25f};

		FontSizes Font{FontSizes::H3};
		ImU32 Color{IM_COL32_WHITE};

		// Window background. Alpha is part of the color -- use 255 for fully opaque
		// (e.g. classic green-on-black terminal style: BgColor=IM_COL32(0,0,0,255)).
		ImU32 BgColor{IM_COL32(20, 30, 60, 220)};
	};

	bool Initialize();
	void ShutDown();

	// Splits the text into pages that fit Config::SizeFrac and begins revealing the first.
	// Asserts if a dialog is already active.
	void Show(Config config);

	// Force-close. No-op if already Hidden. Safe to call from any state.
	void Hide();

	[[nodiscard]] bool IsActive();

	// Draws the dialog if active. Captures input (the underlying window won't receive
	// clicks). Safe to call every frame regardless of state -- no-ops when Hidden.
	void Render();
}
