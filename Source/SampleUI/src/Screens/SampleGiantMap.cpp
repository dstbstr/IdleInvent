#include "SampleUI/Screens/SampleGiantMap.h"
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
#include <World/Movement.h>
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

    struct Pawn {
        World::WorldLocation Location;
        f32 Diameter = 1.0f;
    };
    enum struct TerrainType : u8 { Floor, Wall, Water };
    using Chunk = World::Chunk<TerrainType, 32, 32>;

    Ui::PanelConfig PanelConfig{};
    s32 WorldSeed = 42;
    s32 LoadRadius = 1;
    f32 FloorPercent = 0.7f;
    f32 WaterPercent = 0.2f;

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
        case Floor: return IM_COL32(200, 150, 150, 255);
        case Wall: return IM_COL32(20, 15, 15, 255);
        case Water: return IM_COL32(0, 0, 200, 255);
        }
        return IM_COL32(0, 0, 0, 255);
    }

    constexpr u32 MixSeed(u32 value) { 
        value ^= value >> 16;
        value *= 0x7f3b352dU;
        value ^= value >> 15;
        value *= 0x846ca68bU;
        value ^= value >> 16;
        return value;
    }

    constexpr u32 GetChunkSeed(u32 worldSeed, World::ChunkCoord chunk) {
        auto seed = MixSeed(worldSeed);
        seed ^= MixSeed(static_cast<u32>(chunk.X));
        seed = MixSeed(seed);
        seed ^= MixSeed(static_cast<u32>(chunk.Y));
        return MixSeed(seed);
    }

    Chunk GenerateChunk(u32 seed) {
        auto rand = SeededRandom(seed);
        Chunk chunk = {};

        // ensure at least one open path
        auto centerX = Chunk::Width / 2;
        auto centerY = Chunk::Height / 2;

        for(size_t row = 0; row < Chunk::Height; ++row) {
            for(size_t col = 0; col < Chunk::Width; ++col) {
                auto type = TerrainType::Wall;
                if(row == centerY || col == centerX) {
                    type = TerrainType::Floor;
                } else {
                    auto r = rand.GetNextFloat();
                    if(r < FloorPercent) {
                        type = TerrainType::Floor;
                    } else if(r < FloorPercent + WaterPercent) {
                        type = TerrainType::Water;
                    }
                }
                chunk.Cells.at(row).at(col) = type;
            }
        }


        return chunk;
    }

    Chunk& GetOrCreateChunk(World::ChunkCoord coord) {
        auto [it, inserted] = LoadedChunks.try_emplace(coord);
        if(inserted) {
            it->second = GenerateChunk(GetChunkSeed(static_cast<u32>(WorldSeed), coord));
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
        return chunk.At(loc.ToCellCoord()) == TerrainType::Floor;
    }

    World::Displacement GetMove() {
        auto delta = World::Displacement{};
        if(ImGui::IsKeyPressed(ImGuiKey_UpArrow) || ImGui::IsKeyPressed(ImGuiKey_W)) delta.Y -= 1.f;
        else if(ImGui::IsKeyPressed(ImGuiKey_DownArrow) || ImGui::IsKeyPressed(ImGuiKey_S)) delta.Y += 1.f;
        else if(ImGui::IsKeyPressed(ImGuiKey_LeftArrow) || ImGui::IsKeyPressed(ImGuiKey_A)) delta.X -= 1.f;
        else if(ImGui::IsKeyPressed(ImGuiKey_RightArrow) || ImGui::IsKeyPressed(ImGuiKey_D)) delta.X += 1.f;

        return delta;
    }

    void RenderChunk(const Ui::CanvasPanel& canvas, World::ChunkCoord coord, const Chunk& chunk) {
        auto relativeChunk = coord - PlayerPawn.Location.Chunk;
        auto chunkWidth = static_cast<f32>(Chunk::Width) * CellSize;
        auto chunkHeight = static_cast<f32>(Chunk::Height) * CellSize;

        auto pixelOrigin = ImVec2{relativeChunk.X * chunkWidth, relativeChunk.Y * chunkHeight};

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
        UpdateLoadedChunks(PlayerPawn.Location.Chunk);

        Panel = std::make_unique<Ui::CanvasPanel>(PanelConfig, [](Ui::CanvasPanel& canvas) {
            UpdateCamera(canvas, PlayerPawn);
            RenderMap(canvas);
            RenderPawn(canvas, PlayerPawn);
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

        auto originalFloor = FloorPercent;
        auto originalWater = WaterPercent;

        changed |= ImGui::SliderInt("World Seed", &WorldSeed, 0, 9'999);
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
            LoadedChunks.clear();
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
    }
} // namespace SampleUI::Screens::SampleGiantMap