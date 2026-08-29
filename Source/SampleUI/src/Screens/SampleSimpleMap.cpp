#include "SampleUI/Screens/SampleSimpleMap.h"
#include "SampleUI/Screens/Screens.h"
#include "SampleUI/Screens/SampleScreen.h"

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
    constexpr auto CellPixelSize = 24.f;

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
        using ChunkType = World::Chunk<TerrainType, TWidth, THeight>;

        using Rect = Geometry::Rect<s32, World::ChunkSpace>;

        ChunkType result = {};

        auto fullArea = Rect{World::CellCoord{0, 0}, ChunkType::Size()};
        auto waterArea = fullArea.Contract();

        auto Fill = [&](Rect area, TerrainType t) {
            auto min = World::CellCoord{
                std::max(fullArea.Tl().X, area.Tl().X), std::max(fullArea.Tl().Y, area.Tl().Y)
            };
            auto max = World::CellCoord{
                std::min(fullArea.Br().X, area.Br().X), std::min(fullArea.Br().Y, area.Br().Y)
            };
            if(min.X >= max.X || min.Y >= max.Y) return;

            for(size_t row = min.Y; row < max.Y; row++) {
                for(size_t col = min.X; col < max.X; col++) {
                    result.Cells.at(row).at(col) = t;
                }
            }
        };

        auto roomSize = World::CellSize{
            std::max(1, static_cast<s32>(TWidth * RoomWidth)),
            std::max(1, static_cast<s32>(THeight * RoomHeight))
        };
        auto moatSize = World::CellSize{
            static_cast<s32>(TWidth * MoatWidth), 
            static_cast<s32>(THeight * MoatWidth)
        };
        auto roomInset = World::CellSize{
            moatSize.X + roomSize.X / 2,
            moatSize.Y + roomSize.Y / 2
        };
        auto roadThickness = World::CellSize{
            std::max(1, static_cast<s32>(static_cast<f32>(roomSize.X) * RoadWidth)),
            std::max(1, static_cast<s32>(static_cast<f32>(roomSize.Y) * RoadWidth))
        };

        auto MakeRoom = [&](World::CellCoord center) {
            auto room = Rect::FromCenterSize(center, roomSize);
            Fill(room, TerrainType::Wall);
            Fill(room.Contract(), TerrainType::Floor);
            return room;
        };

        auto MakeRoad = [&](Rect first, Rect second) {
            if(RoadWidth <= 0.f) return;

            auto firstCenter = first.Center();
            auto secondCenter = second.Center();

            Rect road;
            if(firstCenter.X == secondCenter.X) {
                auto top = std::min(first.Br().Y, second.Br().Y);
                auto bottom = std::max(first.Tl().Y, second.Tl().Y);

                road = Rect{
                    World::CellCoord{firstCenter.X - roadThickness.X / 2, top},
                    World::CellSize{roadThickness.X, bottom - top}
                };
            } else {
                auto left = std::min(first.Br().X, second.Br().X);
                auto right = std::max(first.Tl().X, second.Tl().X);

                road = Rect{
                    World::CellCoord{left, firstCenter.Y - roadThickness.Y / 2},
                    World::CellSize{right - left, roadThickness.Y}
                };
            }

            Fill(road, TerrainType::Road);
        };

        Fill(fullArea, TerrainType::Wall);
        Fill(waterArea, TerrainType::Water);

        auto nearCenter = waterArea.Tl() + roomInset;
        auto farCenter = waterArea.Br() - roomInset;

        auto TlRoom = MakeRoom({nearCenter.X, nearCenter.Y});
        auto TrRoom = MakeRoom({farCenter.X, nearCenter.Y});
        auto BlRoom = MakeRoom({nearCenter.X, farCenter.Y});
        auto BrRoom = MakeRoom({farCenter.X, farCenter.Y});

        MakeRoad(TlRoom, TrRoom);
        MakeRoad(TlRoom, BlRoom);
        MakeRoad(TrRoom, BrRoom);
        MakeRoad(BlRoom, BrRoom);

        return result;
    }

    static auto SmallChunk = GenerateChunk<16, 16>();
    static auto MediumChunk = GenerateChunk<32, 32>();
    static auto LargeChunk = GenerateChunk<64, 64>();

    void RenderControls() {
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
    }

    void RenderContent(Ui::CanvasPanel* panel) {
        auto canvasTop = ImGui::GetCursorPosY() + CanvasTopMargin;
        Ui::UiRect canvasBounds{ImVec2{0.f, canvasTop}, ImVec2{Graphics::ScreenWidth, Graphics::ScreenHeight}};

        if(panel) {
            panel->SetBounds(canvasBounds);
            panel->Render();
        }
    }
}

namespace SampleUI::Screens::SampleSimpleMap {
    std::unique_ptr<Ui::CanvasPanel> Panel{nullptr};

    bool Initialize() { 
        PanelConfig.ZoomFn = Ui::Zoom::Exponential<f32, 1.1f>;
        PanelConfig.BackgroundColor = IM_COL32_WHITE;

        Panel = std::make_unique<Ui::CanvasPanel>(PanelConfig, [](Ui::CanvasPanel& canvas) {
            auto renderChunk = [&](const auto& chunk) {
                auto* drawList = ImGui::GetWindowDrawList();

                chunk.VisitCells([&](World::CellCoord cell, TerrainType t) {
                    auto contentBounds = Ui::ToUi(Ui::ToContentRect<CellPixelSize>(cell));
                    auto screen = canvas.ContentToScreen(contentBounds);

                    drawList->AddRectFilled(screen.Min, screen.Max, TerrainToColor(t));
                    drawList->AddRect(screen.Min, screen.Max, IM_COL32_BLACK, 0.f, ImDrawFlags_None, 1.f);
                });
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
        RenderSampleScreen("Simple Map", [] {
            RenderControls();
            RenderContent(Panel.get());
        });
    }
} // namespace SampleUI::Screens::SampleSimpleMap