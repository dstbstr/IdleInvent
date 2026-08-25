#include "SampleUI/Screens/SampleCombat.h"
#include "SampleUI/Screens/Screens.h"

#include <Combat/CombatTypes.h>
#include <Manage/TickManager.h>
#include <Platform/Graphics.h>
#include <Platform/NumTypes.h>
#include <Ui/UiGeometry.h>
#include <Ui/Overlay.h>
#include <Ui/Panel/CanvasPanel.h>
#include <Ui/UiUtil.h>
#include <Utilities/Handle.h>

#include <imgui.h>

#include <cmath>
#include <map>
#include <memory>
#include <vector>

namespace {
    struct CombatantVisual {
        Combat::CombatantId Id{};
        ImVec2 Home{};
        ImVec2 Current{};
        ImU32 Color{};
    };

    struct SampleCombatant {
        std::string Name;
        s32 Hp{};
        s32 MaxHp{};
        s32 Attack{};
    };

    enum struct SampleActionKind : u8 { Attack, Pass };
    struct SampleAction {
        SampleActionKind Kind{};
        Combat::CombatantId Target{};
    };

    constexpr auto HeaderOffsetY = 32.f;
    constexpr auto ControlsOffsetY = 92.f;
    constexpr auto CanvasTopMargin = 8.f;
    
    auto BattlefieldPercent = 0.7f;
    auto StatsPercent = 2.f / 3.f;
    ImVec4 SelectedBattleColor = {0.f, .06f, 0.7f, 1.f};
    ImVec4 SelectedControlColor = {0.f, .06f, 0.28f, 1.f};

    Ui::PanelConfig BattlePanelConfig{};

    void RenderBattlefield(Ui::CanvasPanel& canvas) {}
}

namespace SampleUI::Screens::SampleCombat {
    std::unique_ptr<Ui::CanvasPanel> BattlePanel{nullptr};
    std::vector<ScopedHandle> Handles{};

    bool Initialize() { 
        BattlePanelConfig.BackgroundColor = ImGui::ColorConvertFloat4ToU32(SelectedBattleColor);
        BattlePanel = std::make_unique<Ui::CanvasPanel>(BattlePanelConfig, [](Ui::CanvasPanel& canvas) {
            RenderBattlefield(canvas);
        });

        return true; 
    }
    void ShutDown() {
        BattlePanel.reset();
        Handles.clear();
    }

    void Render() {
        ImGui::SetNextWindowPos({0.f, 0.f});
        ImGui::SetNextWindowSize({Graphics::ScreenWidth, Graphics::ScreenHeight});
        ImGui::Begin("Sample Combat", nullptr, BaseUiFlags);

        if(ImGui::Button("Back")) {
            Screens::SetActiveScreen(Screen::Landing);
            ImGui::End();
            return;
        }

        ImGui::SetCursorPosY(HeaderOffsetY);
        ImGui::PushFont(GetFont(FontSizes::H1));
        TextCenteredX("Sample Combat");
        ImGui::PopFont();

        ImGui::PushFont(GetFont(FontSizes::H4));
        ImGui::SetCursorPosY(ControlsOffsetY);

        ImGui::SliderFloat("Battlefield Height", &BattlefieldPercent, 0.1f, 0.9f);
        ImGui::SliderFloat("Stats Width", &StatsPercent, 0.1f, 0.9f);
        if(ImGui::ColorEdit4("Background Color",&SelectedBattleColor.x, ImGuiColorEditFlags_NoInputs)) {
            if(BattlePanel) {
                BattlePanel->SetBackgroundColor(ImGui::ColorConvertFloat4ToU32(SelectedBattleColor));
            }
        }
        ImGui::SameLine();
        ImGui::ColorEdit4("Control Color", &SelectedControlColor.x, ImGuiColorEditFlags_NoInputs);

        ImGui::PopFont();

        auto width = static_cast<f32>(Graphics::ScreenWidth);

        auto contentTop = ImGui::GetCursorPosY() + CanvasTopMargin;
        auto contentBottom = static_cast<f32>(Graphics::ScreenHeight);
        auto contentHeight = contentBottom - contentTop;

        auto battlefieldBottom = contentTop + contentHeight * BattlefieldPercent;
        auto statsRight = width * StatsPercent;

        auto battlefieldBounds = Ui::UiRect{{0.f, contentTop}, {width, battlefieldBottom}};
        auto statsBounds = Ui::UiRect{{0.f, battlefieldBottom}, {statsRight, contentBottom}};
        auto actionsBounds = Ui::UiRect{{statsRight, battlefieldBottom}, {width, contentBottom}};

        auto RenderChild = [](const char* id, Ui::UiRect bounds, const auto& render) {
            ImGui::SetCursorPos(bounds.Min);
            if(ImGui::BeginChild(id, bounds.GetSize(), ImGuiChildFlags_Borders)) {
                ImGui::GetWindowDrawList()->AddRectFilled(bounds.Min, bounds.Max, ImGui::ColorConvertFloat4ToU32(SelectedControlColor));
                render();
            }
            ImGui::EndChild();
        };

        if(BattlePanel) {
            BattlePanel->SetBounds(battlefieldBounds);
            BattlePanel->Render();
        }
        RenderChild("##CombatStats", statsBounds, [] {
            ImGui::TextUnformatted("Party Stats");
            ImGui::Separator();
            ImGui::TextUnformatted("Hero        HP 10 / 10");
            ImGui::TextUnformatted("Mage        HP  7 /  7");
        });
        RenderChild("##CombatActions", actionsBounds, [] {
            ImGui::TextUnformatted("Actions");
            ImGui::Separator();
            ImGui::Button("Attack");
            ImGui::Button("Pass");
        });

        ImGui::End();

    }
} // namespace SampleUI::Screens::SampleCombat
