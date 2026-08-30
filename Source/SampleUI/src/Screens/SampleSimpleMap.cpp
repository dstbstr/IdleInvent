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
#include <memory>

namespace {
    constexpr auto ControlsOffsetY = 92.f;
    constexpr auto CanvasTopMargin = 8.f;
    constexpr auto CellPixelSize = 24.f;
    using Scale = Geometry::Scale2;

    enum struct TerrainType : u8 { Floor, Wall, Water, Road };
    enum struct ChunkSize : u8 { Small, Medium, Large };

    Ui::PanelConfig PanelConfig{
        .BackgroundColor = IM_COL32_WHITE,
        .ZoomFn = Ui::Zoom::Exponential<f32, 1.1f>
    };
    std::unique_ptr<Ui::CanvasPanel> Panel{nullptr};

    int SelectedChunkSize = static_cast<int>(ChunkSize::Small);
    auto RoomScale = Scale{0.3f, 0.3f};
    auto MoatScale = Scale{0.1f, 0.1f};
    auto RoadScale = Scale{0.25f, 0.25f};

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

            for(auto y = min.Y; y < max.Y; ++y) {
                for(auto x = min.X; x < max.X; x++) {
                    result.At({x, y}) = t;
                }
            }
        };

        auto chunkSize = ChunkType::Size();
        auto roomSize = Geometry::Max(World::CellSize::One(), Geometry::SizeCast<s32>(chunkSize * RoomScale));
        auto moatSize = Geometry::SizeCast<s32>(chunkSize * MoatScale);

        auto ToRoadThickness = [](s32 roomExtent, f32 scale) {
            if(scale <= 0.f) return s32{0};
            return std::max(1, static_cast<s32>(static_cast<f32>(roomExtent) * scale));
        };

        auto roadThickness = World::CellSize{
            ToRoadThickness(roomSize.X, RoadScale.X), 
            ToRoadThickness(roomSize.Y, RoadScale.Y)
        };

        auto roomInset = moatSize + roomSize / 2;

        auto MakeRoom = [&](World::CellCoord center) {
            auto room = Rect::FromCenterSize(center, roomSize);
            Fill(room, TerrainType::Wall);
            Fill(room.Contract(), TerrainType::Floor);
            return room;
        };

        auto MakeRoad = [&](Rect first, Rect second) {
            auto firstCenter = first.Center();
            auto secondCenter = second.Center();

            Rect road;
            if(firstCenter.X == secondCenter.X) {
                if(roadThickness.X == 0) return;

                auto top = std::min(first.Br().Y, second.Br().Y);
                auto bottom = std::max(first.Tl().Y, second.Tl().Y);

                road = Rect{
                    World::CellCoord{firstCenter.X - roadThickness.X / 2, top},
                    World::CellSize{roadThickness.X, bottom - top}
                };
            } else if(firstCenter.Y == secondCenter.Y) {
                if(roadThickness.Y == 0) return;
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

        auto tlRoom = MakeRoom({nearCenter.X, nearCenter.Y});
        auto trRoom = MakeRoom({farCenter.X, nearCenter.Y});
        auto blRoom = MakeRoom({nearCenter.X, farCenter.Y});
        auto brRoom = MakeRoom({farCenter.X, farCenter.Y});

        MakeRoad(tlRoom, trRoom);
        MakeRoad(tlRoom, blRoom);
        MakeRoad(trRoom, brRoom);
        MakeRoad(blRoom, brRoom);

        return result;
    }

    auto SmallChunk = GenerateChunk<16, 16>();
    auto MediumChunk = GenerateChunk<32, 32>();
    auto LargeChunk = GenerateChunk<64, 64>();

    void RenderControls() {
        ImGui::PushFont(GetFont(FontSizes::H4));
        ImGui::SetCursorPosY(ControlsOffsetY);

        bool changed = false;

        changed |= Ui::SliderScale2("Room Scale", RoomScale, 0.01f, 0.5f);
        changed |= Ui::SliderScale2("Road Scale", RoadScale, 0.f, 1.f);
        changed |= Ui::SliderScale2("Moat Scale", MoatScale, 0.f, 0.5f);

        const char* sizeLabels = "Small\0Medium\0Large";
        ImGui::Combo("Chunk Size", &SelectedChunkSize, sizeLabels, 3);

        if(changed) {
            SmallChunk = GenerateChunk<16, 16>();
            MediumChunk = GenerateChunk<32, 32>();
            LargeChunk = GenerateChunk<64, 64>();
        }
        ImGui::PopFont();
    }

    void RenderChunk(Ui::CanvasPanel& canvas, const auto& chunk) {
        auto* drawList = ImGui::GetWindowDrawList();

        chunk.VisitCells([&](World::CellCoord cell, TerrainType t) {
            auto contentBounds = Ui::ToUi(Ui::ToContentRect<CellPixelSize>(cell));
            auto screen = canvas.ContentToScreen(contentBounds);

            drawList->AddRectFilled(screen.Min, screen.Max, TerrainToColor(t));
            drawList->AddRect(screen.Min, screen.Max, IM_COL32_BLACK, 0.f, ImDrawFlags_None, 1.f);
        });
    }

    void RenderContent() {
        if(!Panel) return;
        SampleUI::RenderRemainingPanel(*Panel, CanvasTopMargin);
    }
}

namespace SampleUI::Screens::SampleSimpleMap {

    bool Initialize() { 
        Panel = std::make_unique<Ui::CanvasPanel>(PanelConfig, [](Ui::CanvasPanel& canvas) {
            switch(SelectedChunkSize) {
                case 0: RenderChunk(canvas, SmallChunk); break;
                case 1: RenderChunk(canvas, MediumChunk); break;
                case 2: RenderChunk(canvas, LargeChunk); break;
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
            RenderContent();
        });
    }
} // namespace SampleUI::Screens::SampleSimpleMap