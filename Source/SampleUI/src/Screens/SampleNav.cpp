#include "SampleUI/Screens/SampleNav.h"
#include "SampleUI/Screens/Screens.h"

#include <Utilities/IRandom.h>
#include <Platform/Graphics.h>
#include <Platform/NumTypes.h>
#include <Ui/Panel/CanvasPanel.h>
#include <Ui/Panel/ZoomFunc.h>
#include <Ui/UiUtil.h>
#include <World/World.h>

#include <imgui.h>
#include <memory>
#include <vector>

namespace {
    constexpr auto HeaderOffsetY = 32.f;
    constexpr auto ControlsOffsetY = 92.f;
    constexpr auto CanvasTopMargin = 8.f;
    constexpr auto CellSize = 24.f;

    Ui::PanelConfig PanelConfig{};
    s32 WorldSeed = 42;
    f32 FloorPercent = 0.7f;
    f32 WaterPercent = 0.2f;

    enum struct TerrainType : u8 { Floor, Wall, Water };

    constexpr ImU32 TerrainToColor(TerrainType type) {
        switch(type) {
            using enum TerrainType;
        case Floor: return IM_COL32(200, 150, 150, 255);
        case Wall: return IM_COL32(20, 15, 15, 255);
        case Water: return IM_COL32(0, 0, 200, 255);
        }
        return IM_COL32(0, 0, 0, 255);
    }

    using Map = World::Chunk<TerrainType, 32, 32>;
    Map GenerateMap() {
        auto rand = SeededRandom(static_cast<u32>(WorldSeed));
        Map map = {};
        for(size_t row = 0; row < Map::Height; ++row) {
            for(size_t col = 0; col < Map::Width; ++col) {
                if(row == 0 || col == 0 || row == Map::Height - 1 || col == Map::Width - 1) {
                    map.Cells.at(row).at(col) = TerrainType::Wall;
                } else if((row < 3 && col < 3) || (row > Map::Height - 4 && col > Map::Width - 4)) {
                    map.Cells.at(row).at(col) = TerrainType::Floor;
                } else {
                    auto r = rand.GetNextFloat();
                    auto type = TerrainType::Wall;
                    if(r < FloorPercent) {
                        type = TerrainType::Floor;
                    } else if(r < FloorPercent + WaterPercent) {
                        type = TerrainType::Water;
                    }

                    map.Cells.at(row).at(col) = type;
                }
            }
        }

        return map;
    }

    static auto WorldMap = GenerateMap();

} // namespace

namespace SampleUI::Screens::SampleNav {
    std::unique_ptr<Ui::CanvasPanel> Panel{nullptr};

    bool Initialize() {
        PanelConfig.ZoomFn = Ui::Zoom::Exponential<f32, 1.1f>;
        PanelConfig.BackgroundColor = IM_COL32(255, 255, 255, 255);

        Panel = std::make_unique<Ui::CanvasPanel>(PanelConfig, [](Ui::CanvasPanel& canvas) {
            for(size_t row = 0; row < WorldMap.Height; row++) {
                for(size_t col = 0; col < WorldMap.Width; col++) {
                    auto cellType = WorldMap.Cells.at(row).at(col);
                    auto color = TerrainToColor(cellType);

                    auto contentTl = ImVec2{static_cast<f32>(col) * CellSize, static_cast<f32>(row) * CellSize};
                    auto contentBr = ImVec2{contentTl.x + CellSize, contentTl.y + CellSize};
                    auto screenTl = canvas.ContentToScreen(contentTl);
                    auto screenBr = canvas.ContentToScreen(contentBr);
                    ImGui::GetWindowDrawList()->AddRectFilled(screenTl, screenBr, color);
                    // border
                    ImGui::GetWindowDrawList()->AddRect(
                        screenTl, screenBr, IM_COL32(0, 0, 0, 255), 0.f, ImDrawFlags_None, 1.f
                    );
                }
            }
        });

        return true;
    }

    void ShutDown() { Panel.reset(); }

    void Render() {
        ImGui::SetNextWindowPos({0.f, 0.f});
        ImGui::SetNextWindowSize({Graphics::ScreenWidth, Graphics::ScreenHeight});
        ImGui::Begin("Sample Navigation", nullptr, BaseUiFlags);

        if(ImGui::Button("Back")) {
            Screens::SetActiveScreen(Screen::Landing);
            ImGui::End();
            return;
        }

        ImGui::SetCursorPosY(HeaderOffsetY);
        ImGui::PushFont(GetFont(FontSizes::H1));
        TextCenteredX("Sample Navigation");
        ImGui::PopFont();

        ImGui::PushFont(GetFont(FontSizes::H4));
        ImGui::SetCursorPosY(ControlsOffsetY);

        bool changed = false;

        auto originalFloor = FloorPercent;
        auto originalWater = WaterPercent;

        changed |= ImGui::SliderInt("World Seed", &WorldSeed, 0, 9999);
        changed |= ImGui::SliderFloat("Floors", &FloorPercent, 0.f, 1.f);
        changed |= ImGui::SliderFloat("Water", &WaterPercent, 0.f, 1.f);

        if(FloorPercent + WaterPercent > 1.f) {
            FloorPercent = originalFloor;
            WaterPercent = originalWater;
        }

        if(ImGui::Button("Regenerate")) {
            WorldSeed = (WorldSeed + 32) % 10'000;
            changed = true;
        }

        if(changed) {
            WorldMap = GenerateMap();
        }
        ImGui::PopFont();

        // Place the canvas in the remaining content area below the controls. Sliders and
        // buttons live above canvasTop; the canvas owns everything from there to the bottom.
        const auto canvasTop = ImGui::GetCursorPosY() + CanvasTopMargin;
        const Ui::Rect canvasBounds{
            ImVec2{0.f, canvasTop}, ImVec2{Graphics::ScreenWidth, Graphics::ScreenHeight - canvasTop}
        };

        if(Panel) {
            Panel->SetBounds(canvasBounds);
            Panel->Render();
        }

        ImGui::End();
    }
} // namespace SampleUI::Screens::SampleNav