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

#include <array>
#include <cmath>
#include <map>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>

namespace {
    constexpr auto HeaderOffsetY = 32.f;
    constexpr auto ControlsOffsetY = 92.f;
    constexpr auto CanvasTopMargin = 8.f;
    constexpr auto CombatantSize = 56.f;

    struct CombatantVisual {
        Combat::CombatantId Id{};
        ImVec2 CurrentLocal{};
        ImU32 Color{};
        f32 Size{CombatantSize};
    };

    struct SampleCombatant {
        std::string Name;
        s32 Hp{};
        s32 MaxHp{};
        s32 Attack{};
    };

    enum struct FormationFront : u8 { Left, Right, Top, Bottom };
    
    auto BattlefieldPercent = 0.7f;
    auto StatsPercent = 2.f / 3.f;
    ImVec4 SelectedBattleColor = {0.f, .06f, 0.7f, 1.f};
    ImVec4 SelectedControlColor = {0.f, .06f, 0.28f, 1.f};
    ImVec4 ControlBorderColor = {1.f, 1.f, 1.f, 1.f};
    auto ControlBorderThickness = 2.f;
    auto ControlBorderRounding = 12.f;
    auto ControlPanelGap = 2.f;

    auto CombatantPadding = 0.2f;
    auto GroupVertical = true;
    auto SelectedCombatant = 0;

    Ui::PanelConfig BattlePanelConfig{};
    std::vector<CombatantVisual> PlayerVisuals{};
    std::vector<CombatantVisual> EnemyVisuals{};
    Ui::UiRect PlayerRegion;
    Ui::UiRect EnemyRegion;
    u32 NextVisualId{};

    Combat::CombatantId AddVisual(std::vector<CombatantVisual>& visuals, ImU32 color) {
        auto id = Combat::CombatantId{NextVisualId++};
        visuals.push_back({.Id = id, .Color = color});
        return id;
    }

    void LayoutGrid(std::span<CombatantVisual> visuals, Ui::UiRect region, FormationFront front) {
        if(visuals.empty()) return;

        auto count = visuals.size();
        auto lineCapacity = static_cast<size_t>(std::ceil(std::sqrt(static_cast<f32>(count))));
        auto verticalFront = front == FormationFront::Left || front == FormationFront::Right;

        size_t rows{}, cols{};
        if(verticalFront) {
            rows = lineCapacity;
            cols = (count + rows - 1) / rows;
        } else {
            cols = lineCapacity;
            rows = (count + cols - 1) / cols;
        }
        auto fRows = static_cast<f32>(rows);
        auto fCols = static_cast<f32>(cols);

        auto regionSize = region.GetSize();
        auto slotSize = std::min(regionSize.x / fCols, regionSize.y / fRows);
        auto formationSize = ImVec2{slotSize * fCols, slotSize * fRows};
        auto origin = region.GetCenter() - (formationSize * 0.5f);

        switch(front) {
            using enum FormationFront;
            case Left: origin.x = region.Min.x; break;
            case Right: origin.x = region.Max.x - formationSize.x; break;
            case Top: origin.y = region.Min.y; break;
            case Bottom: origin.y = region.Max.y - formationSize.y; break;
        }

        for(size_t index = 0; index < count; index++) {
            ImVec2 home{};
            if(verticalFront) {
                auto line = index / rows;
                auto entry = index % rows;
                auto entriesInLine = std::min(rows, count - line * rows);
                auto col = front == FormationFront::Right ? line : cols - 1 - line;
                auto vOff = static_cast<f32>(rows - entriesInLine) * slotSize * 0.5f;
                home = {
                    origin.x + (static_cast<f32>(col) + 0.5f) * slotSize, 
                    origin.y + vOff + (static_cast<f32>(entry) + 0.5f) * slotSize
                };
            } else {
                auto line = index / cols;
                auto entry = index % cols;
                auto entriesInLine = std::min(cols, count - line * cols);
                auto row = front == FormationFront::Bottom ? line : rows - 1 - line;
                auto hOff = static_cast<f32>(cols - entriesInLine) * slotSize * 0.5f;
                home = {
                    origin.x + hOff + (static_cast<f32>(entry) + 0.5f) * slotSize,
                    origin.y + (static_cast<f32>(row) + 0.5f) * slotSize
                };
            }

            auto& visual = visuals[index];
            visual.CurrentLocal = home;
            visual.Size = std::min(CombatantSize, slotSize * 0.8f);
        }
    }

    void DrawCombatant(const Ui::CanvasPanel& canvas, const CombatantVisual& visual, bool selected) {
        auto halfSize = Ui::Half * visual.Size;
        auto localBounds = Ui::UiRect{visual.CurrentLocal - halfSize, visual.CurrentLocal + halfSize};
        auto screenBounds = canvas.LocalToScreen(localBounds);
        auto* drawList = ImGui::GetWindowDrawList();

        drawList->AddRectFilled(screenBounds.Min, screenBounds.Max, visual.Color);
        drawList->AddRect(screenBounds.Min, screenBounds.Max, IM_COL32_BLACK);
        if(!selected) return;

        auto centerX = screenBounds.CenterX();
        auto triangleBottom = screenBounds.Min.y - 8.f;
        auto triangleTop = triangleBottom - 12.f;
        drawList->AddTriangleFilled(
            {centerX, triangleBottom}, 
            {centerX - 7.f, triangleTop}, 
            {centerX + 7.f, triangleTop}, 
            IM_COL32_WHITE
        );
    }

    void RenderBattlefield(Ui::CanvasPanel& canvas) { 
        auto size = canvas.GetViewportSize();
        if(GroupVertical) {
            auto gap = size.x * CombatantPadding;
            auto leftFront = (size.x - gap) * 0.5f;
            auto rightFront = (size.x + gap) * 0.5f;

            PlayerRegion = {{0.f, 0.f}, {leftFront, size.y}};
            EnemyRegion = {{rightFront, 0.f}, {size.x, size.y}};
            LayoutGrid(PlayerVisuals, PlayerRegion, FormationFront::Right);
            LayoutGrid(EnemyVisuals, EnemyRegion, FormationFront::Left);
        } else {
            auto gap = size.y * CombatantPadding;
            auto topFront = (size.y - gap) * 0.5f;
            auto bottomFront = (size.y + gap) * 0.5f;
            PlayerRegion = {{0.f, 0.f}, {size.x, topFront}};
            EnemyRegion = {{0.f, bottomFront}, {size.x, size.y}};
            LayoutGrid(PlayerVisuals, PlayerRegion, FormationFront::Bottom);
            LayoutGrid(EnemyVisuals, EnemyRegion, FormationFront::Top);
        }

        auto selected = Combat::CombatantId{static_cast<u32>(SelectedCombatant)};
        for(const auto& visual : PlayerVisuals) {
            DrawCombatant(canvas, visual, visual.Id == selected);
        }
        for(const auto& visual : EnemyVisuals) { 
            DrawCombatant(canvas, visual, visual.Id == selected); 
        }
    }
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
        ImGui::Begin("Sample Combat", nullptr, BaseUiFlags | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

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
        
        ImGui::SliderFloat("Control Border Thickness", &ControlBorderThickness, 0.f, 8.f);
        ImGui::SliderFloat("Control Border Rounding", &ControlBorderRounding, 0.f, 30.f);
        ImGui::SliderFloat("Control Panel Gap", &ControlPanelGap, 0.f, 16.f);

        if(ImGui::ColorEdit4("Background Color",&SelectedBattleColor.x, ImGuiColorEditFlags_NoInputs)) {
            if(BattlePanel) {
                BattlePanel->SetBackgroundColor(ImGui::ColorConvertFloat4ToU32(SelectedBattleColor));
            }
        }
        ImGui::SameLine();
        ImGui::ColorEdit4("Control Color", &SelectedControlColor.x, ImGuiColorEditFlags_NoInputs);
        ImGui::SameLine();
        ImGui::ColorEdit4("Control Border Color", &ControlBorderColor.x, ImGuiColorEditFlags_NoInputs);

        ImGui::SliderFloat("Combatant Padding", &CombatantPadding, 0.1f, 0.4f);
        
        ImGui::Checkbox("Group Vertically", &GroupVertical);
        ImGui::SameLine();
        ImGui::SliderInt("Selected Combatant", &SelectedCombatant, 0, NextVisualId - 1);

        ImGui::TextUnformatted("Allys");
        ImGui::SameLine();
        ImGui::PushID("AllyControls");
        if(ImGui::SmallButton("+")) {
            AddVisual(PlayerVisuals, IM_COL32(0, 255, 0, 255));
        }
        ImGui::SameLine();
        ImGui::BeginDisabled(PlayerVisuals.empty());
        if(ImGui::SmallButton("-")) {
            if(!PlayerVisuals.empty()) {
                PlayerVisuals.pop_back();
            }
        }
        ImGui::EndDisabled();
        ImGui::PopID();

        ImGui::TextUnformatted("Enemies");
        ImGui::SameLine();
        ImGui::PushID("EnemyControls");
        if(ImGui::SmallButton("+")) {
            AddVisual(EnemyVisuals, IM_COL32(255, 0, 0, 255));
        }
        ImGui::SameLine();
        ImGui::BeginDisabled(EnemyVisuals.empty());
        if(ImGui::SmallButton("-")) {
            if(!EnemyVisuals.empty()) {
                EnemyVisuals.pop_back();
            }
        }
        ImGui::EndDisabled();
        ImGui::PopID();

        ImGui::PopFont();

        auto width = Graphics::ScreenWidth;
        auto height = Graphics::ScreenHeight;

        auto contentTop = ImGui::GetCursorPosY() + CanvasTopMargin;
        auto contentHeight = height - contentTop;

        auto battlefieldBottom = contentTop + contentHeight * BattlefieldPercent;
        auto battlefieldBounds = Ui::UiRect{{0.f, contentTop}, {width, battlefieldBottom}};

        auto statsRight = width * StatsPercent;
        auto halfGap = ControlPanelGap * 0.5f;
        auto halfBorder = ControlBorderThickness * 0.5f;

        auto statsBounds = Ui::UiRect{{halfBorder, battlefieldBottom}, {statsRight - halfGap, height}};
        auto actionsBounds = Ui::UiRect{{statsRight + halfGap, battlefieldBottom}, {width - halfBorder, height}};

        auto RenderChild = [](const char* id, Ui::UiRect bounds, const auto& render) {
            ImGui::SetCursorPos(bounds.Min);

            ImGui::PushStyleColor(ImGuiCol_ChildBg, SelectedControlColor);
            ImGui::PushStyleColor(ImGuiCol_Border, ControlBorderColor);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, ControlBorderThickness);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, ControlBorderRounding);

            if(ImGui::BeginChild(id, bounds.GetSize(), ImGuiChildFlags_Borders)) {
                render();
            }

            ImGui::EndChild();
            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(2);
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
