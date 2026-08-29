#include <SampleUI/Screens/SampleParticles.h>
#include <SampleUI/Screens/Screens.h>
#include <SampleUI/Screens/SampleScreen.h>

#include <Manage/TickManager.h>
#include <Platform/Graphics.h>
#include <Ui/Dialog.h>
#include <Ui/Panel/CanvasPanel.h>
#include <Ui/UiUtil.h>
#include <Ui/Particle/ParticleSystem.h>

#include <imgui.h>

#include <array>
#include <memory>
#include <numbers>
#include <vector>

namespace {
    constexpr auto ControlsOffsetY = 92.f;
    constexpr auto CanvasTopMargin = 8.f;
    constexpr auto DefaultCapacity = 16384;
    constexpr auto MinCapacity = 1024;
    constexpr auto MaxCapacity = 131072;

    enum struct PositionMode : u8 { FollowMouse, TopOfScreen };
    struct EmitterEditorState {
        f32 RatePerSecond = 400.f;
        std::array<f32, 2> SpeedRange{80.f, 220.f};
        f32 AngleCenterDeg = 270.f;
        f32 AngleSpreadDeg = 60.f;
        std::array<s32, 2> LifeRangeMs{600, 1'400};
        f32 ParticleSize = 4.f;
        ImVec4 ParticleColor{1.f, 0.7f, 0.2f, 1.f};
        ImVec2 ParticleGravity{0.f, 200.f};
    };

    struct Preset {
        const char* Name{nullptr};
        ImGuiKey Key{};
        PositionMode PosMode{};
        Ui::Direction Direction{};
        Ui::Spread Spread{};
        f32 JitterFracX{0.f};
        Ui::EmitterSettings Settings{};
    };

    std::unique_ptr<Ui::ParticleSystem> Particles{};
    std::unique_ptr<Ui::CanvasPanel> Canvas{};
    Ui::Emitter* UserEmitter = nullptr;
    std::vector<ScopedHandle> TickHandles{};

    auto Capacity = DefaultCapacity;
    auto PendingCapacity = DefaultCapacity;

    EmitterEditorState UserEmitterEditor{};
    const std::array<Preset, 4> Presets{{
        Preset{
            .Name = "Fountain", .Key = ImGuiKey_1, .PosMode = PositionMode::FollowMouse,
            .Direction = Ui::Direction::N, .Spread = Ui::Spread::Narrow,
            .Settings = {
                .Gravity = ImVec2{0.f, 200.f},
                .RatePerSecond = 600.f,
                .SpeedMin = 200.f, .SpeedMax = 380.f,
                .LifeMinMs = 800, .LifeMaxMs = 1600,
                .Size = 3.f,
                .Color = IM_COL32(80, 180, 255, 255),
            }
        },
        Preset{
            .Name = "Sparks", .Key = ImGuiKey_2, .PosMode = PositionMode::FollowMouse,
            .Direction = Ui::Direction::N, .Spread = Ui::Spread::Full,
            .Settings = {
                .RatePerSecond = 1200.f,
                .SpeedMin = 350.f, .SpeedMax = 600.f,
                .LifeMinMs = 120, .LifeMaxMs = 280,
                .Size = 2.f,
                .Color = IM_COL32(255, 230, 120, 255),
            }
        },
        Preset{
            .Name = "Snow", .Key = ImGuiKey_3, .PosMode = PositionMode::TopOfScreen,
            .Direction = Ui::Direction::S, .Spread = Ui::Spread::Thin,
            .JitterFracX = 0.5f,
            .Settings = {
                .Gravity = ImVec2{0.f, 30.f},
                .RatePerSecond = 200.f,
                .SpeedMin = 40.f, .SpeedMax = 90.f,
                .LifeMinMs = 3000, .LifeMaxMs = 5000,
                .Size = 3.f,
                .Color = IM_COL32(240, 240, 255, 255),
            }
        },
        Preset{
            .Name = "Fireworks", .Key = ImGuiKey_4, .PosMode = PositionMode::FollowMouse,
            .Direction = Ui::Direction::N, .Spread = Ui::Spread::Full,
            .Settings = {
                .Gravity = ImVec2{0.f, 250.f},
                .RatePerSecond = 3000.f,
                .SpeedMin = 150.f, .SpeedMax = 420.f,
                .LifeMinMs = 500, .LifeMaxMs = 1200,
                .Size = 2.5f,
                .Color = IM_COL32(255, 120, 200, 255),
            }
        },
    }};

    std::array<Ui::Emitter*, Presets.size()> PresetEmitters{};

    void UpdateEmitterInput(const Ui::CanvasInput& input, Ui::UiRect canvasBounds) {
        if(UserEmitter) {
            UserEmitter->Enabled = input.IsActivate;
            UserEmitter->Position = input.MouseScreen;
        }

        auto center = ImVec2{Graphics::ScreenWidth * 0.5f, canvasBounds.Min.y + 50.f};
        auto top = ImVec2{Graphics::ScreenWidth * 0.5f, canvasBounds.Min.y};

        for(size_t i = 0; i < Presets.size(); ++i) {
            auto* emitter = PresetEmitters.at(i);
            if(!emitter) continue;
            const auto& preset = Presets.at(i);
            const bool held = ImGui::IsKeyDown(preset.Key);
            emitter->Enabled = held;
            if(!held) continue;
            switch(preset.PosMode) {
                using enum PositionMode;
                case FollowMouse: emitter->Position = input.IsHovered ? input.MouseScreen : center; break;
                case TopOfScreen: emitter->Position = top; break;
            }
        }
    }

    Ui::EmitterSettings ToEmitterSettings(const EmitterEditorState& state) {
        auto halfSpread = Constexpr::DegToRad(state.AngleSpreadDeg) * 0.5f;
        auto center = Constexpr::DegToRad(state.AngleCenterDeg);
        return {
            .Gravity = state.ParticleGravity,
            .RatePerSecond = state.RatePerSecond,
            .AngleMin = center - halfSpread,
            .AngleMax = center + halfSpread,
            .SpeedMin = state.SpeedRange.at(0),
            .SpeedMax = state.SpeedRange.at(1),
            .LifeMinMs = static_cast<u16>(state.LifeRangeMs.at(0)),
            .LifeMaxMs = static_cast<u16>(state.LifeRangeMs.at(1)),
            .Size = state.ParticleSize,
            .Color = ImGui::ColorConvertFloat4ToU32(state.ParticleColor)
        };
    }

    void SyncEmitterFromUi() {
        if(!UserEmitter) return;
        UserEmitter->Settings = ToEmitterSettings(UserEmitterEditor);
    }

    void RebuildSystem() {
        UserEmitter = nullptr;
        PresetEmitters.fill(nullptr);

        Particles = std::make_unique<::Ui::ParticleSystem>(static_cast<size_t>(Capacity));
        UserEmitter = &Particles->AddEmitter();
        SyncEmitterFromUi();

        for(size_t i = 0; i < Presets.size(); ++i) {
            const auto& preset = Presets.at(i);
            auto settings = preset.Settings;
            Ui::SetArc(settings, preset.Direction, preset.Spread);
            settings.PositionJitter.x = preset.JitterFracX * Graphics::ScreenWidth;
            PresetEmitters.at(i) = &Particles->AddEmitter(settings);
        }
    }

    void RenderControls() {
        ImGui::PushFont(GetFont(FontSizes::H4));
        ImGui::SetCursorPosY(ControlsOffsetY);

        ImGui::SliderFloat("Rate (per sec)", &UserEmitterEditor.RatePerSecond, 0.f, 4000.f, "%.0f");
        ImGui::SliderFloat2("Speed (px/s)", UserEmitterEditor.SpeedRange.data(), 0.f, 800.f, "%.0f");
        UserEmitterEditor.SpeedRange.at(1) = std::max(UserEmitterEditor.SpeedRange.at(0), UserEmitterEditor.SpeedRange.at(1));

        ImGui::SliderFloat("Angle Center", &UserEmitterEditor.AngleCenterDeg, 0.f, 360.f, "%.0f");
        ImGui::SliderFloat("Angle Spread", &UserEmitterEditor.AngleSpreadDeg, 0.f, 360.f, "%.0f");
        ImGui::SliderInt2("Life", UserEmitterEditor.LifeRangeMs.data(), 50, 5'000);
        UserEmitterEditor.LifeRangeMs.at(1) = std::max(UserEmitterEditor.LifeRangeMs.at(0), UserEmitterEditor.LifeRangeMs.at(1));

        ImGui::SliderFloat("Size", &UserEmitterEditor.ParticleSize, 1.f, 24.f, "%.1f");
        ImGui::ColorEdit4("Color", &UserEmitterEditor.ParticleColor.x);
        ImGui::SliderFloat2("Gravity", &UserEmitterEditor.ParticleGravity.x, -1000.f, 1000.f, "%.0f");

        if(ImGui::Button("Clear")) {
            if(Particles) Particles->Clear();
        }
        ImGui::SameLine();
        ImGui::Text(
            "Active: %zu / %zu", Particles ? Particles->ParticleCount() : 0u, Particles ? Particles->ParticleCapacity() : 0u
        );
        ImGui::SameLine();
        // TODO: remove once a dedicated SampleDialog screen exists.
        if(ImGui::Button("Show Dialog") && !::Ui::Dialog::IsActive()) {
            ::Ui::Dialog::Show({
                .Text = "This is a test dialog. Click once to fast-forward the reveal, "
                        "then click again to dismiss. Try holding the mouse on the "
                        "canvas after closing -- particles should still respond.",
                .CharsPerSecond = 45.f,
            });
        }

        ImGui::SliderInt("Capacity", &PendingCapacity, MinCapacity, MaxCapacity, "%d", ImGuiSliderFlags_Logarithmic);
        ImGui::SameLine();
        if(ImGui::Button("Recreate")) {
            Capacity = PendingCapacity;
            RebuildSystem();
        }

        ImGui::TextUnformatted("Hold: [LMB] User Emitter");
        for(size_t i = 0; i < Presets.size(); ++i) {
            ImGui::SameLine();
            ImGui::Text("  [%zu] %s", i + 1, Presets.at(i).Name);
        }

        ImGui::PopFont();

        SyncEmitterFromUi();
    }

    void RenderContent() {
        if(!Canvas) return;

        const auto canvasTop = ImGui::GetCursorPosY() + CanvasTopMargin;
        auto canvasBounds = Ui::UiRect{ImVec2{0.f, canvasTop}, ImVec2{Graphics::ScreenWidth, Graphics::ScreenHeight}};

        Canvas->SetBounds(canvasBounds);
        Canvas->Render();
        UpdateEmitterInput(Canvas->GetInput(), canvasBounds);
    }
}

namespace SampleUI::Screens::SampleParticles {
    bool Initialize() {
        RebuildSystem();
        Canvas = std::make_unique<Ui::CanvasPanel>(Ui::PanelConfig{}, [](Ui::CanvasPanel&) {
            if(Particles) Particles->Render();
        });
        TickManager::Get().Register(TickHandles, [](BaseTime elapsed) {
            if(Particles) Particles->Update(elapsed);
        });
        return true;
    }

    void ShutDown() {
        TickHandles.clear();
        PresetEmitters.fill(nullptr);
        UserEmitter = nullptr;
        Particles.reset();
        Canvas.reset();
    }

    void Render() {
        RenderSampleScreen("Sample Particles", [] {
            RenderControls();
            RenderContent();
        });
    }
} // namespace SampleUI::Screens::SampleParticles
