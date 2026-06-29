#include <SampleUI/Screens/Landing.h>
#include <SampleUI/Screens/Screens.h>

#include <Platform/Graphics.h>
#include <Ui/UiUtil.h>

#include <algorithm>
#include <array>
#include <imgui.h>

namespace {
	constexpr size_t ButtonCols = 3;
	constexpr size_t ButtonRows = 4;
	constexpr f32 ButtonSpacing = 24.f;
	constexpr f32 HorizontalMargin = 24.f;
	constexpr auto HeaderOffsetY = 48.f;
	constexpr auto ButtonOffsetY = 140.f;

	struct LandingButton {
		const char* Label;
		SampleUI::Screen Target;
	};

	constexpr std::array<LandingButton, ButtonCols * ButtonRows> Buttons{{
		{"Tree\nPanel", SampleUI::Screen::SampleTreePanel},
		{"Particles", SampleUI::Screen::SampleParticles},
		{"Unused", SampleUI::Screen::Landing},
		{"Unused", SampleUI::Screen::Landing},
		{"Unused", SampleUI::Screen::Landing},
		{"Unused", SampleUI::Screen::Landing},
		{"Unused", SampleUI::Screen::Landing},
		{"Unused", SampleUI::Screen::Landing},
		{"Unused", SampleUI::Screen::Landing},
		{"Unused", SampleUI::Screen::Landing},
		{"Unused", SampleUI::Screen::Landing},
		{"Unused", SampleUI::Screen::Landing},
	}};

	struct Rect {
        ImVec2 Pos;
        ImVec2 Size;
	};

	struct GridLayout {
        ImVec2 WindowSize{};
        ImVec2 Start{};
        f32 CellSize{};
        f32 Spacing{};
        size_t ColCount{};
        size_t RowCount{};

		[[nodiscard]] constexpr Rect GetCell(size_t row, size_t col) const {
			return { 
				.Pos = {
					Start.x + static_cast<f32>(col) * (CellSize + Spacing), 
					Start.y + static_cast<f32>(row) * (CellSize + Spacing)
				},
                .Size = {CellSize, CellSize} };
		}
        [[nodiscard]] constexpr size_t ToIndex(size_t row, size_t col) const { return row * ColCount + col; }
	};

	[[nodiscard]] GridLayout BuildGrid() {
        const auto windowSize = ImGui::GetWindowSize();
        const auto availableWidth =
            windowSize.x - (HorizontalMargin * 2.f) - (ButtonSpacing * static_cast<f32>(ButtonCols - 1));
        const auto availableHeight =
            windowSize.y - ButtonOffsetY - HorizontalMargin - (ButtonSpacing * static_cast<f32>(ButtonRows - 1));
        const auto buttonWidth = availableWidth / static_cast<f32>(ButtonCols);
        const auto buttonHeight = availableHeight / static_cast<f32>(ButtonRows);
        const auto buttonSize = std::min(buttonWidth, buttonHeight);
        const auto gridWidth =
            (buttonSize * static_cast<f32>(ButtonCols)) + (ButtonSpacing * static_cast<f32>(ButtonCols - 1));
        const auto startX = (windowSize.x - gridWidth) * 0.5f;
        return {
            .WindowSize = windowSize,
            .Start = {startX, ButtonOffsetY},
            .CellSize = buttonSize,
            .Spacing = ButtonSpacing,
            .ColCount = ButtonCols,
            .RowCount = ButtonRows
        };
	}
}

namespace SampleUI::Screens::Landing {
	bool Initialize() {
		return true;
	}

	void ShutDown() {}

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

		auto grid = BuildGrid();

		for(size_t row = 0; row < ButtonRows; ++row) {
			for(size_t col = 0; col < ButtonCols; ++col) {
                auto cell = grid.GetCell(row, col);
                auto index = grid.ToIndex(row, col);
				const auto& button = Buttons.at(index);
				ImGui::SetCursorPos(cell.Pos);
                ImGui::PushID(static_cast<int>(index));
				if(ImGui::Button(button.Label, cell.Size)) {
					Screens::SetActiveScreen(button.Target);
				}
                ImGui::PopID();
			}
		}

		ImGui::End();
	}
} // namespace SampleUI::Screens::Landing
