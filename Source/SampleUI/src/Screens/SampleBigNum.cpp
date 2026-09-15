#include "SampleUI/Screens/SampleBigNum.h"
#include "SampleUI/Screens/SampleScreen.h"

#include <Math/BigInt.h>
#include <DesignPatterns/ServiceLocator.h>
#include <Manage/TickManager.h>
#include <Utilities/Handle.h>

#include <imgui.h>
#include <vector>

namespace {
    enum struct Display {HumanReadable, Scientific};
    int SelectedDisplay = static_cast<int>(Display::HumanReadable);
    int Precision = 2;
    bool Compound = false;
    f32 CompoundRate = 1.01f;
    BaseTime elapsedTime = BaseTime::zero();
    BigInt Number{100};

    std::vector<ScopedHandle> Subs{};
} // namespace

namespace SampleUI::Screens::SampleBigNum {
    bool Initialize() {
        TickManager::Get().Register(Subs, [](BaseTime elapsed) { 
            if(!Compound) return;
            elapsedTime += elapsed;
            while(elapsedTime > OneSecond) {
                elapsedTime -= OneSecond;
                Number *= CompoundRate;
            }
        });

        return true;
    }

    void ShutDown() {
        Subs.clear();
    }

    void Render() {
        RenderSampleScreen("Sample Big Numbers", [] {
            ImGui::Spacing();
            ImGui::PushFont(GetFont(FontSizes::H3));
            ImGui::SliderInt("Precision", &Precision, 0, 10);
            ImGui::SliderFloat("Compound Rate", &CompoundRate, 1.f, 10.f, "%.2f");
            ImGui::Checkbox("Compound", &Compound);

            auto displays = "Human Readable\0Scientific\0";
            ImGui::Combo("Display", &SelectedDisplay, displays);

            auto txt = SelectedDisplay == static_cast<int>(Display::HumanReadable)
                    ? Number.ToHumanReadable(Precision).value_or(Number.ToScientific(Precision))
                    : Number.ToScientific(Precision);

            if(ImGui::Button("x2")) Number *= 2;
            ImGui::SameLine();
            if(ImGui::Button("x10")) Number *= 10;
            ImGui::SameLine();
            if(ImGui::Button("^2")) Number.Pow(2);
            ImGui::PopFont();
            ImGui::PushFont(GetFont(FontSizes::H1));
            ImGui::TextUnformatted(txt.c_str());
            ImGui::PopFont();
        });
    }
}
