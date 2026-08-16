#include "SampleUI/Screens/SampleNav.h"
#include "SampleUI/Screens/Screens.h"

#include <Manage/TickManager.h>
#include <Platform/Graphics.h>
#include <Platform/NumTypes.h>
#include <Ui/Panel/CanvasPanel.h>
#include <Ui/Panel/ZoomFunc.h>
#include <Ui/UiUtil.h>
#include <World/Movement.h>
#include <World/World.h>
#include <Utilities/Handle.h>
#include <Utilities/IRandom.h>

#include <imgui.h>

#include <chrono>
#include <cmath>
#include <memory>
#include <vector>

namespace {
    using clock = std::chrono::steady_clock;

    constexpr auto HeaderOffsetY = 32.f;
    constexpr auto ControlsOffsetY = 92.f;
    constexpr auto CanvasTopMargin = 8.f;
    constexpr auto CellSize = 24.f;

    struct Pawn {
        World::WorldLocation Location;
        f32 Diameter = 1.0f;
    };

    struct MovementRequest {
        World::Displacement Delta{};

        constexpr bool IsDefault() const { return Delta == World::Displacement{}; }
        constexpr void Reset() { Delta = World::Displacement{}; }
    };

    enum struct MovementKind : u8 {
        Discrete,
        Fluid
    };

    Ui::PanelConfig PanelConfig{};
    s32 WorldSeed = 42;
    f32 FloorPercent = 0.7f;
    f32 WaterPercent = 0.2f;
    int SelectedMovement = static_cast<int>(MovementKind::Discrete);
    f32 MoveSpeed = 1.0f;

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
    static Pawn PlayerPawn = { 
        .Location = {
            .ChunkCoord = { 0, 0 },
            .Pos = {1.5f, 1.5f}
        },
        .Diameter = 0.7f
    };
    static MovementRequest MoveRequest = {};
    static BaseTime timeSinceMove{};
    static int moveIntervalMs = 100;

    void PollMoveRequest() {
        if(SelectedMovement == static_cast<int>(MovementKind::Discrete)) {
            if(timeSinceMove < BaseTime(moveIntervalMs)) {
                return;
            }

            if(ImGui::IsKeyPressed(ImGuiKey_W) || ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
                MoveRequest.Delta = {0.f, -1.f};
            } else if(ImGui::IsKeyPressed(ImGuiKey_S) || ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
                MoveRequest.Delta = {0.f, 1.f};
            } else if(ImGui::IsKeyPressed(ImGuiKey_A) || ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) {
                MoveRequest.Delta = {-1.f, 0.f};
            } else if(ImGui::IsKeyPressed(ImGuiKey_D) || ImGui::IsKeyPressed(ImGuiKey_RightArrow)) {
                MoveRequest.Delta = {1.f, 0.f};
            } else {
                return;
            }
        }
    }

    void TryMove(Pawn& pawn, BaseTime elapsed) {
        timeSinceMove += elapsed;

        if(MoveRequest.IsDefault()) return;
        auto ToCoord = [](const World::LocalPos& pos) {
            return World::Coord{static_cast<s32>(std::floor(pos.X)), static_cast<s32>(std::floor(pos.Y))};
        };
        auto CanOccupy = [&](World::WorldLocation loc) { 
            if(loc.ChunkCoord != World::Coord{0, 0}) return false;

            auto coord = ToCoord(loc.Pos);
            auto terrain = WorldMap.At(coord);
            return terrain == TerrainType::Floor;
        };

        if(World::TryMove<Map::Width, Map::Height>(pawn.Location, MoveRequest.Delta, CanOccupy)) {
            timeSinceMove = BaseTime{};
        }
        MoveRequest.Reset();
    }

    void RenderMap(const Ui::CanvasPanel& canvas, const Map& chunk) {
        for(size_t row = 0; row < chunk.Height; row++) {
            for(size_t col = 0; col < chunk.Width; col++) {
                auto cellType = chunk.Cells.at(row).at(col);
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
    }

    void RenderPawn(const Ui::CanvasPanel& canvas, const Pawn& pawn) {
        auto scaledCenter = pawn.Location.Pos * CellSize;
        auto contentCenter = ImVec2{scaledCenter.X, scaledCenter.Y};
        auto screenCenter = canvas.ContentToScreen(contentCenter);
        auto screenRadius = (pawn.Diameter * CellSize * canvas.GetZoom()) / 2.f;

        ImGui::GetWindowDrawList()->AddCircleFilled(screenCenter, screenRadius, IM_COL32(0, 255, 0, 255));
    }

} // namespace

namespace SampleUI::Screens::SampleNav {
    std::unique_ptr<Ui::CanvasPanel> Panel{nullptr};
    std::vector<ScopedHandle> Handles{};
    bool Initialize() {
        PanelConfig.ZoomFn = Ui::Zoom::Exponential<f32, 1.1f>;
        PanelConfig.BackgroundColor = IM_COL32(255, 255, 255, 255);

        Panel = std::make_unique<Ui::CanvasPanel>(PanelConfig, [](Ui::CanvasPanel& canvas) {
            RenderMap(canvas, WorldMap);
            RenderPawn(canvas, PlayerPawn);
        });

        TickManager::Get().Register(Handles, [](BaseTime elapsed) {
            TryMove(PlayerPawn, elapsed);
        });

        return true;
    }

    void ShutDown() { 
        Panel.reset(); 
        Handles.clear();
    }

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

        auto movementOptions = "Discrete\0Fluid\0";
        ImGui::Combo("Movement Type", &SelectedMovement, movementOptions);

        if(SelectedMovement == static_cast<int>(MovementKind::Fluid)) {
            ImGui::InputFloat("Move Speed", &MoveSpeed);
        } else {
            ImGui::InputInt("Move Interval (ms)", &moveIntervalMs);
        }

        ImGui::PopFont();

        PollMoveRequest();

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