#include "Pets/Ui/Ui.h"
#include <Animation/Animation.h>
#include <Manage/TickManager.h>
#include <Instrumentation/Logging.h>
#include <DesignPatterns/ServiceLocator.h>
#include <Ui/UiUtil.h>

namespace {
    std::vector<AnimationPlayer> g_Anims;
    std::vector<ScopedHandle> g_Handles;

	constexpr auto HeroSheet = "Animation/Hero.png";
    constexpr ImVec2 ImageSize{64, 64};
}

namespace Pets::Ui {
	bool Initialize() {
        InitializeFonts("DroidSans.ttf");

        auto& animSource = ServiceLocator::Get().GetRequired<std::unordered_map<std::string, Animation>>();
        if(!TryLoadAnimations(HeroSheet, animSource)) {
            Log::Error(std::format("Failed to load animations for sheet: {}", HeroSheet));
            return false;
        }
        g_Anims.emplace_back(AnimationPlayer::PlayNew(animSource.at("Hero"), OneSecond, true));
        TickManager::Get().Register(g_Handles, [](BaseTime elapsed) { 
            for(auto& player: g_Anims) {
                player.Tick(elapsed);
            }
        });
		return true;
	}

	void Render() {
		ImGui::Begin("PetPyramid");
        for(const auto& player: g_Anims) {
            auto frame = player.Current();
            ImGui::Image(frame.Texture, ImageSize, frame.UvMin, frame.UvMax);
        }
		ImGui::End();
	}

	void ShutDown() { 
        g_Handles.clear();
        g_Anims.clear();
	}

    ImFont* GetFont(FontSizes fontSize) {
        switch(fontSize) {
            using enum FontSizes;
        case H1: return Graphics::GetFont("H1");
        case H2: return Graphics::GetFont("H2");
        case H3: return Graphics::GetFont("H3");
        case H4: return Graphics::GetFont("H4");
        default: return nullptr;
        }
    }

} // namespace Pets::Ui
