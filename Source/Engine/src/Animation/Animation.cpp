#include "Animation/Animation.h"
#include "Instrumentation/Logging.h"
#include "Platform/Graphics.h"

#include <charconv>
#include <filesystem>
#include <map>
#include <optional>
#include <string_view>
#include <utility>

namespace {
    constexpr std::optional<std::pair<std::string_view, size_t>> TrySplitFrame(std::string_view name) {
        size_t digitCount = name.size();
        while(digitCount > 0) {
            auto c = name[digitCount - 1];
            if(c < '0' || c > '9') break;
            --digitCount;
        }
        if(digitCount == name.size()) return std::nullopt; // no trailing digits
        if(digitCount == 0) return std::nullopt; // name is only digits, no prefix

        auto num = name.substr(digitCount);
        size_t frame;
        const auto [_, ec] = std::from_chars(num.data(), num.data() + num.size(), frame);
        if(ec != std::errc{}) return std::nullopt; // not a valid number

        return std::make_pair(name.substr(0, digitCount), frame);
    }

    static_assert(TrySplitFrame("Hero0").has_value());
    static_assert(TrySplitFrame("Hero0")->first == "Hero");
    static_assert(TrySplitFrame("Hero0")->second == 0);
    static_assert(TrySplitFrame("Hero").has_value() == false);
    static_assert(TrySplitFrame("24").has_value() == false);
    static_assert(TrySplitFrame("Level2Boss0")->first == "Level2Boss");
    static_assert(TrySplitFrame("Level2Boss0")->second == 0);
    static_assert(TrySplitFrame("Agent007")->second == 7);
}

bool TryLoadAnimations(const std::string& sheetName, std::unordered_map<std::string, Animation>& outResult) {
    auto path = std::filesystem::path(sheetName);
    std::string imgPath, txtPath;
    if(path.has_extension()) {
        imgPath = path.string();
        txtPath = path.replace_extension("txt").string();
    } else {
        imgPath = path.string() + ".png";
        txtPath = path.string() + ".txt";
    }
    std::vector<SpriteRegion> regions;
    if(!Graphics::TryLoadSpriteRegions(txtPath, regions)) return false;
    if(!Graphics::TryLoadSpriteSheet(imgPath, regions)) return false;

    std::unordered_map<std::string, std::map<size_t, Sprite>> staging;
    for(const auto& region : regions) {
        auto split = TrySplitFrame(region.Name);
        if(!split.has_value()) continue;
        const auto& [prefix, frame] = split.value();
        auto [_, inserted] = staging[std::string(prefix)].try_emplace(frame, Graphics::GetSprite(region.Name));
        if(!inserted) {
            DR_ASSERT_MSG(inserted, std::format("Duplicate frame detected: {}{}", prefix, frame));
            return false;
        }
    }

    // don't clobber outResult until everything looks good
    std::unordered_map<std::string, Animation> result;
    for(auto& [prefix, frames] : staging) {
        Animation anim;
        anim.Frames.reserve(frames.size());
        size_t expected = 0;
        for(auto& [index, sprite] : frames) {
            if(index != expected) {
                DR_ASSERT_MSG(index == expected, std::format("Non-sequential frame index detected in animation '{}': expected {}, got {}", prefix, expected, index));
                return false;
            }
            anim.Frames.push_back(sprite);
            ++expected;
        }
        result.emplace(prefix, std::move(anim));
    }

    for(auto& [prefix, anim] : result) {
        outResult.insert_or_assign(prefix, std::move(anim));
    }

    return true;
}

AnimationPlayer::AnimationPlayer(const Animation& animation, BaseTime duration, bool loop) 
	: m_Animation(&animation)
	, m_Loop(loop)
	, m_Duration(duration) {
    DR_ASSERT(animation.Frames.size() > 0);
    DR_ASSERT(static_cast<size_t>(m_Duration.count()) >= animation.Frames.size());
}

void AnimationPlayer::Tick(BaseTime elapsed) {
    if(!m_Playing || IsFinished()) return;

	m_Elapsed += BaseTime{ static_cast<u64>(elapsed.count() * m_Speed) };
    auto n = m_Animation->Frames.size();
	if(m_Loop) {
        m_Elapsed %= m_Duration;
	} else if(m_Elapsed >= m_Duration) {
        m_Frame = n;
        return;
	}
	
	m_Frame = static_cast<size_t>((m_Elapsed.count() * n) / m_Duration.count());
}

Sprite AnimationPlayer::Current() const { 
	DR_ASSERT(m_Frame < m_Animation->Frames.size());
	return m_Animation->Frames[m_Frame]; 
}

void AnimationPlayer::SetPlaybackSpeed(double speed)
{
    DR_ASSERT(speed > 0.0);
    DR_ASSERT(speed < 1000.0); // sanity check
    m_Speed = speed;
}

void AnimationPlayer::Play() { m_Playing = true; }
void AnimationPlayer::Pause() { m_Playing = false; }
void AnimationPlayer::TogglePlaying() { m_Playing = !m_Playing; }

void AnimationPlayer::Show() { m_Visible = true; }
void AnimationPlayer::Hide() { m_Visible = false; }
void AnimationPlayer::ToggleVisible() { m_Visible = !m_Visible; }

void AnimationPlayer::Reset() {
    m_Elapsed = BaseTime{0};
	m_Frame = 0;
    m_Speed = 1.0;
}

bool AnimationPlayer::IsFinished() const { return m_Frame >= m_Animation->Frames.size(); }