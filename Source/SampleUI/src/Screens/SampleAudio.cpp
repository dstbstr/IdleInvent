#include "SampleUI/Screens/SampleAudio.h"
#include "SampleUI/Screens/SampleScreen.h"

#include <Platform/Audio.h>

#include <imgui.h>

namespace {
    enum struct Waveform {
        Sine,
        Square,
        Sawtooth,
        Triangle,
        Pulse
    };
    f32 Volume = 0.1f;
    f32 Hz = 440.f;
    f32 Duration = 1.f;
    f32 DutyCycle = 0.5f;
    bool Looping = false;
    int SelectedWaveform = static_cast<int>(Waveform::Sine);

    Audio::Sound Tone{};
    Audio::AudioHandle Voice{};

    Audio::Sound ButtonClick{};
    Audio::Sound CoinDrop{};
}

namespace SampleUI::Screens::SampleAudio {
    bool Initialize() { 
        ButtonClick = Audio::LoadSound("Sfx/ButtonClick.mp3");
        CoinDrop = Audio::LoadSound("Sfx/DropCoin.mp3");
        return ButtonClick.GetDuration() > Audio::Duration::zero();
    }

    void ShutDown() { 
        Voice.Stop();
        Tone = {};
    }

    void Render() { 
        RenderSampleScreen("Sample Audio", [] {
            ImGui::Spacing();
            ImGui::PushFont(GetFont(FontSizes::H3));
            ImGui::SliderFloat("Frequency", &Hz, 10.f, 20'000.f, "%.0f Hz", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat("Duration", &Duration, 0.1f, 5.f, "%.2f s");
            auto waveformOptions = "Sine\0Square\0Sawtooth\0Triangle\0Pulse\0";
            ImGui::Combo("Waveform", &SelectedWaveform, waveformOptions);
            if(SelectedWaveform == static_cast<int>(Waveform::Pulse)) {
                ImGui::SliderFloat("Duty Cycle", &DutyCycle, 0.1f, 0.9f, "%.2f");
            }
            ImGui::Checkbox("Loop", &Looping);

            if(ImGui::SliderFloat("Volume", &Volume, 0.f, 1.f, "%.2f")) {
                Voice.SetVolume(Volume);
            }
            ImGui::TextUnformatted("Frequency, duration, looping take affect on next play");

            auto playing = !Voice.IsDone();
            if(ImGui::Button(playing ? "Stop" : "Play")) {
                if(playing) Voice.Stop();
                else {
                    auto form = static_cast<Waveform>(SelectedWaveform);
                    if(form == Waveform::Pulse) {
                        Tone = Audio::ToneGenerator::Pulse(Hz, Audio::Duration{Duration}, DutyCycle);
                    } else {
                        auto fn = Audio::ToneGenerator::Sine;
                        switch(form) {
                            using enum Waveform;
                            case Sine: fn = Audio::ToneGenerator::Sine; break;
                            case Square: fn = Audio::ToneGenerator::Square; break;
                            case Sawtooth: fn = Audio::ToneGenerator::Sawtooth; break;
                            case Triangle: fn = Audio::ToneGenerator::Triangle; break;
                        }
                        Tone = fn(Hz, Audio::Duration{Duration});
                    }

                    Voice = Looping ? Tone.Loop(Volume) : Tone.Play(Volume);
                }
            }

            if(ImGui::Button("Click")) ButtonClick.PlayOnce(0.2f);
            ImGui::SameLine();
            if(ImGui::Button("Coin Drop")) CoinDrop.PlayOnce(0.2f);
            

            ImGui::PopFont();
        });

        if(GetActiveScreen() != Screen::SampleAudio) {
            Voice.Stop();
        }
    }
} // namespace SampleUI::Screens::SampleAudio