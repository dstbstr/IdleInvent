#include "SampleUI/Screens/SampleGiantMap.h"
#include "SampleUI/Screens/Screens.h"

#include <Manage/TickManager.h>
#include <Platform/Graphics.h>
#include <Platform/NumTypes.h>
#include <Ui/UiGeometry.h>
#include <Ui/Overlay.h>
#include <Ui/Panel/CanvasPanel.h>
#include <Ui/Panel/ZoomFunc.h>
#include <Ui/UiUtil.h>
#include <Utilities/Handle.h>
#include <Utilities/IRandom.h>
#include <World/Movement.h>
#include <World/Noise.h>
#include <World/World.h>

#include <imgui.h>

#include <cmath>
#include <map>
#include <memory>
#include <vector>

namespace {
    constexpr auto HeaderOffsetY = 32.f;
    constexpr auto ControlsOffsetY = 92.f;
    constexpr auto CanvasTopMargin = 8.f;
    constexpr auto CellSize = 64.f;
    constexpr f32 HudMapDiameter = 192.f;
    constexpr f32 HudMapPadding = 12.f;

    struct Pawn {
        World::WorldLocation Location;
        f32 Diameter = 1.0f;
    };
    enum struct TerrainType : u8 { Dirt, Forest, Water, Snow };
    enum struct SmoothingKind : u8 { Cubic, Quintic };

    using Chunk = World::Chunk<TerrainType, 32, 32>;

    Ui::PanelConfig PanelConfig{};
    s32 WorldSeed = 42;
    s32 LoadRadius = 1;
    s32 Octaves = 4;
    int SelectedSmoothing = static_cast<int>(SmoothingKind::Cubic);
    f32 SnowLevel = 0.8f;
    f32 ForestLevel = 0.6f;
    f32 WaterLevel = 0.42f;
    f32 BaseFeatureSize = 28.f;
    bool ShowHudMap = true;
    bool ShowDebug = false;

    static std::map<World::ChunkCoord, Chunk> LoadedChunks{};
    static Pawn PlayerPawn = {
        .Location = {
            .Chunk = {0, 0}, 
            .Local = {
                static_cast<f32>(Chunk::Width) / 2.f + 0.5f, 
                static_cast<f32>(Chunk::Height) / 2.f + 0.5f}
            }
        , .Diameter = 0.7f
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

    f64 GetHeight(f64 x, f64 y) {
        auto seed = static_cast<u32>(WorldSeed);
        auto octaves = static_cast<size_t>(Octaves);
        auto baseFrequency = 1.0 / static_cast<f64>(BaseFeatureSize);

        switch(static_cast<SmoothingKind>(SelectedSmoothing)) {
            using enum SmoothingKind;
            case Cubic: return Noise::Fractal2D(seed, x, y, octaves, baseFrequency, Smoothstep::Cubic);
            case Quintic: return Noise::Fractal2D(seed, x, y, octaves, baseFrequency, Smoothstep::Quintic);
        }

        return 0.0;
    }

    Chunk GenerateChunk(World::ChunkCoord chunkCoord) {
        Chunk chunk = {};

        for(size_t row = 0; row < Chunk::Height; ++row) {
            for(size_t col = 0; col < Chunk::Width; ++col) {
                auto globalX = static_cast<s64>(chunkCoord.X) * static_cast<s64>(Chunk::Width) + static_cast<s64>(col);
                auto globalY = static_cast<s64>(chunkCoord.Y) * static_cast<s64>(Chunk::Height) + static_cast<s64>(row);
                auto height = static_cast<f32>(GetHeight(static_cast<f64>(globalX), static_cast<f64>(globalY)));
                auto type = TerrainType::Dirt;
                if(height > SnowLevel) {
                    type = TerrainType::Snow;
                } else if(height > ForestLevel) {
                    type = TerrainType::Forest;
                } else if(height < WaterLevel) {
                    type = TerrainType::Water;
                } else {
                    type = TerrainType::Dirt;
                }

                chunk.Cells.at(row).at(col) = type;
            }
        }


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
        origin.Local = {static_cast<f32>(cell.X) + 0.5f, static_cast<f32>(cell.Y) + 0.5f};
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

    void RenderChunk(const Ui::CanvasPanel& canvas, World::ChunkCoord coord, const Chunk& chunk) {
        auto relativeChunk = coord - PlayerPawn.Location.Chunk;
        auto chunkWidth = static_cast<f32>(Chunk::Width) * CellSize;
        auto chunkHeight = static_cast<f32>(Chunk::Height) * CellSize;

        auto pixelOrigin = ImVec2{static_cast<f32>(relativeChunk.X) * chunkWidth, static_cast<f32>(relativeChunk.Y) * chunkHeight};

        for(size_t row = 0; row < chunk.Height; row++) {
            for(size_t col = 0; col < chunk.Width; col++) {
                auto cellType = chunk.Cells.at(row).at(col);
                auto color = TerrainToColor(cellType);

                auto contentTl = ImVec2{pixelOrigin.x + static_cast<f32>(col) * CellSize, pixelOrigin.y + static_cast<f32>(row) * CellSize};
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

    void RenderMap(const Ui::CanvasPanel& canvas) {
        for(const auto& [coord, chunk]: LoadedChunks) {
            RenderChunk(canvas, coord, chunk);
        }
    }

    void RenderPawn(const Ui::CanvasPanel& canvas, const Pawn& pawn) {
        auto scaledCenter = pawn.Location.Local * CellSize;
        auto contentCenter = ImVec2{scaledCenter.X, scaledCenter.Y};
        auto screenCenter = canvas.ContentToScreen(contentCenter);
        auto screenRadius = (pawn.Diameter * CellSize * canvas.GetZoom()) / 2.f;

        ImGui::GetWindowDrawList()->AddCircleFilled(screenCenter, screenRadius, IM_COL32(0, 255, 0, 255));
    }

    void RenderHudMap(const Ui::CanvasPanel& canvas, const Pawn& pawn) { 
        if(!ShowHudMap) return;

        auto vpSize = canvas.GetViewportSize();
        auto mapMaxLocal = ImVec2{vpSize.x - HudMapPadding, vpSize.y - HudMapPadding};
        auto mapMinLocal = ImVec2{mapMaxLocal.x - HudMapDiameter, mapMaxLocal.y - HudMapDiameter};
        auto mapCenterLocal = ImVec2{(mapMinLocal.x + mapMaxLocal.x) * 0.5f, (mapMinLocal.y + mapMaxLocal.y) * 0.5f};

        auto mapCenter = canvas.LocalToScreen(mapCenterLocal);

        auto* drawList = ImGui::GetWindowDrawList();

        auto visibleRadiusCells =
            static_cast<f32>(LoadRadius) * static_cast<f32>(std::min(Chunk::Width, Chunk::Height));
        auto mapRadius = HudMapDiameter * 0.5f;
        auto hudCellSize = mapRadius / visibleRadiusCells;

        for(const auto& [coord, chunk] : LoadedChunks) {
            auto relativeChunk = coord - pawn.Location.Chunk;

            for(size_t row = 0; row < Chunk::Height; row++) {
                for(size_t col = 0; col < Chunk::Width; col++) {
                    auto relCellX = static_cast<f32>(relativeChunk.X) * static_cast<f32>(Chunk::Width) +
                                    static_cast<f32>(col) - pawn.Location.Local.X + 0.5f;
                    auto relCellY = static_cast<f32>(relativeChunk.Y) * static_cast<f32>(Chunk::Height) +
                                    static_cast<f32>(row) - pawn.Location.Local.Y + 0.5f;

                    auto cellCenter = ImVec2{mapCenter.x + relCellX * hudCellSize, mapCenter.y + relCellY * hudCellSize};
                
                    auto dx = cellCenter.x - mapCenter.x;
                    auto dy = cellCenter.y - mapCenter.y;
                    if(dx * dx + dy * dy > mapRadius * mapRadius) continue;

                    auto halfCell = hudCellSize * 0.5f;
                    auto cellMin = ImVec2{cellCenter.x - halfCell, cellCenter.y - halfCell};
                    auto cellMax = ImVec2{cellCenter.x + halfCell, cellCenter.y + halfCell};

                    auto terrain = chunk.Cells.at(row).at(col);
                    drawList->AddRectFilled(cellMin, cellMax, TerrainToColor(terrain));
                }
            }
        }

        drawList->AddCircleFilled(mapCenter, hudCellSize, IM_COL32(0, 255, 0, 255));

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
        auto pawnContent = pawn.Location.Local * CellSize;
        auto pawnLocal = canvas.ContentToLocal(Ui::ToUi(pawnContent));
        auto targetLocal = canvas.GetLocalCenter();
        auto delta = targetLocal - pawnLocal;
        canvas.PanBy(delta);
    }

    void UpdateMovement(Pawn& pawn, BaseTime elapsed) {
        auto move = GetMove();
        if(move == World::Displacement{}) return;

        auto originChunk = pawn.Location.Chunk;
        if(World::TryMove<Chunk::Width, Chunk::Height>(pawn.Location, move, CanOccupy)) {
            if(pawn.Location.Chunk != originChunk) {
                UpdateLoadedChunks(pawn.Location.Chunk);
            }
        }
    }

} // namespace

namespace SampleUI::Screens::SampleGiantMap {
    std::unique_ptr<Ui::CanvasPanel> Panel{nullptr};
    std::vector<ScopedHandle> Handles{};
    bool Initialize() {
        PanelConfig.ZoomFn = Ui::Zoom::Exponential<f32, 1.2f, -20, 5>;
        PanelConfig.BackgroundColor = IM_COL32(255, 255, 255, 255);
        LoadedChunks.clear();
        if(auto validLocation = FindNearestValidLocation(PlayerPawn.Location)) {
            PlayerPawn.Location = *validLocation;
        }
        UpdateLoadedChunks(PlayerPawn.Location.Chunk);

        Panel = std::make_unique<Ui::CanvasPanel>(PanelConfig, [](Ui::CanvasPanel& canvas) {
            UpdateCamera(canvas, PlayerPawn);
            RenderMap(canvas);
            RenderPawn(canvas, PlayerPawn);
            RenderHudMap(canvas, PlayerPawn);
        });

        TickManager::Get().Register(Handles, [](BaseTime elapsed) { UpdateMovement(PlayerPawn, elapsed); });

        return true;
    }

    void ShutDown() {
        Panel.reset();
        Handles.clear();
    }

    void Render() {
        ImGui::SetNextWindowPos({0.f, 0.f});
        ImGui::SetNextWindowSize({Graphics::ScreenWidth, Graphics::ScreenHeight});
        ImGui::Begin("Sample Giant Map", nullptr, BaseUiFlags);

        if(ImGui::Button("Back")) {
            Screens::SetActiveScreen(Screen::Landing);
            ImGui::End();
            return;
        }

        ImGui::SetCursorPosY(HeaderOffsetY);
        ImGui::PushFont(GetFont(FontSizes::H1));
        TextCenteredX("Sample Giant Map");
        ImGui::PopFont();

        ImGui::PushFont(GetFont(FontSizes::H4));
        ImGui::SetCursorPosY(ControlsOffsetY);

        bool changed = false;

        changed |= ImGui::SliderInt("World Seed", &WorldSeed, 0, 9'999);
        changed |= ImGui::SliderFloat("Water Level", &WaterLevel, 0.f, 1.f);
        changed |= ImGui::SliderFloat("Forest Level", &ForestLevel, 0.f, 1.f);
        changed |= ImGui::SliderFloat("Snow Level", &SnowLevel, 0.f, 1.f);
        changed |= ImGui::SliderInt("Octaves", &Octaves, 1, 8);
        changed |= ImGui::SliderFloat("Feature Size", &BaseFeatureSize, 4.f, 512.f, "%.0f cells", ImGuiSliderFlags_Logarithmic);

        const char* smoothingOptions = "Cubic\0Quintic\0";
        changed |= ImGui::Combo("Smoothing", &SelectedSmoothing, smoothingOptions);
        
        if(WaterLevel >= ForestLevel) {
            WaterLevel = ForestLevel - 0.1f;
        }
        if(ForestLevel >= SnowLevel) {
            ForestLevel = SnowLevel - 0.1f;
        }

        if(ImGui::Button("Regenerate")) {
            WorldSeed = (WorldSeed + 32) % 10'000;
            changed = true;
        }
        
        ImGui::Checkbox("Show HUD", &ShowHudMap);
        ImGui::SameLine();
        ImGui::Checkbox("Show Debug", &ShowDebug);

        if(changed) {
            LoadedChunks.clear();
            if(auto validLocation = FindNearestValidLocation(PlayerPawn.Location)) {
                PlayerPawn.Location = *validLocation;
            }
            UpdateLoadedChunks(PlayerPawn.Location.Chunk);
        }

        ImGui::PopFont();

        // Place the canvas in the remaining content area below the controls. Sliders and
        // buttons live above canvasTop; the canvas owns everything from there to the bottom.
        const auto canvasTop = ImGui::GetCursorPosY() + CanvasTopMargin;
        auto canvasBounds = Ui::UiRect{ImVec2{0.f, canvasTop}, ImVec2{Graphics::ScreenWidth, Graphics::ScreenHeight}};

        if(Panel) {
            Panel->SetBounds(canvasBounds);
            Panel->Render();
        }

        ImGui::End();
        RenderDebug();
    }
} // namespace SampleUI::Screens::SampleGiantMap