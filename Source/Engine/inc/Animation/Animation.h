#pragma once

#include "Platform/Graphics.h"
#include "GameState/GameTime.h"

#include <string>
#include <unordered_map>
#include <vector>

struct Animation {
    std::vector<Sprite> Frames;
};

bool TryLoadAnimations(const std::string& sheetName, std::unordered_map<std::string, Animation>& outResult);

class AnimationPlayer {
public:
    AnimationPlayer(const Animation& animation, BaseTime duration = OneSecond, bool loop = true);
    AnimationPlayer(Animation&&, BaseTime, bool) = delete;
    
    static AnimationPlayer PlayNew(const Animation& animation, BaseTime duration = OneSecond, bool loop = true) {
        AnimationPlayer player(animation, duration, loop);
        player.Play();
        return player;
    }
    static AnimationPlayer PlayNew(Animation&&, BaseTime, bool) = delete;

    void Tick(BaseTime elapsed);
    [[nodiscard]] bool IsFinished() const;
    [[nodiscard]] Sprite Current() const;

    void Play();
    void Pause();
    void Reset();
    void TogglePlaying();

    void Show();
    void Hide();
    void ToggleVisible();

    void SetPlaybackSpeed(f32 speed);

private:
    const Animation* m_Animation;
    BaseTime m_Elapsed{0};
    BaseTime m_Duration{0};
    f32 m_Speed{1.0};
    bool m_Loop{true};
    bool m_Visible{true};
    bool m_Playing{false};
    u8 m_Frame{0};
};