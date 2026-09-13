#include "Platform/Audio.h"
#include "Platform/Platform.h"

#include <imgui.h>
#include <miniaudio.h>

#include <cmath>
#include <numbers>
#include <string>
#include <utility>
#include <vector>

namespace {
    Platform* PlatformPtr{nullptr};
    ma_engine Engine{};
    bool AudioSystemInitialized{};

    std::vector<Audio::AudioHandle> PlayOnceHandles{};
}

namespace Audio {
    struct SoundData {
        std::vector<float> Samples{};
        ma_uint32 Channels{};
        ma_uint32 SampleRate{};
    };

    struct AudioHandle::Impl {
        std::shared_ptr<const SoundData> Data;
        ma_audio_buffer Buffer{};
        ma_sound Voice{};
        bool BufferInitialized{};
        bool VoiceInitialized{};

        bool Initialize(std::shared_ptr<const SoundData> data, bool loop, float volume) {
            if(!AudioSystemInitialized || !data || data->Samples.empty() || data->Channels == 0 || data->SampleRate == 0) {
                return false;
            }

            Data = std::move(data);
            auto frameCount = Data->Samples.size() / Data->Channels;

            auto config = ma_audio_buffer_config_init(ma_format_f32, Data->Channels, frameCount, Data->Samples.data(), nullptr);
            config.sampleRate = Data->SampleRate;

            if(ma_audio_buffer_init(&config, &Buffer) != MA_SUCCESS) return false;
            BufferInitialized = true;

            if(ma_sound_init_from_data_source(&Engine, &Buffer, MA_SOUND_FLAG_NO_SPATIALIZATION, nullptr, &Voice) != MA_SUCCESS) {
                return false;
            }
            VoiceInitialized = true;

            ma_sound_set_looping(&Voice, loop ? MA_TRUE : MA_FALSE);
            ma_sound_set_volume(&Voice, volume);
            return ma_sound_start(&Voice) == MA_SUCCESS;
        }

        void SetVolume(float volume) {
            if(VoiceInitialized) {
                ma_sound_set_volume(&Voice, volume);
            }
        }

        bool IsDone() const { 
            return !VoiceInitialized || ma_sound_at_end(&Voice) == MA_TRUE;
        }

        ~Impl() {
            if(VoiceInitialized) ma_sound_uninit(&Voice);
            if(BufferInitialized) ma_audio_buffer_uninit(&Buffer);
        }
    };

    Duration Sound::GetDuration() const {
        if(!m_Data || m_Data->Channels == 0 || m_Data->SampleRate == 0) return Duration::zero();

        auto frames = m_Data->Samples.size() / m_Data->Channels;
        return Duration{static_cast<double>(frames) / m_Data->SampleRate};
    }

    void Sound::PlayOnce(float volume) const { 
        PlayOnceHandles.emplace_back(Play(volume)); 
    }

    AudioHandle Sound::Play(float volume) const {
        auto impl = std::make_unique<AudioHandle::Impl>();
        if(!impl->Initialize(m_Data, false, volume)) return {};

        return AudioHandle(std::move(impl));
    }

    AudioHandle Sound::Loop(float volume) const {
        auto impl = std::make_unique<AudioHandle::Impl>();
        if(!impl->Initialize(m_Data, true, volume)) return {};

        return AudioHandle(std::move(impl));
    }

    AudioHandle::AudioHandle() = default;
    AudioHandle::~AudioHandle() = default;

    AudioHandle::AudioHandle(AudioHandle&&) noexcept = default;
    AudioHandle& AudioHandle::operator=(AudioHandle&&) noexcept = default;

    AudioHandle::AudioHandle(std::unique_ptr<Impl> impl) : m_Impl(std::move(impl)){}
    void AudioHandle::Stop() { m_Impl.reset(); }
    bool AudioHandle::IsDone() const {
        return !m_Impl || m_Impl->IsDone();
    }
    void AudioHandle::SetVolume(float volume) {
        if(m_Impl) m_Impl->SetVolume(volume);
    }

    bool Initialize(Platform& platform) {
        if(AudioSystemInitialized) return true;

        PlatformPtr = &platform;
        auto config = ma_engine_config_init();
        AudioSystemInitialized = ma_engine_init(&config, &Engine) == MA_SUCCESS;
        return AudioSystemInitialized;
    }

    void Shutdown() {
        if(!AudioSystemInitialized) return;

        PlayOnceHandles.clear();

        ma_engine_uninit(&Engine);
        AudioSystemInitialized = false;
        PlatformPtr = nullptr;
    }

    void Update() {
        std::erase_if(PlayOnceHandles, [](const AudioHandle& handle) {
            return handle.IsDone(); 
        });
    }

    Sound LoadSound(std::string_view soundName) {
        if(!AudioSystemInitialized || !PlatformPtr) return {};

        auto fileName = std::string(soundName);
        void* assetBytes = nullptr;
        size_t assetSize = 0;
        auto loaded = PlatformPtr->TryGetAsset(fileName.c_str(), &assetBytes, assetSize);

        auto freeAsset = [](void* memory) { IM_FREE(memory); };
        std::unique_ptr<void, decltype(freeAsset)> asset(assetBytes, freeAsset);

        if(!loaded || !asset || assetSize == 0) return {};

        auto config = ma_decoder_config_init(ma_format_f32, 0, 0);
        ma_uint64 frameCount = 0;
        void* decodedBytes = nullptr;
        auto result = ma_decode_memory(asset.get(), assetSize, &config, &frameCount, &decodedBytes);

        auto freeDecoded = [](void* memory) { ma_free(memory, nullptr); };
        std::unique_ptr<void, decltype(freeDecoded)> decoded(decodedBytes, freeDecoded);

        if(result != MA_SUCCESS || !decoded || frameCount == 0) return {};

        auto data = std::make_shared<SoundData>();
        data->Channels = config.channels;
        data->SampleRate = config.sampleRate;

        auto sampleCount = static_cast<size_t>(frameCount) * data->Channels;
        auto* samples = static_cast<const float*>(decoded.get());
        data->Samples.assign(samples, samples + sampleCount);

        Sound sound;
        sound.m_Data = std::move(data);
        return sound;
    }

    template<typename TWaveform>
    Sound ToneGenerator::Generate(float hz, Duration duration, TWaveform waveform) {
        auto sampleRate = ma_uint32{48'000};
        auto seconds = duration.count();

        if(!std::isfinite(hz) || hz <= 0.f || hz >= sampleRate * 0.5f || !std::isfinite(seconds) || seconds <= 0.0) {
            return {};
        }

        auto data = std::make_shared<SoundData>();
        auto frames = seconds * sampleRate;
        if(frames < 1.0 || frames >= static_cast<double>(data->Samples.max_size())) {
            return {};
        }

        data->Channels = 1;
        data->SampleRate = sampleRate;
        data->Samples.resize(static_cast<size_t>(frames));

        auto phaseStep = 2.0 * std::numbers::pi * static_cast<double>(hz) / sampleRate;

        for(size_t i = 0; i < data->Samples.size(); i++) {
            auto phase = phaseStep * static_cast<double>(i);
            data->Samples[i] = static_cast<float>(waveform(phase));
        }

        Sound sound;
        sound.m_Data = std::move(data);
        return sound;
    }

    Sound ToneGenerator::Pulse(float hz, Duration duration, float dutyCycle) {
        if(dutyCycle <= 0.f || dutyCycle >= 1.f) return {};
        return Generate(hz, duration, [dutyCycle](double phase) {
            auto cycle = phase / (2.0 * std::numbers::pi);
            auto fraction = cycle - std::floor(cycle);
            return fraction < dutyCycle ? 1.0 : -1.0;
        });
    }

    Sound ToneGenerator::Sine(float hz, Duration duration) {
        return Generate(hz, duration, [](double phase) { 
            return std::sin(phase); 
        });
    }

    Sound ToneGenerator::Square(float hz, Duration duration) {
        return Pulse(hz, duration, 0.5f);
    }

    Sound ToneGenerator::Sawtooth(float hz, Duration duration) {
        return Generate(hz, duration, [](double phase) {
            auto cycle = phase / (2.0 * std::numbers::pi);
            auto fraction = cycle - std::floor(cycle);
            return 2.0 * fraction - 1.0;
        });
    }

    Sound ToneGenerator::Triangle(float hz, Duration duration) {
        return Generate(hz, duration, [](double phase) {
            auto cycle = phase / (2.0 * std::numbers::pi);
            auto fraction = cycle - std::floor(cycle);
            return 1.0 - 4.0 * std::abs(fraction - 0.5);
        });
    }

}