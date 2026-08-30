#pragma once

#include "SampleUI/Screens/Screens.h"

#include <Platform/Graphics.h>
#include <Ui/Panel/Panel.h>
#include <Ui/UiUtil.h>
#include <Ui/UiGeometry.h>

#include <imgui.h>

namespace SampleUI {
	template<typename TRender>
	void RenderSampleScreen(const char* title, const TRender& render) {
        ImGui::SetNextWindowPos(Ui::Zero);
        ImGui::SetNextWindowSize({Graphics::ScreenWidth, Graphics::ScreenHeight});

		ImGui::Begin(
            title, nullptr, BaseUiFlags | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
        );

        if(ImGui::Button("Back")) {
            Screens::SetActiveScreen(Screen::Landing);
            ImGui::End();
            return;
        }

        ImGui::SetCursorPosY(32.f);
        if(auto* headerFont = GetFont(FontSizes::H1)) {
            ImGui::PushFont(headerFont);
            TextCenteredX(title);
            ImGui::PopFont();
        } else {
            TextCenteredX(title);
        }

        render();

        ImGui::End();
	}

    inline void RenderRemainingPanel(Ui::Panel& panel, f32 topMargin) {
        auto panelTop = ImGui::GetCursorPosY() + topMargin;
        auto bounds = Ui::UiRect{{0.f, panelTop}, {Graphics::ScreenWidth, Graphics::ScreenHeight}};
        panel.SetBounds(bounds);
        panel.Render();
    }
}