#include "Ui/Screens/Achievements.h"

#include "Ui/Ui.h"

#include "InventLib/Mechanics/Achievement.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"

#include "imgui.h"

namespace {
    std::vector<Invent::Achievement> unlocked{};
}

namespace Ui::Screens::Achievements {
    bool Initialize() {
        ServiceLocator::Get().GetRequired<PubSub<Invent::Achievement>>().Subscribe([](const Invent::Achievement& achievement){
            unlocked.push_back(achievement);
        });
        return true;
    }

    void ShutDown() {}

    void Render() {
        ImGui::Begin("AchievementsScreen", nullptr, BaseFlags);
        for(const auto& achievement : unlocked) {
            ImGui::TextWrapped("%s", std::format("{}: {}", achievement.Name, achievement.Description).c_str());
        }
        ImGui::End();
    }
}