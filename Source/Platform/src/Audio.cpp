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
    float MasterVolume{1.f};
    float MusicVolume{1.f};
    float SfxVolume{1.f};
    bool Muted{false};

    std::vector<Audio::AudioHandle> PlayOnceHandles{};
    ma_sound_group MusicGroup{};
    ma_sound_group SfxGroup{};

    ma_sound_group* GetGroup(Audio::Kind kind) { return kind == Audio::Kind::Music ? &MusicGroup : &SfxGroup; }

    void ApplyVolumes() {
        if(!AudioSystemInitialized) return;

        ma_engine_set_volume(&Engine, Muted ? 0.f : MasterVolume);
        ma_sound_group_set_volume(&MusicGroup, MusicVolume);
        ma_sound_group_set_volume(&SfxGroup, SfxVolume);
    }
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

        bool Initialize(std::shared_ptr<const SoundData> data, bool loop, Kind kind) {
            if(!AudioSystemInitialized || !data || data->Samples.empty() || data->Channels == 0 || data->SampleRate == 0) {
                return false;
            }

            Data = std::move(data);
            auto frameCount = Data->Samples.size() / Data->Channels;

            auto config = ma_audio_buffer_config_init(ma_format_f32, Data->Channels, frameCount, Data->Samples.data(), nullptr);
            config.sampleRate = Data->SampleRate;

            if(ma_audio_buffer_init(&config, &Buffer) != MA_SUCCESS) return false;
            BufferInitialized = true;

            auto* group = GetGroup(kind);
            if(ma_sound_init_from_data_source(&Engine, &Buffer, MA_SOUND_FLAG_NO_SPATIALIZATION, group, &Voice) != MA_SUCCESS) {
                return false;
            }
            VoiceInitialized = true;

            ma_sound_set_looping(&Voice, loop ? MA_TRUE : MA_FALSE);
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

    void Sound::PlayOnce() const { 
        PlayOnceHandles.emplace_back(Play()); 
    }

    AudioHandle Sound::Play() const {
        auto impl = std::make_unique<AudioHandle::Impl>();
        if(!impl->Initialize(m_Data, false, m_Kind)) return {};

        return AudioHandle(std::move(impl));
    }

    AudioHandle Sound::Loop() const {
        auto impl = std::make_unique<AudioHandle::Impl>();
        if(!impl->Initialize(m_Data, true, m_Kind)) return {};

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

        auto config = ma_engine_config_init();
        if(ma_engine_init(&config, &Engine) != MA_SUCCESS) return false;
        
        auto flags = MA_SOUND_FLAG_NO_SPATIALIZATION;
        if(ma_sound_group_init(&Engine, flags, nullptr, &MusicGroup) != MA_SUCCESS) {
            ma_engine_uninit(&Engine);
            return false;
        }
        if(ma_sound_group_init(&Engine, flags, nullptr, &SfxGroup) != MA_SUCCESS) {
            ma_sound_group_uninit(&MusicGroup);
            ma_engine_uninit(&Engine);
            return false;
        }

        PlatformPtr = &platform;
        AudioSystemInitialized = true;
        ApplyVolumes();
        return true;
    }

    void Shutdown() {
        if(!AudioSystemInitialized) return;

        PlayOnceHandles.clear();

        ma_sound_group_uninit(&SfxGroup);
        ma_sound_group_uninit(&MusicGroup);
        ma_engine_uninit(&Engine);
        AudioSystemInitialized = false;
        PlatformPtr = nullptr;
    }

    void Update() {
        std::erase_if(PlayOnceHandles, [](const AudioHandle& handle) {
            return handle.IsDone(); 
        });
    }

    void SetMasterVolume(float volume) { 
        MasterVolume = volume; 
        ApplyVolumes();
    }

    void SetKindVolume(Kind kind, float volume) {
        switch(kind) {
            using enum Kind;
            case Music: MusicVolume = volume; break;
            case Sfx: SfxVolume = volume; break;
        }

        ApplyVolumes();
    }

    void SetMuted(bool muted) { 
        Muted = muted; 
        ApplyVolumes();
    }

    Sound LoadSound(std::string_view soundName, Kind kind) {
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
        sound.m_Kind = kind;
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
        sound.m_Kind = Kind::Music;
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