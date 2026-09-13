#pragma once

#include <chrono>
#include <memory>
#include <string_view>

struct Platform;

namespace Audio {
    using Duration = std::chrono::duration<double>;

    class AudioHandle {
    public:
        AudioHandle();
        ~AudioHandle();
        AudioHandle(const AudioHandle&) = delete;
        AudioHandle& operator=(const AudioHandle&) = delete;

        AudioHandle(AudioHandle&&) noexcept;
        AudioHandle& operator=(AudioHandle&&) noexcept;

        bool IsDone() const;
        void Stop();
        void SetVolume(float volume);

    private:
        struct Impl;
        std::unique_ptr<Impl> m_Impl;

        explicit AudioHandle(std::unique_ptr<Impl> impl);
        friend struct Sound;
    };

    struct SoundData;
    struct Sound {
        Duration GetDuration() const;
        void PlayOnce(float volume = 1.f) const;
        AudioHandle Play(float volume = 1.f) const;
        AudioHandle Loop(float volume = 1.f) const;

    private:
        std::shared_ptr<const SoundData> m_Data;
        friend Sound LoadSound(std::string_view soundName);
        friend struct ToneGenerator;
    };

    bool Initialize(Platform& platform);
    void Shutdown();

    Sound LoadSound(std::string_view soundName);
    void Update();

    struct ToneGenerator {
        static Sound Sine(float hz, Duration duration);
        static Sound Square(float hz, Duration duration);
        static Sound Sawtooth(float hz, Duration duration);
        static Sound Triangle(float hz, Duration duration);
        static Sound Pulse(float hz, Duration duration, float dutyCycle = 0.5f);

    private:
        template<typename TWaveform>
        static Sound Generate(float hz, Duration duration, TWaveform waveform);
    };
}