#include "SampleUI/Screens/SampleSimpleMap.h"
#include "SampleUI/Screens/Screens.h"

#include <World/World.h>
#include <Ui/UiUtil.h>
#include <Ui/Panel/CanvasPanel.h>
#include <Ui/Panel/ZoomFunc.h>
#include <Platform/Graphics.h>
#include <Platform/NumTypes.h>

#include <imgui.h>
#include <vector>
#include <memory>

namespace {
    constexpr auto HeaderOffsetY = 32.f;
    constexpr auto ControlsOffsetY = 92.f;
    constexpr auto CanvasTopMargin = 8.f;
    constexpr auto CellSize = 24.f;

    Ui::PanelConfig PanelConfig{};

    enum struct TerrainType : u8 {
        Floor, Wall, Water, Road
    };
    enum struct ChunkSize : u8 {
        Small, Medium, Large
    };
    int SelectedChunkSize = static_cast<int>(ChunkSize::Small);

    constexpr ImU32 TerrainToColor(TerrainType type) {
        switch(type) {
            using enum TerrainType;
        case Floor: return IM_COL32(200, 150, 150, 255);
        case Wall: return IM_COL32(20, 15, 15, 255);
        case Water: return IM_COL32(0, 0, 200, 255);
        case Road: return IM_COL32(150, 75, 0, 255);
        }
        return IM_COL32(0, 0, 0, 255);
    }

    //using Map = std::vector<std::vector<SimpleChunk>>;

    template<typename ChunkType>
    constexpr ChunkType GenerateChunk() { 
        using namespace World;
        struct Rect {
            Coord Tl;
            Coord Br;
        };

        ChunkType result = {};
        auto Fill = [&](Coord topLeft, Coord bottomRight, TerrainType t) {
            for(size_t row = topLeft.Y; row <= bottomRight.Y; row++) {
                for(size_t col = topLeft.X; col <= bottomRight.X; col++) {
                    result.Cells.at(row).at(col) = t;
                }
            }
        };
        auto FillRect = [&](Rect r, TerrainType t) { Fill(r.Tl, r.Br, t); };

        Fill({0, 0}, {ChunkType::Width - 1, ChunkType::Height - 1}, TerrainType::Wall);
        Fill({1, 1}, {ChunkType::Width - 2, ChunkType::Height - 2}, TerrainType::Water);

        auto thirdWidth = static_cast<s32>(ChunkType::Width / 3);
        auto thirdHeight = static_cast<s32>(ChunkType::Height / 3);
        auto roomWidth = thirdWidth - 2;
        auto roomHeight = thirdHeight - 2;

        auto MakeRoom = [&](Coord center) {
            auto topLeft = Coord{center.X - roomWidth / 2, center.Y - roomHeight / 2};
            auto bottomRight = Coord{center.X + roomWidth / 2, center.Y + roomHeight / 2};
            return Rect{topLeft, bottomRight};
        };

        auto MakeRoad = [&](Rect start, Rect end) {
            if(start.Tl.X == end.Tl.X) {
                auto mid = start.Tl.X + (start.Br.X - start.Tl.X) / 2;
                Fill({ mid, start.Br.Y}, {mid, end.Tl.Y}, TerrainType::Road);
            } else if(start.Tl.Y == end.Tl.Y) {
                auto mid = start.Tl.Y + (start.Br.Y - start.Tl.Y) / 2;
                Fill({start.Br.X, mid}, {end.Tl.X, mid}, TerrainType::Road);
            }
        };

        auto TlRoom = MakeRoom({thirdWidth, thirdHeight});
        auto TrRoom = MakeRoom({thirdWidth * 2, thirdHeight});

        FillRect(TlRoom, TerrainType::Floor);
        FillRect(TrRoom, TerrainType::Floor);

        auto BlRoom = MakeRoom({thirdWidth, thirdHeight * 2});
        auto BrRoom = MakeRoom({thirdWidth * 2, thirdHeight * 2});
        FillRect(BlRoom, TerrainType::Floor);
        FillRect(BrRoom, TerrainType::Floor);

        MakeRoad(TlRoom, TrRoom);
        MakeRoad(TlRoom, BlRoom);
        MakeRoad(TrRoom, BrRoom);
        MakeRoad(BlRoom, BrRoom);

        return result;
    }

    static constexpr auto SmallChunk = GenerateChunk<World::Chunk<TerrainType, 16, 16>>();
    static constexpr auto MediumChunk = GenerateChunk<World::Chunk<TerrainType, 32, 32>>();
    static constexpr auto LargeChunk = GenerateChunk<World::Chunk<TerrainType, 64, 64>>();

}

namespace SampleUI::Screens::SampleSimpleMap {
    std::unique_ptr<Ui::CanvasPanel> Panel{nullptr};

    bool Initialize() { 
        PanelConfig.ZoomFn = Ui::Zoom::Exponential<f32, 1.1f>;
        PanelConfig.BackgroundColor = IM_COL32(32, 32, 32, 255);

        Panel = std::make_unique<Ui::CanvasPanel>(PanelConfig, [](Ui::CanvasPanel& canvas) {
            auto renderChunk = [&](const auto& chunk) {
                for(size_t row = 0; row < chunk.Height; row++) {
                    for(size_t col = 0; col < chunk.Width; col++) {
                        auto cellType = chunk.Cells.at(row).at(col);
                        auto color = TerrainToColor(cellType);
                        auto x = static_cast<f32>(col) * CellSize;
                        auto y = static_cast<f32>(row) * CellSize;
                        auto topLeft = ImVec2(x, y);
                        auto bottomRight = ImVec2(x + CellSize, y + CellSize);

                        auto contentTl = ImVec2{static_cast<f32>(col) * CellSize, static_cast<f32>(row) * CellSize};
                        auto contentBr = ImVec2{contentTl.x + CellSize, contentTl.y + CellSize};
                        auto screenTl = canvas.ContentToScreen(contentTl);
                        auto screenBr = canvas.ContentToScreen(contentBr);
                        ImGui::GetWindowDrawList()->AddRectFilled(screenTl, screenBr, color);
                        // border
                        ImGui::GetWindowDrawList()->AddRect(screenTl, screenBr, IM_COL32(0, 0, 0, 255), 0.f, ImDrawFlags_None, 1.f);
                    }
                }
            };
            switch(SelectedChunkSize) {
                case 0: renderChunk(SmallChunk); break;
                case 1: renderChunk(MediumChunk); break;
                case 2: renderChunk(LargeChunk); break;
                default: break;
            }
        });

        return true; 
    }

    void ShutDown() {
        Panel.reset();
    }

    void Render() {
        ImGui::SetNextWindowPos({0.f, 0.f});
        ImGui::SetNextWindowSize({Graphics::ScreenWidth, Graphics::ScreenHeight});
        ImGui::Begin("Sample Simple Map", nullptr, BaseUiFlags);

        if(ImGui::Button("Back")) {
            Screens::SetActiveScreen(Screen::Landing);
            ImGui::End();
            return;
        }

        ImGui::SetCursorPosY(HeaderOffsetY);
        ImGui::PushFont(GetFont(FontSizes::H1));
        TextCenteredX("Sample Simple Map");
        ImGui::PopFont();

        ImGui::PushFont(GetFont(FontSizes::H4));
        ImGui::SetCursorPosY(ControlsOffsetY);

        const char* sizeLabels = "Small\0Medium\0Large";
        ImGui::Combo("Chunk Size", &SelectedChunkSize, sizeLabels, 3);

        //ImGui::SliderInt2("World Size", s_WorldSize.Data(), 1, 32);

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
} // namespace SampleUI::Screens::SampleSimpleMap