#include "SampleUI/Screens/SampleSimpleMap.h"
#include "SampleUI/Screens/Screens.h"

#include <World/World.h>
#include <Ui/UiGeometry.h>
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

    template<size_t TWidth, size_t THeight>
    constexpr World::Chunk<TerrainType, TWidth, THeight> GenerateChunk() { 
        using namespace World;
        using Rect = Geometry::Rect<s32, World::ChunkSpace>;
        using CellSize = Geometry::Size2<s32, World::ChunkSpace>;

        Chunk<TerrainType, TWidth, THeight> result = {};

        auto fullArea = Rect{CellCoord{0, 0}, CellCoord{TWidth, THeight}};
        auto waterArea = fullArea.Contract();

        auto Fill = [&](CellCoord topLeft, CellCoord bottomRight, TerrainType t) {
            auto startX = std::max(fullArea.Tl().X, topLeft.X);
            auto startY = std::max(fullArea.Tl().Y, topLeft.Y);
            auto endX = std::min(fullArea.Br().X, bottomRight.X);
            auto endY = std::min(fullArea.Br().Y, bottomRight.Y);
            if(startX > endX || startY > endY) return;

            for(size_t row = startY; row < endY; row++) {
                for(size_t col = startX; col < endX; col++) {
                    result.Cells.at(row).at(col) = t;
                }
            }
        };

        auto FillRect = [&](Rect r, TerrainType t) { Fill(r.Tl(), r.Br(), t); };

        auto roomWidth = std::max(1, static_cast<s32>(TWidth * RoomWidth));
        auto roomHeight = std::max(1, static_cast<s32>(THeight * RoomHeight));
        auto moatWidth = static_cast<s32>(TWidth * MoatWidth);

        auto MakeRoom = [&](CellCoord center) {
            auto topLeft = CellCoord{center.X - roomWidth / 2, center.Y - roomHeight / 2};
            auto size = CellSize{roomWidth, roomHeight};
            auto room = Rect{topLeft, size};
            FillRect(room, TerrainType::Wall);
            auto floor = room.Contract();
            FillRect(floor, TerrainType::Floor);
            return room;
        };

        auto MakeRoad = [&](Rect start, Rect end) {
            if(RoadWidth <= 0.f) return;
            if(start.Tl().X > end.Br().X || start.Tl().Y > end.Br().Y) {
                std::swap(start, end);
            }

            if(start.Tl().X == end.Tl().X) {
                auto roadWidth = std::max(1, static_cast<s32>(static_cast<f32>(roomWidth) * RoadWidth));
                auto mid = start.Tl().X + (start.Br().X - start.Tl().X) / 2;
                auto startX = mid - roadWidth / 2;
                Fill({ startX, start.Br().Y}, {startX + roadWidth, end.Tl().Y}, TerrainType::Road);
            } else if(start.Tl().Y == end.Tl().Y) {
                auto roadWidth = std::max(1, static_cast<s32>(static_cast<f32>(roomHeight) * RoadWidth));
                auto mid = start.Tl().Y + (start.Br().Y - start.Tl().Y) / 2;
                auto startY = mid - roadWidth / 2;
                Fill({start.Br().X, startY}, {end.Tl().X, startY + roadWidth}, TerrainType::Road);
            }
        };

        FillRect(fullArea, TerrainType::Wall);
        FillRect(waterArea, TerrainType::Water);

        auto halfWidth = static_cast<s32>(roomWidth / 2);
        auto halfHeight = static_cast<s32>(roomHeight / 2);
        auto leftCenterX = waterArea.Tl().X + moatWidth + halfWidth;
        auto rightCenterX = waterArea.Br().X - moatWidth - halfWidth;
        auto topCenterY = waterArea.Tl().Y + moatWidth + halfHeight;
        auto bottomCenterY = waterArea.Br().Y - moatWidth - halfHeight;

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

    static auto SmallChunk = GenerateChunk<16, 16>();
    static auto MediumChunk = GenerateChunk<32, 32>();
    static auto LargeChunk = GenerateChunk<64, 64>();
}

namespace SampleUI::Screens::SampleSimpleMap {
    std::unique_ptr<Ui::CanvasPanel> Panel{nullptr};

    bool Initialize() { 
        PanelConfig.ZoomFn = Ui::Zoom::Exponential<f32, 1.1f>;
        PanelConfig.BackgroundColor = IM_COL32_WHITE;

        Panel = std::make_unique<Ui::CanvasPanel>(PanelConfig, [](Ui::CanvasPanel& canvas) {
            auto renderChunk = [&](const auto& chunk) {
                for(size_t row = 0; row < chunk.Height; row++) {
                    for(size_t col = 0; col < chunk.Width; col++) {
                        auto cell = World::CellCoord{static_cast<s32>(col), static_cast<s32>(row)};

                        auto color = TerrainToColor(chunk.At(cell));
                        auto contentBounds = Ui::ToUi(Ui::ToContentRect<CellSize>(cell));
                        auto screen = canvas.ContentToScreen(contentBounds);
                        auto* drawList = ImGui::GetWindowDrawList();
                        drawList->AddRectFilled(screen.Min, screen.Max, color);
                        drawList->AddRect(screen.Min, screen.Max, IM_COL32_BLACK, 0.f, ImDrawFlags_None, 1.f);
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

        if(changed) {
            SmallChunk = GenerateChunk<16, 16>();
            MediumChunk = GenerateChunk<32, 32>();
            LargeChunk = GenerateChunk<64, 64>();
        }
        ImGui::PopFont();

        // Place the canvas in the remaining content area below the controls. Sliders and
        // buttons live above canvasTop; the canvas owns everything from there to the bottom.
        auto canvasTop = ImGui::GetCursorPosY() + CanvasTopMargin;
        Ui::UiRect canvasBounds{
            ImVec2{0.f, canvasTop}, 
            ImVec2{Graphics::ScreenWidth, Graphics::ScreenHeight}
        };

        if(Panel) {
            Panel->SetBounds(canvasBounds);
            Panel->Render();
        }

        ImGui::End();

    }
} // namespace SampleUI::Screens::SampleSimpleMap