#include "Ui/StartLife.h"

#include "Ui/Ui.h"
#include "Ui/Screens/Screens.h"
#include "Platform/Graphics.h"

#include "InventLib/Resources/Resource.h"
#include "InventLib/Character/Life.h"
#include "InventLib/Character/Society.h"

#include "Core/DesignPatterns/ServiceLocator.h"

namespace {
    Invent::Society* society = nullptr;
    Invent::GameSettings* gameSettings = nullptr;
    int specialty = 0;
    std::string specialtyDescription = "";
}

namespace Ui::StartLife {
    bool Initialize() {
        society = &ServiceLocator::Get().GetRequired<Invent::Society>();
        gameSettings = &ServiceLocator::Get().GetRequired<Invent::GameSettings>();
        return true;
    }

    void ShutDown() {
    }

    void Render() {
        ImGui::SetNextWindowPos(ImVec2(0, 100));
        ImGui::SetNextWindowSize(ImVec2(Graphics::ScreenWidth, Graphics::ScreenHeight));
        auto buttonSize = ImVec2(Graphics::ScreenWidth * 0.9F, 50);

        ImGui::Begin("StartLife", nullptr, BaseFlags);
        ImGui::Text("Welcome to Idle Invent!");
        ImGui::Text("Select your specialty and let's begin!");
        for(const auto& resource : Invent::SecondaryResources()) {
            if(ImGui::Button(Invent::ToString(resource).c_str(), buttonSize)) {
                if(specialty == static_cast<int>(resource)) {
                    specialty = 0;
                    specialtyDescription = "";
                } else {
                    specialty = static_cast<int>(resource);
                    specialtyDescription = Invent::DescribeSociety(resource);
                }
            }
        }

        if(specialty > 0) {
            ImGui::TextWrapped("%s", specialtyDescription.c_str());
            if(ImGui::Button("Start", buttonSize)) {
                society->Start(static_cast<Invent::ResourceName>(specialty), *gameSettings);
                //cheating
                for(auto resource : Invent::SecondaryResources()) {
                    society->CurrentLife.Resources[resource].Current = 10000;
                    society->CurrentLife.Resources[resource].Capacity = 10000;
                }
                society->CurrentLife.MaxPopulation = 1000;
                society->CurrentLife.CurrentPopulation = society->CurrentLife.MaxPopulation;
                society->CurrentLife.MaxWorkers = 1000;
                society->CurrentLife.AvailableWorkers = society->CurrentLife.MaxWorkers;
                //society->CurrentLife.CurrentPopulation = 5;
                //society->HasWorkerShift = true;
                //society->HasConvertAll = true;
                //society->HasInvestAll = true;
            }
        }

        ImGui::End();
    }

    bool IsDone() {
        return society->Active;
    }
}