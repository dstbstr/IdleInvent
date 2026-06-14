#pragma once

#include "Platform/NumTypes.h"
#include <imgui.h>

namespace Pets::Ui {
    enum struct FontSizes : u8 { H1, H2, H3, H4 };
    ImFont* GetFont(FontSizes font);

	bool Initialize();
	void Render();
	void ShutDown();
} // namespace Pets::Ui
