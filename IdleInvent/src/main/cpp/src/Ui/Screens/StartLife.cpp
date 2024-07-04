#include "Ui/Screens/StartLife.h"

#include "Ui/Ui.h"
#include "Ui/Screens/Screens.h"

#include "InventLib/Resources/Resource.h"
#include "InventLib/Character/Life.h"

#include "Core/DesignPatterns/ServiceLocator.h"

namespace Ui::Screens::StartLife {
    bool Initialize() {
        return true;
    }

    void ShutDown() {
    }

    void Render() {
        ImGui::Text("Welcome to Idle Invent!");
        ImGui::Text("Select your specialty and let's begin!");
        int selected = 0;
        for(const auto& resource : Invent::SecondaryResources) {
            ImGui::RadioButton(Invent::ToString(resource).c_str(), &selected, static_cast<int>(resource));
        }

        if(selected > 0) {
            if(ImGui::Button("Start")) {
                auto& life = ServiceLocator::Get().GetRequired<Invent::Life>();
                life.Specialty = static_cast<Invent::ResourceName>(selected);
                Screens::SetActiveScreen(Screen::Home);
            }
        }
    }
}