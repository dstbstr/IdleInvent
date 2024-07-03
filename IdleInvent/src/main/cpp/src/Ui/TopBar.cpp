#include "Ui/TopBar.h"
#include "Ui/Ui.h"
#include "Ui/Screens.h"

#include "InventLib/GameState/GameState.h"
#include "InventLib/Technology/TechAge.h"
#include "InventLib/Settings/GameSettings.h"

#include "Core/Constexpr/ConstexprStrUtils.h"
#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"
#include "Core/Instrumentation/Logging.h"

#include "imgui.h"

namespace {
    Invent::GameState* gameState {nullptr};
    Invent::GameSettings* gameSettings {nullptr};
    std::string era {"Stone Age"};
}

namespace Ui::TopBar {
    bool Initialize() {
        auto& services = ServiceLocator::Get();
        gameState = &services.GetRequired<Invent::GameState>();
        gameSettings = &services.GetOrCreate<Invent::GameSettings>();
        services.GetRequired<PubSub<Invent::TechAge>>().Subscribe([](const Invent::TechAge& age) {
            era = age.Name;
        });

        return true;
    }

    void ShutDown() {
        gameState = nullptr;
        gameSettings = nullptr;
    }

    void Render() {
        const auto& frameRate = ImGui::GetIO().Framerate;
        ImGui::Begin("TopBar", nullptr, BaseFlags);
        
        if(gameSettings->showFps){
            ImGui::SameLine(ImGui::GetContentRegionAvail().x / 2);
            ImGui::Text("%.1f FPS", frameRate);
        } else {
            ImGui::Text("");
        }

        /*
        ImGui::TableNextColumn();
        ImGui::Text("%s", std::format("Era: {}", era).c_str());

        ImGui::TableNextColumn();
        auto year = (gameState->CurrentRunElapsed / Invent::OneGameYear) - 3000;
        auto era = year < 0 ? "BC" : "AD";
        ImGui::Text("%s", std::format("Year: {}{}", std::abs(year), era).c_str());

        ImGui::TableNextColumn();
        //auto age = gameState->CurrentLife.CurrentAge / Invent::OneGameYear;
        //ImGui::Text("%s", std::format("Age: {}", age).c_str());

        //ImGui::TableNextColumn();

        if(gameSettings->showDeathDetails) {
            ImGui::Text("%s", std::format("You will die at age {}", gameState->Character.CharacterDeath.TimeOfDeath / Invent::OneGameYear).c_str());
        } else {
            if(gameState->Character.CharacterDeath.NearDeathData.HasWarned) {
                ImGui::Text("You will die soon");
            } else {
                ImGui::Text("%s", "You will die eventually");
            }
        }

        ImGui::EndTable();
         */
        ImGui::End();
    }
}