#include <SampleUI/Screens/Landing.h>
#include <SampleUI/Screens/Screens.h>

#include <Platform/Graphics.h>
#include <Ui/UiUtil.h>
#include <Ui/UiGeometry.h>

#include <algorithm>
#include <imgui.h>

namespace {
	constexpr size_t ButtonCols = 3;
	constexpr f32 ButtonSpacing = 24.f;
	constexpr f32 HorizontalMargin = 24.f;
	constexpr auto HeaderOffsetY = 48.f;
	constexpr auto ButtonOffsetY = 140.f;

	struct GridLayout {
        ImVec2 Start{};
        f32 CellSize{};
        f32 Spacing{};

		[[nodiscard]] constexpr Ui::UiRect GetCell(size_t row, size_t col) const {
            auto stride = CellSize + Spacing;
            auto pos = Start + ImVec2{static_cast<f32>(col), static_cast<f32>(row)} * stride;
            return Ui::UiRect::FromPosSize(pos, Ui::One * CellSize);
		}
	};

	[[nodiscard]] GridLayout BuildGrid(size_t buttonRows) {
        const auto windowSize = ImGui::GetWindowSize();
        const auto availableWidth = windowSize.x - (HorizontalMargin * 2.f) - (ButtonSpacing * static_cast<f32>(ButtonCols - 1));
        const auto availableHeight = windowSize.y - ButtonOffsetY - HorizontalMargin - (ButtonSpacing * static_cast<f32>(buttonRows - 1));
        const auto buttonWidth = availableWidth / static_cast<f32>(ButtonCols);
        const auto buttonHeight = availableHeight / static_cast<f32>(buttonRows);
        const auto buttonSize = std::min(buttonWidth, buttonHeight);
        const auto gridWidth = (buttonSize * static_cast<f32>(ButtonCols)) + (ButtonSpacing * static_cast<f32>(ButtonCols - 1));
        const auto startX = (windowSize.x - gridWidth) * 0.5f;
        return {
            .Start = {startX, ButtonOffsetY},
            .CellSize = buttonSize,
            .Spacing = ButtonSpacing,
        };
	}
}

namespace SampleUI::Screens::Landing {
	void Render() {
		ImGui::SetNextWindowPos({0.f, 0.f});
		ImGui::SetNextWindowSize({Graphics::ScreenWidth, Graphics::ScreenHeight});

		ImGui::Begin("SampleUI Landing", nullptr, BaseUiFlags);

		if(auto* headerFont = GetFont(FontSizes::H1)) {
			ImGui::PushFont(headerFont);
			ImGui::SetCursorPosY(HeaderOffsetY);
			TextCenteredX("SampleUI");
			ImGui::PopFont();
		} else {
			ImGui::SetCursorPosY(HeaderOffsetY);
			TextCenteredX("SampleUI");
		}

		auto buttonCount = static_cast<size_t>(std::ranges::count_if(GetScreenDefinitions(), [](const auto& def) {
            return def.LandingLabel != nullptr;
		}));
        auto buttonRows = (buttonCount + ButtonCols - 1) / ButtonCols;

		auto grid = BuildGrid(buttonRows);
        size_t buttonIndex = 0;

		for(const auto& screen : GetScreenDefinitions()) {
            if(!screen.LandingLabel) continue;

			auto row = buttonIndex / ButtonCols;
            auto col = buttonIndex % ButtonCols;
            auto cell = grid.GetCell(row, col);

			ImGui::SetCursorPos(cell.Min);
            ImGui::PushID(static_cast<int>(buttonIndex));
			if(ImGui::Button(screen.LandingLabel, cell.GetSize())) {
                Screens::SetActiveScreen(screen.Id);
			}
            ImGui::PopID();
            buttonIndex++;
		}

		ImGui::End();
	}
} // namespace SampleUI::Screens::Landing
