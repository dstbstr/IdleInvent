#include <SampleUI/Screens/SampleParticles.h>

#include <SampleUI/Screens/Screens.h>

#include <Manage/TickManager.h>
#include <Platform/Graphics.h>
#include <Ui/UiUtil.h>
#include <Ui/Particle/ParticleSystem.h>

#include <imgui.h>

#include <array>
#include <memory>
#include <numbers>
#include <vector>

namespace {
    constexpr auto HeaderOffsetY  = 32.f;
    constexpr auto ControlsOffsetY = 92.f;
    constexpr auto CanvasTopMargin = 8.f;
    constexpr size_t DefaultCapacity = 16384;
    constexpr s32 MinCapacity = 1024;
    constexpr s32 MaxCapacity = 131072;

    std::unique_ptr<Ui::ParticleSystem> s_System{};
    Ui::Emitter* s_UserEmitter = nullptr;
    std::vector<ScopedHandle> s_TickHandles{};
    s32 s_Capacity = static_cast<s32>(DefaultCapacity);
    s32 s_PendingCapacity = static_cast<s32>(DefaultCapacity);

    // Slider-backed values. Stored as floats here so ImGui can edit them directly; copied
    // back into the emitter every frame (cheap, and lets us round/clamp on the way in).
    f32  s_RatePerSecond = 400.f;
    auto s_SpeedRange = std::array{80.f, 220.f};
    f32  s_AngleCenterDeg = 270.f; // 270 == straight up in screen space (y down)
    f32  s_AngleSpreadDeg = 60.f;
    std::array<s32, 2> s_LifeRangeMs = {600, 1400};
    f32  s_Size = 4.f;
    ImVec4 s_Color = ImVec4(1.f, 0.7f, 0.2f, 1.f);
    ImVec2 s_Gravity = ImVec2(0.f, 200.f);

    // ---- Preset emitters (keys 1..PresetCount). ----
    enum struct PositionMode : u8 { FollowMouse, TopOfScreen };

    struct Preset {
        const char* Name {nullptr};
        ImGuiKey Key {};
        PositionMode PosMode{};
        Ui::Direction Direction{}; // applied via SetArc at rebuild time
        Ui::Spread Spread{};       // applied via SetArc at rebuild time
        f32 JitterFracX{0.f};      // fraction of Graphics::ScreenWidth applied to PositionJitter.x at build time
        Ui::Emitter Config{};        // copied into the ParticleSystem at (re)creation; AngleMin/AngleMax overwritten
    };

    // Notes on tuning:
    //  - SetArc(emitter, Direction, Spread) writes AngleMin/AngleMax from named values.
    //    Spread tiers: Laser ~5, Thin ~20, Narrow ~45, Wide ~90, Half 180, Full 360 deg.
    //  - PositionJitter on Snow makes flakes scatter across the screen width (set via
    //    JitterFracX since the screen size isn't known at static init time).
    //  - LifeMaxMs is what drives the alpha fade, so a longer LifeMax with similar LifeMin
    //    means "long fade tail".

    const std::array<Preset, 4> s_Presets{{
        // 1: Fountain -- upward arc, warm color, rides gravity
        Preset{
            .Name = "Fountain", .Key = ImGuiKey_1, .PosMode = PositionMode::FollowMouse,
            .Direction = Ui::Direction::N, .Spread = Ui::Spread::Narrow,
            .Config = {
                .RatePerSecond = 600.f,
                .SpeedMin = 200.f, .SpeedMax = 380.f,
                .LifeMinMs = 800, .LifeMaxMs = 1600,
                .Size = 3.f,
                .Color = IM_COL32(80, 180, 255, 255),
            }
        },
        // 2: Sparks -- fast, short-lived, omni, zero gravity look
        Preset{
            .Name = "Sparks", .Key = ImGuiKey_2, .PosMode = PositionMode::FollowMouse,
            .Direction = Ui::Direction::N, .Spread = Ui::Spread::Full,
            .Config = {
                .RatePerSecond = 1200.f,
                .SpeedMin = 350.f, .SpeedMax = 600.f,
                .LifeMinMs = 120, .LifeMaxMs = 280,
                .Size = 2.f,
                .Color = IM_COL32(255, 230, 120, 255),
            }
        },
        // 3: Snow -- slow downward drift, scattered across screen width via PositionJitter
        Preset{
            .Name = "Snow", .Key = ImGuiKey_3, .PosMode = PositionMode::TopOfScreen,
            .Direction = Ui::Direction::S, .Spread = Ui::Spread::Thin,
            .JitterFracX = 0.5f, // PositionJitter.x = ScreenWidth * 0.5 at rebuild time
            .Config = {
                .RatePerSecond = 200.f,
                .SpeedMin = 40.f, .SpeedMax = 90.f,
                .LifeMinMs = 3000, .LifeMaxMs = 5000,
                .Size = 3.f,
                .Color = IM_COL32(240, 240, 255, 255),
            }
        },
        // 4: Fireworks -- dense omni burst, gravity pulls them down for a falling-star look
        Preset{
            .Name = "Fireworks", .Key = ImGuiKey_4, .PosMode = PositionMode::FollowMouse,
            .Direction = Ui::Direction::N, .Spread = Ui::Spread::Full,
            .Config = {
                .RatePerSecond = 3000.f,
                .SpeedMin = 150.f, .SpeedMax = 420.f,
                .LifeMinMs = 500, .LifeMaxMs = 1200,
                .Size = 2.5f,
                .Color = IM_COL32(255, 120, 200, 255),
            }
        },
    }};

    // Indexed in lockstep with s_Presets. Reassigned every time the system is (re)built.
    std::array<Ui::Emitter*, s_Presets.size()> s_PresetEmitters{};

    f32 Deg2Rad(f32 d) { return d * (std::numbers::pi_v<f32> / 180.f); }

    void SyncEmitterFromUi() {
        if(!s_UserEmitter) return;
        s_UserEmitter->RatePerSecond = s_RatePerSecond;
        s_UserEmitter->SpeedMin      = s_SpeedRange.at(0);
        s_UserEmitter->SpeedMax      = s_SpeedRange.at(1);

        const f32 halfSpread = Deg2Rad(s_AngleSpreadDeg) * 0.5f;
        const f32 center     = Deg2Rad(s_AngleCenterDeg);
        s_UserEmitter->AngleMin  = center - halfSpread;
        s_UserEmitter->AngleMax  = center + halfSpread;

        s_UserEmitter->LifeMinMs = static_cast<u16>(s_LifeRangeMs.at(0));
        s_UserEmitter->LifeMaxMs = static_cast<u16>(s_LifeRangeMs.at(1));
        s_UserEmitter->Size      = s_Size;
        s_UserEmitter->Color     = ImGui::ColorConvertFloat4ToU32(s_Color);
    }

    // Build (or rebuild) the system at the current s_Capacity. User emitter is added first so
    // its EmitterId is 0; presets follow in s_Presets order so their ids are 1..N.
    void RebuildSystem() {
        s_System = std::make_unique<::Ui::ParticleSystem>(static_cast<size_t>(s_Capacity));
        s_UserEmitter = &s_System->AddEmitter();
        SyncEmitterFromUi();
        for(size_t i = 0; i < s_Presets.size(); ++i) {
            auto& emitter = s_System->AddEmitter(s_Presets.at(i).Config);
            // Resolve runtime-dependent fields here (ScreenWidth isn't valid at static init).
            Ui::SetArc(emitter, s_Presets.at(i).Direction, s_Presets.at(i).Spread);
            emitter.PositionJitter.x = s_Presets.at(i).JitterFracX * Graphics::ScreenWidth;
            s_PresetEmitters.at(i) = &emitter;
        }
    }
}

namespace SampleUI::Screens::SampleParticles {
    bool Initialize() {
        RebuildSystem();
        TickManager::Get().Register(s_TickHandles, [](BaseTime elapsed) {
            if(s_System) s_System->Update(elapsed);
        });
        return true;
    }

    void ShutDown() {
        s_TickHandles.clear(); // unregister before the system dies
        s_PresetEmitters.fill(nullptr);
        s_UserEmitter = nullptr;
        s_System.reset();
    }

    void Render() {
        ImGui::SetNextWindowPos({0.f, 0.f});
        ImGui::SetNextWindowSize({Graphics::ScreenWidth, Graphics::ScreenHeight});
        ImGui::Begin("Sample Particles", nullptr, BaseUiFlags);

        if(ImGui::Button("Back")) {
            Screens::SetActiveScreen(Screen::Landing);
            ImGui::End();
            return;
        }

        ImGui::SetCursorPosY(HeaderOffsetY);
        ImGui::PushFont(GetFont(FontSizes::H1));
        TextCenteredX("Particles Sample");
        ImGui::PopFont();

        ImGui::PushFont(GetFont(FontSizes::H4));
        ImGui::SetCursorPosY(ControlsOffsetY);

        ImGui::SliderFloat("Rate (per sec)", &s_RatePerSecond, 0.f, 4000.f, "%.0f");
        ImGui::SliderFloat2("Speed (px/s)", s_SpeedRange.data(), 0.f, 800.f, "%.0f");
        if(s_SpeedRange.at(1) < s_SpeedRange.at(0)) s_SpeedRange.at(1) = s_SpeedRange.at(0);
        ImGui::SliderFloat("Angle Center (deg)", &s_AngleCenterDeg, 0.f, 360.f, "%.0f");
        ImGui::SliderFloat("Angle Spread (deg)", &s_AngleSpreadDeg, 0.f, 360.f, "%.0f");
        ImGui::SliderInt2("Life (ms)", s_LifeRangeMs.data(), 50, 5000);
        if(s_LifeRangeMs.at(1) < s_LifeRangeMs.at(0)) s_LifeRangeMs.at(1) = s_LifeRangeMs.at(0);
        ImGui::SliderFloat("Size (px)", &s_Size, 1.f, 24.f, "%.1f");
        ImGui::ColorEdit4("Color", &s_Color.x);
        ImGui::SliderFloat2("Gravity (px/s\xc2\xb2)", &s_Gravity.x, -1000.f, 1000.f, "%.0f");

        if(ImGui::Button("Clear")) {
            if(s_System) s_System->Clear();
        }
        ImGui::SameLine();
        ImGui::Text("Active: %zu / %zu", s_System ? s_System->ParticleCount() : 0u,
                                          s_System ? s_System->ParticleCapacity() : 0u);

        ImGui::SliderInt("Capacity", &s_PendingCapacity, MinCapacity, MaxCapacity, "%d",
                         ImGuiSliderFlags_Logarithmic);
        ImGui::SameLine();
        if(ImGui::Button("Recreate")) {
            s_Capacity = s_PendingCapacity;
            RebuildSystem();
        }

        // Preset legend. Keys 1..N spawn the matching preset at the mouse (or top-of-screen).
        ImGui::TextUnformatted("Hold:");
        ImGui::SameLine();
        ImGui::TextUnformatted("[LMB] User Emitter");
        for(size_t i = 0; i < s_Presets.size(); ++i) {
            ImGui::SameLine();
            ImGui::Text("  [%zu] %s", i + 1, s_Presets.at(i).Name);
        }

        ImGui::PopFont();

        SyncEmitterFromUi();

        // Treat the rest of the window as the spawn canvas. Sliders/buttons sit above
        // canvasTop, so requiring the mouse to be inside the canvas naturally excludes
        // slider-dragging from triggering emission.
        const auto canvasTop = ImGui::GetCursorScreenPos().y + CanvasTopMargin;
        const ImVec2 mouse = ImGui::GetIO().MousePos;
        const bool mouseInCanvas = mouse.y >= canvasTop
                                && mouse.x >= 0.f && mouse.x <= Graphics::ScreenWidth
                                && mouse.y <= Graphics::ScreenHeight;

        if(s_UserEmitter) {
            s_UserEmitter->Enabled  = ImGui::IsMouseDown(ImGuiMouseButton_Left) && mouseInCanvas;
            s_UserEmitter->Position = mouse;
        }

        // Preset emitters: held key + (mostly) follow mouse. Multiple keys at once = layered.
        for(size_t i = 0; i < s_Presets.size(); ++i) {
            auto* emitter = s_PresetEmitters.at(i);
            if(!emitter) continue;
            const auto& preset = s_Presets.at(i);
            const bool held = ImGui::IsKeyDown(preset.Key);
            emitter->Enabled = held;
            if(!held) continue;
            switch(preset.PosMode) {
                using enum PositionMode;
                case FollowMouse:
                    emitter->Position = mouseInCanvas ? mouse : ImVec2{Graphics::ScreenWidth * 0.5f, canvasTop + 50.f};
                    break;
                case TopOfScreen:
                    emitter->Position = ImVec2{Graphics::ScreenWidth * 0.5f, canvasTop};
                    break;
            }
        }

        if(s_System) {
            s_System->SetGravity(s_Gravity);
            s_System->Render();
        }

        ImGui::End();
    }
} // namespace SampleUI::Ui::Screens::SampleParticles
