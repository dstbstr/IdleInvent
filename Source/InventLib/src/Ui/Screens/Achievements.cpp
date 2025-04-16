#include "InventLib/Ui/Screens/Achievements.h"
#include "InventLib/Ui/Ui.h"

#include <DesignPatterns/ServiceLocator.h>
#include <DesignPatterns/PubSub.h>
#include <Mechanics/Achievement.h>
#include <imgui.h>

namespace {
    std::vector<Achievement> unlocked{};
}

namespace Ui::Screens::Achievements {
    bool Initialize() {
        ServiceLocator::Get().GetRequired<PubSub<Achievement>>().Subscribe([](const Achievement& achievement){
            unlocked.push_back(achievement);
        });
        return true;
    }

    void ShutDown() {}

    void Render() {
        ImGui::Begin("AchievementsScreen", nullptr, BaseFlags);
        for(const auto& achievement : unlocked) {
            if(ImGui::CollapsingHeader(achievement.Name.c_str())) {
                ImGui::TextWrapped("%s", achievement.Description.c_str());
            }
        }
        ImGui::End();
    }
}