#include "SampleUI/Screens/SampleGiantMap.h"
#include "SampleUI/Screens/Screens.h"
#include "SampleUI/Screens/SampleScreen.h"

#include <Manage/TickManager.h>
#include <Platform/Graphics.h>
#include <Platform/NumTypes.h>
#include <Ui/UiGeometry.h>
#include <Ui/Overlay.h>
#include <Ui/Panel/CanvasPanel.h>
#include <Ui/Panel/ZoomFunc.h>
#include <Ui/UiUtil.h>
#include <Utilities/Handle.h>
#include <World/Movement.h>
#include <World/Noise.h>
#include <World/World.h>

#include <imgui.h>

#include <algorithm>
#include <bitset>
#include <cmath>
#include <map>
#include <optional>
#include <memory>
#include <vector>

namespace {
    constexpr auto ControlsOffsetY = 92.f;
    constexpr auto CanvasTopMargin = 8.f;
    constexpr auto CellPixelSize = 64.f;
    constexpr auto HudMapDiameter = 192.f;
    constexpr auto HudMapPadding = 12.f;

    enum struct TerrainType : u8 { Dirt, Forest, Water, Snow };
    enum struct SmoothingKind : u8 { Cubic, Quintic };

    struct Pawn {
        World::WorldLocation Location;
        f32 Diameter = 1.0f;
    };
    struct TerrainSettings {
        s32 WorldSeed{42};
        s32 Octaves{4};
        int SelectedSmoothing{static_cast<int>(SmoothingKind::Cubic)};
        f32 SnowLevel{0.8f};
        f32 ForestLevel{0.6f};
        f32 WaterLevel{0.42f};
        f32 BaseFeatureSize{28.f};
    };

    using Chunk = World::Chunk<TerrainType, 32, 32>;
    constexpr size_t CellsPerChunk = Chunk::Width * Chunk::Height;
    using ExploredCells = std::bitset<CellsPerChunk>;
    std::map<World::ChunkCoord, ExploredCells> ExploredChunks{};

    std::unique_ptr<Ui::CanvasPanel> Panel{nullptr};
    std::vector<ScopedHandle> TickHandles{};

    TerrainSettings TerrainConfig{};

    Ui::PanelConfig PanelConfig{
        .BackgroundColor = IM_COL32_WHITE,
        .ZoomFn = Ui::Zoom::Exponential<f32, 1.2f, -20, 5>
    };

    s32 LoadRadius = 1;
    bool ShowHudMap = true;
    bool ShowDebug = false;
    bool ShowFogOfWar = true;
    int LightRadius = 6;

    std::map<World::ChunkCoord, Chunk> LoadedChunks{};
    Pawn PlayerPawn = {
        .Location = {
            .Chunk = {0, 0}, 
            .Local = World::GetCellCenter(Chunk::CenterCell())
        }, 
        .Diameter = 0.7f
    };

    constexpr ImU32 TerrainToColor(TerrainType type) {
        switch(type) {
            using enum TerrainType;
            case Dirt: return IM_COL32(200, 150, 150, 255);
            case Forest: return IM_COL32(0, 200, 0, 255);
            case Water: return IM_COL32(0, 0, 200, 255);
            case Snow: return IM_COL32(240, 240, 240, 255);
        }
        return IM_COL32(0, 0, 0, 255);
    }

    constexpr TerrainType HeightToTerrain(const TerrainSettings& settings, f32 height) {
        if(height > settings.SnowLevel) return TerrainType::Snow;
        if(height > settings.ForestLevel) return TerrainType::Forest;
        if(height < settings.WaterLevel) return TerrainType::Water;
        return TerrainType::Dirt;
    }

    f64 GetHeight(f64 x, f64 y) {
        auto seed = static_cast<u32>(TerrainConfig.WorldSeed);
        auto octaves = static_cast<size_t>(TerrainConfig.Octaves);
        auto baseFrequency = 1.0 / static_cast<f64>(TerrainConfig.BaseFeatureSize);

        switch(static_cast<SmoothingKind>(TerrainConfig.SelectedSmoothing)) {
            using enum SmoothingKind;
            case Cubic: return Noise::Fractal2D(seed, x, y, octaves, baseFrequency, Smoothstep::Cubic);
            case Quintic: return Noise::Fractal2D(seed, x, y, octaves, baseFrequency, Smoothstep::Quintic);
        }

        return 0.0;
    }

    Chunk GenerateChunk(World::ChunkCoord chunkCoord) {
        Chunk chunk = {};
        chunk.VisitCells([&](World::CellCoord cell, TerrainType& terrain) {
            auto globalX = static_cast<s64>(chunkCoord.X) * static_cast<s64>(Chunk::Width) + cell.X;
            auto globalY = static_cast<s64>(chunkCoord.Y) * static_cast<s64>(Chunk::Height) + cell.Y;
            auto height = static_cast<f32>(GetHeight(static_cast<f64>(globalX), static_cast<f64>(globalY)));
            terrain = HeightToTerrain(TerrainConfig, height);
        });

        return chunk;
    }

    Chunk& GetOrCreateChunk(World::ChunkCoord coord) {
        auto [it, inserted] = LoadedChunks.try_emplace(coord);
        if(inserted) {
            it->second = GenerateChunk(coord);
        }

        return it->second;
    }

    void UpdateLoadedChunks(World::ChunkCoord center) {
        for(auto y = center.Y - LoadRadius; y <= center.Y + LoadRadius; y++) {
            for(auto x = center.X - LoadRadius; x <= center.X + LoadRadius; x++) {
                GetOrCreateChunk({x, y});
            }
        }

        std::erase_if(LoadedChunks, [center](const auto& entry) {
            auto coord = entry.first;
            return std::abs(coord.X - center.X) > LoadRadius || std::abs(coord.Y - center.Y) > LoadRadius;
        });
    }

    constexpr size_t ToCellIndex(World::CellCoord cell) {
        return static_cast<size_t>(cell.Y) * Chunk::Width + static_cast<size_t>(cell.X);
    }

    bool IsExplored(World::ChunkCoord chunk, World::CellCoord cell) {
        auto found = ExploredChunks.find(chunk);
        if(found == ExploredChunks.end()) return false;

        return found->second.test(ToCellIndex(cell));
    }

    void RevealAround(const Pawn& pawn) {
        auto r2 = LightRadius * LightRadius;

        for(auto y = -LightRadius; y <= LightRadius; ++y) {
            for(auto x = -LightRadius; x <= LightRadius; ++x) {
                if(x * x + y * y > r2) continue;

                auto location = World::Offset<Chunk::Width, Chunk::Height>(
                    pawn.Location, {static_cast<f32>(x), static_cast<f32>(y)}
                );
                auto cell = location.ToCellCoord();
                ExploredChunks[location.Chunk].set(ToCellIndex(cell));
            }
        }
    }

    bool IsVisible(const Pawn& pawn, World::ChunkCoord chunk, World::CellCoord cell) {
        auto pawnCell = pawn.Location.ToCellCoord();
        auto relativeChunk = chunk - pawn.Location.Chunk;

        auto dx = static_cast<s64>(relativeChunk.X) * static_cast<s64>(Chunk::Width) + static_cast<s64>(cell.X - pawnCell.X);
        auto dy = static_cast<s64>(relativeChunk.Y) * static_cast<s64>(Chunk::Height) + static_cast<s64>(cell.Y - pawnCell.Y);
        auto radius = static_cast<s64>(LightRadius);
        return dx * dx + dy * dy <= radius * radius;
    }

    bool CanOccupy(World::WorldLocation loc) {
        auto& chunk = GetOrCreateChunk(loc.Chunk);
        auto t = chunk.At(loc.ToCellCoord());
        return t != TerrainType::Water && t != TerrainType::Forest;
    }

    World::Displacement GetMove() {
        auto delta = World::Displacement{};
        if(ImGui::IsKeyPressed(ImGuiKey_UpArrow) || ImGui::IsKeyPressed(ImGuiKey_W)) delta.Y -= 1.f;
        else if(ImGui::IsKeyPressed(ImGuiKey_DownArrow) || ImGui::IsKeyPressed(ImGuiKey_S)) delta.Y += 1.f;
        else if(ImGui::IsKeyPressed(ImGuiKey_LeftArrow) || ImGui::IsKeyPressed(ImGuiKey_A)) delta.X -= 1.f;
        else if(ImGui::IsKeyPressed(ImGuiKey_RightArrow) || ImGui::IsKeyPressed(ImGuiKey_D)) delta.X += 1.f;

        return delta;
    }

    std::optional<World::WorldLocation> FindNearestValidLocation(World::WorldLocation origin, s32 maxDistance = 64) {
        auto cell = origin.ToCellCoord();
        origin.Local = World::GetCellCenter(cell);
        if(CanOccupy(origin)) return origin;

        for(auto distance = 1; distance <= maxDistance; ++distance) {
            for(auto dx = -distance; dx <= distance; ++dx) {
                auto dy = distance - std::abs(dx);
                auto candidate =
                    World::Offset<Chunk::Width, Chunk::Height>(origin, {static_cast<f32>(dx), static_cast<f32>(dy)});
                if(CanOccupy(candidate)) return candidate;
                if(dy == 0) continue;

                candidate =
                    World::Offset<Chunk::Width, Chunk::Height>(origin, {static_cast<f32>(dx), -static_cast<f32>(dy)});
                if(CanOccupy(candidate)) return candidate;
            }
        }

        return std::nullopt;
    }

    std::optional<ImU32> GetFogOverlay(World::ChunkCoord chunk, World::CellCoord cell) {
        if(!ShowFogOfWar) return std::nullopt;
        if(!IsExplored(chunk, cell)) return IM_COL32_BLACK;
        if(!IsVisible(PlayerPawn, chunk, cell)) return IM_COL32(0, 0, 0, 160);
        return std::nullopt;
    }

    void RenderChunk(const Ui::CanvasPanel& canvas, World::ChunkCoord coord, const Chunk& chunk) {
        auto relativeChunk = coord - PlayerPawn.Location.Chunk;
        auto chunkCellOrigin = Ui::ToUi(relativeChunk) * Ui::ToUi(Chunk::Size());
        auto chunkContentOrigin = chunkCellOrigin * CellPixelSize;
        auto* drawList = ImGui::GetWindowDrawList();

        chunk.VisitCells([&](World::CellCoord cell, const TerrainType& terrain) {
            auto chunkLocalBounds = Ui::ToUi(Ui::ToContentRect<CellPixelSize>(cell));
            auto contentBounds = chunkLocalBounds.Translate(chunkContentOrigin);
            auto cellBounds = canvas.ContentToScreen(contentBounds);

            drawList->AddRectFilled(cellBounds.Min, cellBounds.Max, TerrainToColor(terrain));
            if(auto fog = GetFogOverlay(coord, cell)) {
                drawList->AddRectFilled(cellBounds.Min, cellBounds.Max, *fog);
            }

            // border
            drawList->AddRect(cellBounds.Min, cellBounds.Max, IM_COL32(0, 0, 0, 255), 0.f, ImDrawFlags_None, 1.f);
        });
    }

    void RenderMap(const Ui::CanvasPanel& canvas) {
        for(const auto& [coord, chunk]: LoadedChunks) {
            RenderChunk(canvas, coord, chunk);
        }
    }

    void RenderPawn(const Ui::CanvasPanel& canvas, const Pawn& pawn) {
        auto contentCenter = Ui::ToUi(pawn.Location.Local) * CellPixelSize;
        auto screenCenter = canvas.ContentToScreen(contentCenter);
        auto screenRadius = (pawn.Diameter * CellPixelSize * canvas.GetZoom()) / 2.f;

        ImGui::GetWindowDrawList()->AddCircleFilled(screenCenter, screenRadius, IM_COL32(0, 255, 0, 255));
    }

    void RenderHudMap(const Ui::CanvasPanel& canvas, const Pawn& pawn) { 
        if(!ShowHudMap) return;

        auto vpSize = canvas.GetViewportSize();
        auto diameter = Ui::One * HudMapDiameter;
        auto padding = Ui::One * HudMapPadding;
    
        auto mapMaxLocal = vpSize - padding;
        auto mapMinLocal = mapMaxLocal - diameter;
        auto mapCenterLocal = Ui::Midpoint(mapMinLocal, mapMaxLocal);
        auto mapCenter = canvas.LocalToScreen(mapCenterLocal);

        auto* drawList = ImGui::GetWindowDrawList();

        auto visibleRadiusCells =
            static_cast<f32>(LoadRadius) * static_cast<f32>(std::min(Chunk::Width, Chunk::Height));
        auto mapRadius = HudMapDiameter * 0.5f;
        auto hudCellPixelSize = mapRadius / visibleRadiusCells;

        for(const auto& [coord, chunk] : LoadedChunks) {
            auto relativeChunk = coord - pawn.Location.Chunk;
            auto chunkOffset = Ui::ToUi(relativeChunk) * Ui::ToUi(Chunk::Size());
            auto pawnPos = Ui::ToUi(pawn.Location.Local);

            chunk.VisitCells([&](World::CellCoord cell, const TerrainType& terrain) {
                auto cellCenter = Ui::ToUi(cell) + Ui::Half;
                auto relativeCell = chunkOffset + cellCenter - pawnPos;
                auto screenCellCenter = mapCenter + relativeCell * hudCellPixelSize;
                
                auto d = screenCellCenter - mapCenter;
                if(d.x * d.x + d.y * d.y > mapRadius * mapRadius) return;

                auto halfCellPixelSize = Ui::Half * hudCellPixelSize;
                auto cellBounds = Ui::UiRect{screenCellCenter - halfCellPixelSize, screenCellCenter + halfCellPixelSize};
                drawList->AddRectFilled(cellBounds.Min, cellBounds.Max, TerrainToColor(terrain));
                if(auto fog = GetFogOverlay(coord, cell)) {
                    drawList->AddRectFilled(cellBounds.Min, cellBounds.Max, *fog);
                }
            });
        }

        drawList->AddCircleFilled(mapCenter, hudCellPixelSize, IM_COL32(0, 255, 0, 255));

        // Draw border around minimap
        drawList->AddCircle(mapCenter, mapRadius, IM_COL32_BLACK);
    }

    void RenderDebug() {
        if(!ShowDebug) return;
        Ui::Overlay::Draw("##GiantMapDebug", Ui::Overlay::Anchor::BottomLeft, []() {
            auto* font = GetFont(FontSizes::H4);
            if(font) ImGui::PushFont(font);

            ImGui::Text("Player Chunk: (%d, %d)", PlayerPawn.Location.Chunk.X, PlayerPawn.Location.Chunk.Y);
            ImGui::Text("Player Local: (%.2f, %.2f)", PlayerPawn.Location.Local.X, PlayerPawn.Location.Local.Y);
            ImGui::Text("Loaded Chunks: %zu", LoadedChunks.size());

            if(font) ImGui::PopFont();
        });
    }

    void UpdateCamera(Ui::CanvasPanel& canvas, const Pawn& pawn) {
        auto pawnContent = pawn.Location.Local * CellPixelSize;
        auto pawnLocal = canvas.ContentToLocal(Ui::ToUi(pawnContent));
        auto targetLocal = canvas.GetLocalCenter();
        auto delta = targetLocal - pawnLocal;
        canvas.PanBy(delta);
    }

    void UpdateMovement(Pawn& pawn) {
        auto move = GetMove();
        if(move == World::Displacement{}) return;

        auto originChunk = pawn.Location.Chunk;
        if(World::TryMove<Chunk::Width, Chunk::Height>(pawn.Location, move, CanOccupy)) {
            RevealAround(pawn);

            if(pawn.Location.Chunk != originChunk) {
                UpdateLoadedChunks(pawn.Location.Chunk);
            }
        }
    }

    void RenderControls() {
        ImGui::PushFont(GetFont(FontSizes::H4));
        ImGui::SetCursorPosY(ControlsOffsetY);

        bool changed = false;

        constexpr f32 TGap = 0.01f;
        changed |= ImGui::SliderInt("World Seed", &TerrainConfig.WorldSeed, 0, 9'999);
        changed |= ImGui::SliderFloat("Water Level", &TerrainConfig.WaterLevel, 0.f, TerrainConfig.ForestLevel - TGap);
        changed |= ImGui::SliderFloat("Forest Level", &TerrainConfig.ForestLevel, TerrainConfig.WaterLevel + TGap, TerrainConfig.SnowLevel - TGap);
        changed |= ImGui::SliderFloat("Snow Level", &TerrainConfig.SnowLevel, TerrainConfig.ForestLevel + TGap, 1.f);
        changed |= ImGui::SliderInt("Octaves", &TerrainConfig.Octaves, 1, 8);
        changed |= ImGui::SliderFloat(
            "Feature Size", &TerrainConfig.BaseFeatureSize, 4.f, 512.f, "%.0f cells", ImGuiSliderFlags_Logarithmic
        );

        const char* smoothingOptions = "Cubic\0Quintic\0";
        changed |= ImGui::Combo("Smoothing", &TerrainConfig.SelectedSmoothing, smoothingOptions);

        if(ImGui::Button("Regenerate")) {
            TerrainConfig.WorldSeed = (TerrainConfig.WorldSeed + 32) % 10'000;
            changed = true;
        }

        if(ImGui::SliderInt("Light Radius", &LightRadius, 1, 16)) {
            RevealAround(PlayerPawn);
        }
        ImGui::Checkbox("Fog of War", &ShowFogOfWar);
        ImGui::SameLine();
        ImGui::Checkbox("Show HUD", &ShowHudMap);
        ImGui::SameLine();
        ImGui::Checkbox("Show Debug", &ShowDebug);

        if(changed) {
            LoadedChunks.clear();
            ExploredChunks.clear();
            if(auto validLocation = FindNearestValidLocation(PlayerPawn.Location)) {
                PlayerPawn.Location = *validLocation;
            }

            UpdateLoadedChunks(PlayerPawn.Location.Chunk);
            RevealAround(PlayerPawn);
        }

        ImGui::PopFont();
    }

    void RenderContent() {
        if(!Panel) return;
        SampleUI::RenderRemainingPanel(*Panel, CanvasTopMargin);
    }
} // namespace

namespace SampleUI::Screens::SampleGiantMap {
    bool Initialize() {
        LoadedChunks.clear();
        ExploredChunks.clear();

        if(auto validLocation = FindNearestValidLocation(PlayerPawn.Location)) {
            PlayerPawn.Location = *validLocation;
        }
        UpdateLoadedChunks(PlayerPawn.Location.Chunk);
        RevealAround(PlayerPawn);

        Panel = std::make_unique<Ui::CanvasPanel>(PanelConfig, [](Ui::CanvasPanel& canvas) {
            UpdateCamera(canvas, PlayerPawn);
            RenderMap(canvas);
            RenderPawn(canvas, PlayerPawn);
            RenderHudMap(canvas, PlayerPawn);
        });

        TickManager::Get().Register(TickHandles, [](BaseTime) { UpdateMovement(PlayerPawn); });

        return true;
    }

    void ShutDown() {
        TickHandles.clear();
        Panel.reset();
    }

    void Render() {
        RenderSampleScreen("Sample Giant Map", [] { 
            RenderControls();
            RenderContent();
        });

        RenderDebug();
    }
} // namespace SampleUI::Screens::SampleGiantMap