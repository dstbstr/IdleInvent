#pragma once

namespace Pets::Ui::Screens::BottomContent {
	bool Initialize();
	void Render();
	void ShutDown();

	f32 GetRequestedHeight(f32 availableWidth);
} // namespace Pets::Ui::Screens::BottomContent
