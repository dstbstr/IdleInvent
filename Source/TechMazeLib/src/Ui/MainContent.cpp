#include "Platform/Graphics.h"
#include "TechMaze/Ui/MainContent.h"
#include "TechMaze/Ui/Ui.h"

#include <imgui.h>
#include <Ui/ChildBuilder.h>
#include <Ui/UiUtil.h>

namespace TechMaze::Ui::Screens::MainContent {
    void Initialize() {
        // Initialization code here
    }
    void Render() {
        auto MakeColor = [](float hue) -> ImVec4 { return ImColor::HSV(hue, 0.7f, 1.0f); };
        static const ImVec4 Colors[] = {
            MakeColor(0.25f), MakeColor(0.40f), MakeColor(0.55f), MakeColor(0.70f), MakeColor(0.85f)
        };

        /*
            enum struct Communication : u8 { Verbal, Written, Printed, Digital, Global, Telepathic };
    enum struct Military : u8 { Militia, Cavalry, Platoon, Regiment, Battalion, ArmedForces };
    enum struct Settlement : u8 { Hamlet, Town, City, Metro, Kingdom, Empire };
    enum struct Technology : u8 { Stone, Bronze, Iron, Steel, Electronic, Nano };
    enum struct Transportation : u8 { Foot, Horse, Carriage, Ship, Plane, Rocket };
*/
        ImGui::Begin("Main Content", nullptr, BaseFlags);
        size_t boxIdx = 0;
        auto boxSize = Graphics::ScreenWidth / 3;
        auto halfBoxSize = boxSize / 2;
        auto buffer = Graphics::ScreenWidth / 10;
        auto MakeBox = [boxSize, halfBoxSize, buffer](const char* label, size_t idx) {
            ImGui::SetCursorPosX(buffer);
            ImGui::SetCursorPosY(idx * (boxSize + buffer));

            {
                auto child = ChildBuilder(std::format("{}_level", label).c_str(), boxSize)
                                 .WithBgColor(Colors[idx])
                                 .WithFont(GetFont(FontSizes::H1))
                                 .CenterY()
                                 .Build();
                //ImGui::SetCursorPosY(ImGui::GetCursorPosY() + halfBoxSize);
                TextCentered("0");
            }
            ImGui::SameLine();
            {
                auto child = ChildBuilder(label, ImVec2(Graphics::ScreenWidth - boxSize, boxSize))
                                 .WithBgColor(Colors[idx])
                                 .WithFont(GetFont(FontSizes::H2))
                                 .CenterY()
                                 .Build();

                TextCentered(label);
            }
        };
        MakeBox("Communication", boxIdx++);
        MakeBox("Military", boxIdx++);
        MakeBox("Settlement", boxIdx++);
        MakeBox("Technology", boxIdx++);
        MakeBox("Transportation", boxIdx++);

        ImGui::End();
    }
} // namespace TechMaze::Ui::Screens::MainContent