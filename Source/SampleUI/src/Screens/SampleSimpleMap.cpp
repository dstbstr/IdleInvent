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
    f32 RoomWidth = 0.3;
    f32 RoomHeight = 0.3;
    f32 MoatWidth = 0.1;
    f32 RoadWidth = 0.25;

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

    template<typename ChunkType>
    constexpr ChunkType GenerateChunk() { 
        using namespace World;
        struct Rect {
            Coord Tl;
            Coord Br;
            constexpr auto operator<=>(const Rect&) const = default;
            constexpr Rect Expand(s32 dX = 1, s32 dY = 1) const {
                return Rect{Tl + Coord{-dX, -dY}, Br + Coord{dX, dY}};
            }
            constexpr Rect Contract(s32 dX = 1, s32 dY = 1) const {
                return Rect{Tl + Coord{dX, dY}, Br + Coord{-dX, -dY}};
            }
        };

        ChunkType result = {};
        auto fullArea = Rect{Coord{0, 0}, Coord{ChunkType::Width - 1, ChunkType::Height - 1}};
        auto waterArea = fullArea.Contract();

        auto Fill = [&](Coord topLeft, Coord bottomRight, TerrainType t) {
            auto startX = std::max(fullArea.Tl.X, topLeft.X);
            auto startY = std::max(fullArea.Tl.Y, topLeft.Y);
            auto endX = std::min(fullArea.Br.X, bottomRight.X);
            auto endY = std::min(fullArea.Br.Y, bottomRight.Y);
            if(startX > endX || startY > endY) return;

            for(size_t row = startY; row <= endY; row++) {
                for(size_t col = startX; col <= endX; col++) {
                    result.Cells.at(row).at(col) = t;
                }
            }
        };

        auto FillRect = [&](Rect r, TerrainType t) { Fill(r.Tl, r.Br, t); };

        auto roomWidth = std::max(1, static_cast<s32>(ChunkType::Width * RoomWidth));
        auto roomHeight = std::max(1, static_cast<s32>(ChunkType::Height * RoomHeight));
        auto moatWidth = static_cast<s32>(ChunkType::Width * MoatWidth);

        auto MakeRoom = [&](Coord center) {
            auto topLeft = Coord{center.X - roomWidth / 2, center.Y - roomHeight / 2};
            auto bottomRight = Coord{center.X + roomWidth / 2, center.Y + roomHeight / 2};
            auto room = Rect{topLeft, bottomRight};
            FillRect(room, TerrainType::Wall);
            auto floor = room.Contract();
            FillRect(floor, TerrainType::Floor);
            return room;
        };

        auto MakeRoad = [&](Rect start, Rect end) {
            if(RoadWidth <= 0.f) return;
            if(start.Tl.X > end.Br.X || start.Tl.Y > end.Br.Y) {
                std::swap(start, end);
            }

            if(start.Tl.X == end.Tl.X) {
                auto roadWidth = static_cast<s32>(static_cast<f32>(roomWidth) * RoadWidth);
                auto halfRoad = roadWidth / 2;
                auto mid = start.Tl.X + (start.Br.X - start.Tl.X) / 2;
                Fill({ mid - halfRoad, start.Br.Y}, {mid + halfRoad, end.Tl.Y}, TerrainType::Road);
            } else if(start.Tl.Y == end.Tl.Y) {
                auto roadWidth = static_cast<s32>(static_cast<f32>(roomHeight) * RoadWidth);
                auto halfRoad = roadWidth / 2;
                auto mid = start.Tl.Y + (start.Br.Y - start.Tl.Y) / 2;
                Fill({start.Br.X, mid - halfRoad}, {end.Tl.X, mid + halfRoad}, TerrainType::Road);
            }
        };

        FillRect(fullArea, TerrainType::Wall);
        FillRect(waterArea, TerrainType::Water);

        auto halfWidth = static_cast<s32>(roomWidth / 2);
        auto halfHeight = static_cast<s32>(roomHeight / 2);
        auto leftCenterX = waterArea.Tl.X + moatWidth + halfWidth;
        auto rightCenterX = waterArea.Br.X - moatWidth - halfWidth;
        auto topCenterY = waterArea.Tl.Y + moatWidth + halfHeight;
        auto bottomCenterY = waterArea.Br.Y - moatWidth - halfHeight;

        auto TlRoom = MakeRoom({leftCenterX, topCenterY});
        auto TrRoom = MakeRoom({rightCenterX, topCenterY});
        auto BlRoom = MakeRoom({leftCenterX, bottomCenterY});
        auto BrRoom = MakeRoom({rightCenterX, bottomCenterY});

        MakeRoad(TlRoom, TrRoom);
        MakeRoad(TlRoom, BlRoom);
        MakeRoad(TrRoom, BrRoom);
        MakeRoad(BlRoom, BrRoom);

        return result;
    }

    static auto SmallChunk = GenerateChunk<World::Chunk<TerrainType, 16, 16>>();
    static auto MediumChunk = GenerateChunk<World::Chunk<TerrainType, 32, 32>>();
    static auto LargeChunk = GenerateChunk<World::Chunk<TerrainType, 64, 64>>();

}

namespace SampleUI::Screens::SampleSimpleMap {
    std::unique_ptr<Ui::CanvasPanel> Panel{nullptr};

    bool Initialize() { 
        PanelConfig.ZoomFn = Ui::Zoom::Exponential<f32, 1.1f>;
        PanelConfig.BackgroundColor = IM_COL32(255, 255, 255, 255);

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

        bool changed = false;

        changed |= ImGui::SliderFloat("Room Width", &RoomWidth, 0.01f, 0.5f);
        changed |= ImGui::SliderFloat("Room Height", &RoomHeight, 0.01f, 0.5f);
        changed |= ImGui::SliderFloat("Road Width", &RoadWidth, 0.f, 1.f);
        changed |= ImGui::SliderFloat("Moat Width", &MoatWidth, 0.f, 0.5f);

        const char* sizeLabels = "Small\0Medium\0Large";
        ImGui::Combo("Chunk Size", &SelectedChunkSize, sizeLabels, 3);

        //ImGui::SliderInt2("World Size", s_WorldSize.Data(), 1, 32);

        if(changed) {
            SmallChunk = GenerateChunk<World::Chunk<TerrainType, 16, 16>>();
            MediumChunk = GenerateChunk<World::Chunk<TerrainType, 32, 32>>();
            LargeChunk = GenerateChunk<World::Chunk<TerrainType, 64, 64>>();
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
} // namespace SampleUI::Screens::SampleSimpleMap