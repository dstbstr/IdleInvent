#include "SampleUI/Screens/SampleNav.h"
#include "SampleUI/Screens/Screens.h"

#include <Manage/TickManager.h>
#include <Platform/Graphics.h>
#include <Platform/NumTypes.h>
#include <Ui/UiGeometry.h>
#include <Ui/Panel/CanvasPanel.h>
#include <Ui/Panel/ZoomFunc.h>
#include <Ui/UiUtil.h>
#include <Utilities/Handle.h>
#include <Utilities/IRandom.h>
#include <World/AStar.h>
#include <World/Movement.h>
#include <World/World.h>

#include <imgui.h>

#include <bit>
#include <chrono>
#include <cmath>
#include <memory>
#include <vector>

namespace {
    using clock = std::chrono::steady_clock;

    constexpr auto HeaderOffsetY = 32.f;
    constexpr auto ControlsOffsetY = 92.f;
    constexpr auto CanvasTopMargin = 8.f;
    constexpr auto CellSize = 64.f;

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
    enum struct CameraKind : u8 {
        Manual,
        Locked,
        Snap
    };

    Ui::PanelConfig PanelConfig{};
    s32 WorldSeed = 42;
    f32 FloorPercent = 0.7f;
    f32 WaterPercent = 0.2f;
    int SelectedMovement = static_cast<int>(MovementKind::Discrete);
    f32 MoveSpeed = 8.f;
    int SelectedCamera = static_cast<int>(CameraKind::Snap);
    f32 SnapPercent = 0.2f;
    std::optional<World::CellCoord> TargetCell{};
    std::optional<std::vector<World::CellCoord>> PathCells{};
    bool AutoFollow = true;

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
        TargetCell.reset();
        PathCells.reset();
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
            .Chunk = { 0, 0 },
            .Local = {1.5f, 1.5f}
        },
        .Diameter = 0.7f
    };

    enum struct MoveDir : u8 { None = 0, Up = 1 << 0, Down = 1 << 1, Left = 1 << 2, Right = 1 << 3 };
    constexpr MoveDir operator|(MoveDir lhs, MoveDir rhs) { return static_cast<MoveDir>(static_cast<u8>(lhs) | static_cast<u8>(rhs)); }
    constexpr MoveDir operator|=(MoveDir& lhs, MoveDir rhs) {
        lhs = lhs | rhs;
        return lhs;
    }
    constexpr bool HasFlag(MoveDir value, MoveDir flag) {
        return (static_cast<u8>(value) & static_cast<u8>(flag)) != 0;
    }

    static MoveDir MoveRequest = MoveDir::None;
    static BaseTime timeSinceMove{};
    static int moveIntervalMs = 100;

    World::CellCoord ContentPosToCell(ImVec2 pos) {
        return World::CellCoord{
            static_cast<s32>(std::floor(pos.x / CellSize)), 
            static_cast<s32>(std::floor(pos.y / CellSize))
        };
    }

    bool IsWalkable(World::CellCoord cell) {
        if(!Map::Contains(cell)) return false;
        return WorldMap.At(cell) == TerrainType::Floor;
    }

    bool CanOccupy(World::WorldLocation loc, const Pawn& pawn) {
        if(loc.Chunk != World::ChunkCoord{0, 0}) return false;
        auto radius = pawn.Diameter / 2.f;
        auto minX = static_cast<s32>(std::floor(loc.Local.X - radius));
        auto maxX = static_cast<s32>(std::floor(loc.Local.X + radius));
        auto minY = static_cast<s32>(std::floor(loc.Local.Y - radius));
        auto maxY = static_cast<s32>(std::floor(loc.Local.Y + radius));

        for(auto y = minY; y <= maxY; ++y) {
            for(auto x = minX; x <= maxX; ++x) {
                auto cell = World::CellCoord(x, y);
                if(IsWalkable(cell)) continue;

                // check if we're overlapping a non floor tile
                auto closeX = std::clamp(loc.Local.X, static_cast<f32>(x), static_cast<f32>(x + 1));
                auto closeY = std::clamp(loc.Local.Y, static_cast<f32>(y), static_cast<f32>(y + 1));
                auto dx = loc.Local.X - closeX;
                auto dy = loc.Local.Y - closeY;

                if(dx * dx + dy * dy <= radius * radius) return false;
            }
        }
        auto coord = loc.ToCellCoord();
        auto terrain = WorldMap.At(coord);

        return terrain == TerrainType::Floor;
    }

    bool ReachedWaypoint(const Pawn& pawn, World::CellCoord waypoint) {
        if(static_cast<MovementKind>(SelectedMovement) == MovementKind::Discrete) {
            return pawn.Location.ToCellCoord() == waypoint;
        }

        auto targetX = static_cast<f32>(waypoint.X) + 0.5f;
        auto targetY = static_cast<f32>(waypoint.Y) + 0.5f;
        auto dx = pawn.Location.Local.X - targetX;
        auto dy = pawn.Location.Local.Y - targetY;

        auto epsilon = 0.001f;

        return dx * dx + dy * dy <= epsilon * epsilon;
    }

    MoveDir GetManualMoveRequest() {
        auto GetKeys = [](auto keyFn) {
            auto keys = MoveDir::None;
            if(keyFn(ImGuiKey_W) || keyFn(ImGuiKey_UpArrow)) keys |= MoveDir::Up;
            if(keyFn(ImGuiKey_S) || keyFn(ImGuiKey_DownArrow)) keys |= MoveDir::Down;
            if(keyFn(ImGuiKey_A) || keyFn(ImGuiKey_LeftArrow)) keys |= MoveDir::Left;
            if(keyFn(ImGuiKey_D) || keyFn(ImGuiKey_RightArrow)) keys |= MoveDir::Right;

            return keys;
        };

        if(SelectedMovement == static_cast<int>(MovementKind::Discrete)) {
            // HA! Function currying for the win!
            return GetKeys([](auto key) { return ImGui::IsKeyPressed(key, false); });
        } else if(SelectedMovement == static_cast<int>(MovementKind::Fluid)) {
            return GetKeys(ImGui::IsKeyDown);
        }
    
        return MoveDir::None;
    }

    std::optional<World::Displacement> GetManualMoveDelta(MoveDir moveRequest, BaseTime elapsed) {
        if(moveRequest == MoveDir::None) return std::nullopt;

        auto elapsedSeconds = static_cast<f32>(elapsed.count()) / 1000.f;

        auto delta = World::Displacement{};
        if(SelectedMovement == static_cast<int>(MovementKind::Discrete)) {
            auto bits = static_cast<u8>(moveRequest);
            if(!std::has_single_bit(bits)) {
                return std::nullopt;
            }
            delta.X += static_cast<f32>(HasFlag(moveRequest, MoveDir::Right));
            delta.X -= static_cast<f32>(HasFlag(moveRequest, MoveDir::Left));
            delta.Y += static_cast<f32>(HasFlag(moveRequest, MoveDir::Down));
            delta.Y -= static_cast<f32>(HasFlag(moveRequest, MoveDir::Up));

        } else if(SelectedMovement == static_cast<int>(MovementKind::Fluid)) {
            delta.X += static_cast<f32>(HasFlag(moveRequest, MoveDir::Right)) * MoveSpeed * elapsedSeconds;
            delta.X -= static_cast<f32>(HasFlag(moveRequest, MoveDir::Left)) * MoveSpeed * elapsedSeconds;
            delta.Y += static_cast<f32>(HasFlag(moveRequest, MoveDir::Down)) * MoveSpeed * elapsedSeconds;
            delta.Y -= static_cast<f32>(HasFlag(moveRequest, MoveDir::Up)) * MoveSpeed * elapsedSeconds;
            if(std::abs(delta.X) + std::abs(delta.Y) > MoveSpeed * elapsedSeconds) {
                auto scale = std::sqrt(2.0f);
                delta.X /= scale;
                delta.Y /= scale;
            }
        }

        return delta;
    }

    std::optional<World::Displacement> GetAutoMoveDelta(Pawn& pawn, BaseTime elapsed) {
        if(!AutoFollow || !PathCells || PathCells->empty()) return std::nullopt;
        auto movement = static_cast<MovementKind>(SelectedMovement);
        auto next = PathCells->front();

        if(movement == MovementKind::Discrete) {
            auto current = pawn.Location.ToCellCoord();
            return World::Displacement{static_cast<f32>(next.X - current.X), static_cast<f32>(next.Y - current.Y)};
        } else {
            auto current = pawn.Location.Local;
            auto target = World::LocalPos{static_cast<f32>(next.X) + 0.5f, static_cast<f32>(next.Y) + 0.5f};

            auto difference = World::Displacement{target.X - current.X, target.Y - current.Y};
            auto distance = std::sqrt(difference.X * difference.X + difference.Y * difference.Y);
            if(distance == 0.f) return std::nullopt;

            auto elapsedSeconds = static_cast<f32>(elapsed.count()) / 1000.f;
            auto maxStep = MoveSpeed * elapsedSeconds;
            auto step = std::min(distance, maxStep);
            auto scale = step / distance;

            return World::Displacement{
                difference.X * scale,
                difference.Y * scale
            };
        }
    }

    void FindPath(const World::CellCoord& start, const World::CellCoord& end) {
        PathCells = World::AStar(start, end, [](World::CellCoord cell) {
            auto neighbors = World::SquareTopology::GetNeighbors(cell);
            std::vector<World::CellCoord> result{};
            std::copy_if(neighbors.begin(), neighbors.end(), std::back_inserter(result), [](World::CellCoord c) {
                return IsWalkable(c);
            });

            return result;
        });
        if(PathCells.has_value()) {
            PathCells->erase(PathCells->begin());
        }
    }

    void PollPathRequest(const Ui::CanvasPanel& canvas) {
        auto input = canvas.GetInput();
        if(!input.IsActivate) return;

        PathCells.reset();
        TargetCell = ContentPosToCell(input.MouseContent);
        if(!IsWalkable(TargetCell.value())) {
            TargetCell.reset();
            return;
        }

        auto playerCell = PlayerPawn.Location.ToCellCoord();
        FindPath(playerCell, TargetCell.value());
    }

    void AdvancePath(const Pawn& pawn) {
        if(TargetCell.has_value() && PathCells.has_value()) {
            if(ReachedWaypoint(pawn, PathCells->at(0))) {
                PathCells->erase(PathCells->begin());
                if(PathCells->empty()) {
                    PathCells.reset();
                    TargetCell.reset();
                }
            } else {
                if(!AutoFollow) {
                    FindPath(pawn.Location.ToCellCoord(), TargetCell.value());
                }
            }
        }
    }

    bool TryMove(Pawn& pawn, World::Displacement delta) {

        auto canOccupy = [&pawn](World::WorldLocation loc) { return CanOccupy(loc, pawn); };

        auto moved = false;
        if(delta.X != 0.f) {
            moved |= World::TryMove<Map::Width, Map::Height>(pawn.Location, {delta.X, 0.f}, canOccupy); 
        }
        if(delta.Y != 0.f) {
            moved |= World::TryMove<Map::Width, Map::Height>(pawn.Location, {0.f, delta.Y}, canOccupy); 
        }

        return moved;
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

    void RenderPath(const Ui::CanvasPanel& canvas) {
        if(!TargetCell.has_value()) return;
        if(!PathCells.has_value()) {
            auto contentCenter = ImVec2{
                (static_cast<f32>(TargetCell->X) + 0.5f) * CellSize, 
                (static_cast<f32>(TargetCell->Y) + 0.5f) * CellSize
            };
            auto screenCenter = canvas.ContentToScreen(contentCenter);
            auto screenRadius = (CellSize * canvas.GetZoom()) / 6.f;
            ImGui::GetWindowDrawList()->AddCircleFilled(screenCenter, screenRadius, IM_COL32(255, 0, 0, 255));
            return;
        }

        for(const auto& cell : PathCells.value()) {
            auto contentCenter = ImVec2{(static_cast<f32>(cell.X) + 0.5f) * CellSize, (static_cast<f32>(cell.Y) + 0.5f) * CellSize};
            auto screenCenter = canvas.ContentToScreen(contentCenter);
            auto screenRadius = (CellSize * canvas.GetZoom()) / 6.f;

            ImGui::GetWindowDrawList()->AddCircle(screenCenter, screenRadius, IM_COL32(0, 255, 255, 255));
        }
    }

    void RenderPawn(const Ui::CanvasPanel& canvas, const Pawn& pawn) {
        auto scaledCenter = pawn.Location.Local * CellSize;
        auto contentCenter = ImVec2{scaledCenter.X, scaledCenter.Y};
        auto screenCenter = canvas.ContentToScreen(contentCenter);
        auto screenRadius = (pawn.Diameter * CellSize * canvas.GetZoom()) / 2.f;

        ImGui::GetWindowDrawList()->AddCircleFilled(screenCenter, screenRadius, IM_COL32(0, 255, 0, 255));
    }

    void UpdateCamera(Ui::CanvasPanel& canvas, const Pawn& pawn) {
        auto camera = static_cast<CameraKind>(SelectedCamera);
        if(camera == CameraKind::Manual) return;

        auto pawnContent = pawn.Location.Local * CellSize;
        auto pawnLocal = canvas.ContentToLocal(Ui::ToUi(pawnContent));
        auto targetLocal = canvas.GetLocalCenter();
        auto delta = targetLocal - pawnLocal;

        if(camera == CameraKind::Locked) {
            canvas.PanBy(delta);
        } else if(camera == CameraKind::Snap) {
            auto size = canvas.GetViewportSize();
            if(pawnLocal.x < size.x * SnapPercent || pawnLocal.x > size.x * (1.f - SnapPercent)) {
                canvas.PanBy(ImVec2{delta.x, 0.f});
            }
            if(pawnLocal.y < size.y * SnapPercent || pawnLocal.y > size.y * (1.f - SnapPercent)) {
                canvas.PanBy(ImVec2{0.f, delta.y});
            }
        }
    }

    void UpdateMovement(Pawn& pawn, BaseTime elapsed) { 
        timeSinceMove += elapsed;

        auto movement = static_cast<MovementKind>(SelectedMovement);
        if(movement == MovementKind::Discrete && timeSinceMove < BaseTime(moveIntervalMs)) {
            return;
        }

        std::optional<World::Displacement> delta;
        if(AutoFollow) {
            if(PathCells.has_value()) {
                delta = GetAutoMoveDelta(pawn, elapsed);
            }
        }
        if(!delta.has_value()) {
            auto moveRequest = GetManualMoveRequest();
            delta = GetManualMoveDelta(moveRequest, elapsed);
        }
        if(!delta || *delta == World::Displacement{}) return;

        if(TryMove(pawn, *delta)) {
            if(movement == MovementKind::Discrete) {
                timeSinceMove = BaseTime{};
            }

            AdvancePath(pawn);
        }
    }

} // namespace

namespace SampleUI::Screens::SampleNav {
    std::unique_ptr<Ui::CanvasPanel> Panel{nullptr};
    std::vector<ScopedHandle> Handles{};
    bool Initialize() {
        PanelConfig.ZoomFn = Ui::Zoom::Exponential<f32, 1.1f>;
        PanelConfig.BackgroundColor = IM_COL32(255, 255, 255, 255);

        Panel = std::make_unique<Ui::CanvasPanel>(PanelConfig, [](Ui::CanvasPanel& canvas) {
            UpdateCamera(canvas, PlayerPawn);
            RenderMap(canvas, WorldMap);
            RenderPath(canvas);
            RenderPawn(canvas, PlayerPawn);
        });

        TickManager::Get().Register(Handles, [](BaseTime elapsed) {
            UpdateMovement(PlayerPawn, elapsed);
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
        if(ImGui::Combo("Movement Type", &SelectedMovement, movementOptions)) {
            MoveRequest = MoveDir::None;
            if(SelectedMovement == static_cast<int>(MovementKind::Discrete)) {
                auto& local = PlayerPawn.Location.Local;
                local.X = std::floor(local.X) + 0.5f;
                local.Y = std::floor(local.Y) + 0.5f;
            }
        }

        if(SelectedMovement == static_cast<int>(MovementKind::Fluid)) {
            ImGui::InputFloat("Move Speed", &MoveSpeed);
        } else {
            ImGui::InputInt("Move Interval (ms)", &moveIntervalMs);
        }

        auto cameraOptions = "Manual\0Locked\0Snap\0";
        ImGui::Combo("Camera Kind", &SelectedCamera, cameraOptions);

        if(SelectedCamera == static_cast<int>(CameraKind::Snap)) {
            ImGui::SliderFloat("Snap Percent", &SnapPercent, 0.1f, 0.5f);
        }

        ImGui::Checkbox("Auto Follow", &AutoFollow);
        ImGui::PopFont();


        // Place the canvas in the remaining content area below the controls. Sliders and
        // buttons live above canvasTop; the canvas owns everything from there to the bottom.
        const auto canvasTop = ImGui::GetCursorPosY() + CanvasTopMargin;
        auto canvasBounds = Ui::UiRect{
            ImVec2{0.f, canvasTop}, 
            ImVec2{Graphics::ScreenWidth, Graphics::ScreenHeight}
        };

        if(Panel) {
            Panel->SetBounds(canvasBounds);

            PollPathRequest(*Panel);

            Panel->Render();
        }

        ImGui::End();
    }
} // namespace SampleUI::Screens::SampleNav