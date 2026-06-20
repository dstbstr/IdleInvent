#include <SampleUI/Ui/Screens/SampleTreePanel.h>

#include <SampleUI/Ui/Screens/Screens.h>

#include <Platform/Graphics.h>
#include <Ui/UiUtil.h>

#include <imgui.h>

namespace {
	constexpr auto HeaderOffsetY = 32.f;
}

namespace SampleUI::Ui::Screens::SampleTreePanel {
	bool Initialize() {
		return true;
	}

	void ShutDown() {}

	void Render() {
		ImGui::SetNextWindowPos({0.f, 0.f});
		ImGui::SetNextWindowSize({Graphics::ScreenWidth, Graphics::ScreenHeight});

		ImGui::Begin("Sample TreePanel", nullptr, BaseUiFlags);

		if(ImGui::Button("Back")) {
			Screens::SetActiveScreen(Screen::Landing);
		}

		ImGui::SetCursorPosY(HeaderOffsetY);
		if(auto* headerFont = GetFont(FontSizes::H1)) {
			ImGui::PushFont(headerFont);
			TextCenteredX("TreePanel Sample");
			ImGui::PopFont();
		} else {
			TextCenteredX("TreePanel Sample");
		}

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::TextUnformatted("This screen will host TreePanel experiments.");

		ImGui::End();
	}
} // namespace SampleUI::Ui::Screens::SampleTreePanel
