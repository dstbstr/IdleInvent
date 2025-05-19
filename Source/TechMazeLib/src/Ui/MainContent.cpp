#include "Platform/Graphics.h"
#include "TechMaze/Ui/MainContent.h"
#include "TechMaze/Ui/Ui.h"
#include "TechMaze/Technology.h"
#include "TechMaze/Formatting.h"

#include <Ui/ChildBuilder.h>
#include <Ui/UiUtil.h>
#include <Platform/NumTypes.h>

#include <imgui.h>
#include <array>

namespace TechMaze::Ui::Screens::MainContent {
    void Initialize() {
        // Initialization code here
    }
    void Render() {
        auto MakeColor = [](float hue) -> ImVec4 { return ImColor::HSV(hue, 0.7f, 0.7f); };
        static const ImVec4 Colors[] = {
            MakeColor(0.15f), MakeColor(0.25f), MakeColor(0.40f), MakeColor(0.55f), MakeColor(0.70f), MakeColor(0.85f)
        };
        static const ImVec4 SelectedColor = ImColor::HSV(0.0f, 0.0f, 0.3f, 0.9f);
        static const ImVec4 UnselectedColor = ImColor::HSV(0.0f, 0.0f, 0.2f, 0.9f);

        static std::array<size_t, 6> s_Levels{0, 0, 0, 0, 0, 0};
        static std::array<u8, 6> s_Eras{0, 0, 0, 0, 0, 0};
        static std::vector<size_t> s_ActiveSkills{};
        static size_t s_MaxSkills{1};
        static size_t s_MaxLevel{500};

        ImGui::Begin("Main Content", nullptr, BaseFlags);
        for(auto i : s_ActiveSkills) {
            s_Levels[i] = std::min(s_MaxLevel, s_Levels[i] + 1);
            if(s_Levels[i] > 100 && s_Eras[i] < 5) {
                s_Eras[i]++;
                s_Levels[i] = 0;
            }
        }

        auto boxSize = Graphics::ScreenWidth / 6;
        auto halfBoxSize = boxSize / 2;
        auto buffer = boxSize / 4;
        auto halfBuffer = buffer / 2;
        auto MakeBox = [boxSize, halfBoxSize, buffer, halfBuffer](const char* label, size_t idx) {
            ImGui::SetCursorPosX(buffer);
            ImGui::SetCursorPosY(idx * (boxSize + buffer) + halfBuffer);

            auto OnClick = [&]() {
                auto it = std::remove(s_ActiveSkills.begin(), s_ActiveSkills.end(), idx);
                if(it == s_ActiveSkills.end()) {
                    s_ActiveSkills.push_back(idx);
                } else {
                    s_ActiveSkills.erase(it, s_ActiveSkills.end());
                }
            };
            {
                auto child = ChildBuilder(std::format("{}_level", label).c_str(), boxSize)
                                 .WithBgColor(Colors[idx])
                                 .WithFont(GetFont(FontSizes::H1))
                                 .Build();
                if(ImGui::InvisibleButton(std::format("{}_Button", label).c_str(), {boxSize, boxSize})) {
                    OnClick();
                }
                
                TextCentered(std::format("{}", s_Levels[idx]).c_str());
            }

            ImGui::SameLine();
            ImGui::SetCursorPosX(buffer + boxSize);

            {
                auto dims = ImVec2(ImGui::GetWindowWidth() - boxSize - (buffer * 2), boxSize);
                auto child = ChildBuilder(label, dims)
                                 .WithBgColor(std::find(s_ActiveSkills.begin(), s_ActiveSkills.end(), idx) == s_ActiveSkills.end() ? UnselectedColor : SelectedColor)
                                 .WithFont(GetFont(FontSizes::H2))
                                 .Build();
                if(ImGui::InvisibleButton(std::format("{}_Button", label).c_str(), dims)) {
                    OnClick();
                }

                        /*
enum struct Communication : u8 { Verbal, Written, Printed, Digital, Global, Telepathic };
enum struct Military : u8 { Militia, Cavalry, Platoon, Regiment, Battalion, ArmedForces };
enum struct Settlement : u8 { Hamlet, Town, City, Metro, Kingdom, Empire };
enum struct Technology : u8 { Stone, Bronze, Iron, Steel, Electronic, Nano };
enum struct Transportation : u8 { Foot, Horse, Carriage, Ship, Plane, Rocket };
enum struct Power : u8 { Human, Animal, Steam, Combustion, Fission, Fusion};
*/

                auto era = s_Eras[idx];
                std::string eraText;
                switch(idx) {
                case 0: eraText = std::format("{}", static_cast<Communication>(era)); break;
                case 1: eraText = std::format("{}", static_cast<Military>(era)); break;
                case 2: eraText = std::format("{}", static_cast<Settlement>(era)); break;
                case 3: eraText = std::format("{}", static_cast<Technology>(era)); break;
                case 4: eraText = std::format("{}", static_cast<Transportation>(era)); break;
                case 5: eraText = std::format("{}", static_cast<Power>(era)); break;
                default: eraText = "Unknown"; break;
                }
                
                TextCentered(std::format("{}: {}", label, eraText).c_str());
            }
        };

        size_t boxIdx = 0;
        MakeBox("Communication", boxIdx++);
        MakeBox("Military", boxIdx++);
        MakeBox("Settlement", boxIdx++);
        MakeBox("Technology", boxIdx++);
        MakeBox("Transportation", boxIdx++);
        MakeBox("Power", boxIdx++);

        static size_t s_Increment = 1;
        ImGui::InputScalar("Max Skills", ImGuiDataType_U64, &s_MaxSkills, &s_Increment);

        while(s_ActiveSkills.size() > s_MaxSkills) {
            std::rotate(s_ActiveSkills.begin(), s_ActiveSkills.begin() + 1, s_ActiveSkills.end());
            s_ActiveSkills.pop_back();
        }

        ImGui::End();
    }
} // namespace TechMaze::Ui::Screens::MainContent