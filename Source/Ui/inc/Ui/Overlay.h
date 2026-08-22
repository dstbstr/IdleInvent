#pragma once
#include <Platform/NumTypes.h>
#include <functional>

namespace Ui::Overlay {
	void DrawFps();

	enum struct Anchor : u8 {
		TopLeft,
		TopRight,
		BottomLeft,
		BottomRight
	};
	using OverlayRenderFn = std::function<void()>;

	void Draw(const char* id, Anchor anchor, const OverlayRenderFn& renderFn);
}
