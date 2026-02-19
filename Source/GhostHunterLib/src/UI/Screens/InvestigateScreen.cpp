#include "GhostHunter/Ui/Screens/InvestigateScreen.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/Locations/Locations.h"
#include "GhostHunter/Investigation/Investigation.h"
#include "GhostHunter/Formatting.h"
#include "GhostHunter/GameState/Life.h"
#include "GhostHunter/Tools/UseTool.h"

#include "Utilities/EnumUtils.h"
#include "Mechanics/Purchasable.h"

#include <unordered_set>

namespace {
    GhostHunter::Life* life{nullptr};

    constexpr auto* ToolPayload = "GHOSTHUNTER_TOOL";
    constexpr auto* MemberPayload = "GHOSTHUNTER_MEMBER";
    constexpr auto* GearRoomPayload = "GHOSTHUNTER_GEAR_ROOM";

    constexpr auto SlotFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    GhostHunter::LocationName selectedLocation{GhostHunter::LocationName::Unset};

    void RenderSelectLocation() {
        using namespace GhostHunter;
        for(auto location : life->GetUnlockedLocations()) {
            if(ImGui::Button(ToString(location).c_str())) {
                selectedLocation = location;
            }
        }
    }

    void RenderConfirmLocation() {
        using namespace GhostHunter;
        auto& resources = life->GetInventory().Resources;
        auto afford = Purchasables::CanPurchase(selectedLocation, resources);

        ImGui::Text("%s", ToString(selectedLocation).c_str());
        ImGui::TextWrapped("%s", Describe(selectedLocation).c_str());

        ImGui::BeginTable("NavButtons", 2);
        ImGui::TableNextColumn();
        if(ImGui::Button("Back")) {
            selectedLocation = LocationName::Unset;
        }
        
        ImGui::TableNextColumn();
        ImGui::BeginDisabled(!afford);
        if(ImGui::Button("Rent")) {
            Purchasables::TryPurchase(selectedLocation, resources, BuyOnce::No);
            selectedLocation = LocationName::Unset;
        }
        ImGui::EndDisabled();
        ImGui::EndTable();
    }

    void RenderRentLocation() {
        if(selectedLocation == GhostHunter::LocationName::Unset) {
            RenderSelectLocation();
        } else {
            RenderConfirmLocation();
        }
    }

    using TeamMap = std::unordered_map<std::string, std::vector<GhostHunter::TeamMember*>>;
    void RenderRoomSlots(std::vector<GhostHunter::Room>& rooms, const TeamMap& teamMap) {
        ImGui::BeginTable("RoomSlots", 2, ImGuiTableFlags_SizingStretchSame);

        for(size_t i = 0; i < rooms.size(); i++) {
            std::vector<GhostHunter::TeamMember*> members;
            if(teamMap.contains(rooms[i].Name)) {
                members = teamMap.at(rooms[i].Name);
            }
            ImGui::TableNextColumn();
            ImGui::PushID(static_cast<int>(i));
            ImGui::BeginChild("slot", ImVec2(-1, 100), true, SlotFlags);
            ImGui::TextUnformatted(rooms[i].Name.c_str());

            if(members.empty()) {
                ImGui::Dummy(ImVec2(0.f, 8.f));
            } else {
                for(const auto* member : members) {
                    auto label = std::format("{} {}", ToString(member->Id), ToString(member->GetCurrentTool()->Id));
                    ImGui::Button(label.c_str());
                    if(ImGui::BeginDragDropSource()) {
                        auto* dataPtr = &member;
                        ImGui::SetDragDropPayload(GearRoomPayload, dataPtr, sizeof(member));
                        ImGui::TextUnformatted(label.c_str());
                        ImGui::EndDragDropSource();
                    }
                }
            }

            ImGui::EndChild();

            auto slotStart = ImGui::GetItemRectMin();
            auto slotEnd = ImGui::GetItemRectMax();
            auto* dl = ImGui::GetWindowDrawList();
            dl->AddRect(slotStart, slotEnd, IM_COL32(180, 180, 180, 255), 6.f, 0, 2.f);

            if(members.empty()) {
                const auto oldCursor = ImGui::GetCursorPos();
                ImGui::SetCursorScreenPos(slotStart);
                ImGui::InvisibleButton("cell_drop", ImVec2(slotEnd.x - slotStart.x, slotEnd.y - slotStart.y));
                if(ImGui::BeginDragDropTarget()) {
                    if(const auto* payload = ImGui::AcceptDragDropPayload(MemberPayload)) {
                        if(payload->DataSize == sizeof(GhostHunter::TeamMember*)) {
                            auto* member = *static_cast<GhostHunter::TeamMember* const*>(payload->Data);
                            member->SetCurrentRoom(&rooms[i]);
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::SetCursorScreenPos(oldCursor);
            }

            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    void RenderGearRoom(const TeamMap& teamMap, const std::vector<GhostHunter::ToolName>& ownedTools) {
        using namespace GhostHunter;
        std::vector<TeamMember*> members;
        if(teamMap.contains("GearRoom")) {
            members = teamMap.at("GearRoom");
        }
        auto availableTools = std::unordered_set<ToolName>{ownedTools.begin(), ownedTools.end()};
        for(const auto& [_, member] : teamMap) {
            for(const auto* m : member) {
                if(m->GetCurrentTool()) {
                    availableTools.erase(m->GetCurrentTool()->Id);
                }
            }
        }

        auto linesNeeded = 1 + members.size() + availableTools.size();
        auto height = 48.f + (ImGui::GetFrameHeightWithSpacing() * linesNeeded);

        ImGui::BeginChild("slot", ImVec2(-1.f, height), true, SlotFlags);
        ImGui::TextUnformatted("Gear Room");
        ImGui::Dummy(ImVec2(0.f, 8.f));

        if(ImGui::BeginTable("GearTable", 2, ImGuiTableFlags_SizingStretchSame)) {
            ImGui::TableNextColumn();
            for(size_t i = 0; i < members.size(); i++) {
                auto* member = members[i];
                ImGui::PushID(static_cast<int>(i));
                auto label = std::format("{} {}", member->Id, member->GetCurrentTool() ? ToString(member->GetCurrentTool()->Id) : "");
                ImGui::Button(label.c_str());
                if(member->GetCurrentTool() && ImGui::BeginDragDropSource()) {
                    auto* dataPtr = &member;
                    ImGui::SetDragDropPayload(MemberPayload, dataPtr, sizeof(member));
                    ImGui::Text("%s %s", ToString(member->Id).c_str(), ToString(member->GetCurrentTool()->Id).c_str());
                    ImGui::EndDragDropSource();
                }
                if(!member->GetCurrentTool() && ImGui::BeginDragDropTarget()) {
                    if(const auto* payload = ImGui::AcceptDragDropPayload(ToolPayload)) {
                        ToolName id = *static_cast<ToolName*>(payload->Data);
                        member->SetCurrentTool(&life->GetInventory().OwnedTools.at(id));
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::PopID();
                ImGui::Spacing();
            }

            ImGui::TableNextColumn();
            size_t toolIndex = 0;
            for(const auto& tool : ownedTools) {
                if(!availableTools.contains(tool)) continue;
                ImGui::PushID(static_cast<int>(toolIndex++));
                ImGui::Button(ToString(tool).c_str());
                if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                    ImGui::SetDragDropPayload(ToolPayload, &tool, sizeof(tool));
                    ImGui::TextUnformatted(ToString(tool).c_str());
                    ImGui::EndDragDropSource();
                }
                ImGui::PopID();
                ImGui::Spacing();
            }

            ImGui::EndTable();
        }
        /*
        for(auto* member: members) {
            auto label = std::format("{} {}", member->Id, member->GetCurrentTool() ? ToString(member->GetCurrentTool()->Id) : "");
            ImGui::Button(label.c_str());
            if(member->GetCurrentTool() && ImGui::BeginDragDropSource()) {
                auto* dataPtr = &member;
                ImGui::SetDragDropPayload(MemberPayload, dataPtr, sizeof(member));
                ImGui::Text("%s %s", ToString(member->Id).c_str(), ToString(member->GetCurrentTool()->Id).c_str());
                ImGui::EndDragDropSource();
            }
            if(!member->GetCurrentTool() && ImGui::BeginDragDropTarget()) {
                if(const auto* payload = ImGui::AcceptDragDropPayload(ToolPayload)) {
                    ToolName id = *static_cast<ToolName*>(payload->Data);
                    member->SetCurrentTool(&life->GetInventory().OwnedTools.at(id));
                }
                ImGui::EndDragDropTarget();
            }
        }

        for(size_t i = 0; i < ownedTools.size(); i++) {
            auto tool = ownedTools[i];
            if(!availableTools.contains(tool)) continue;
            ImGui::Button(ToString(tool).c_str());
            if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                ImGui::SetDragDropPayload(ToolPayload, &tool, sizeof(tool));
                ImGui::TextUnformatted(ToString(tool).c_str());
                ImGui::EndDragDropSource();
            }
        }
        */
        ImGui::EndChild();

        auto rectStart = ImGui::GetItemRectMin();
        auto rectEnd = ImGui::GetItemRectMax();
        auto* dl = ImGui::GetWindowDrawList();
        dl->AddRect(rectStart, rectEnd, IM_COL32(180, 180, 180, 255), 6.f, 0, 2.f);

        auto oldCursor = ImGui::GetCursorScreenPos();
        ImGui::SetCursorScreenPos(rectStart);
        ImGui::InvisibleButton("gearRoomDrop", {rectEnd.x - rectStart.x, rectEnd.y - rectStart.y});

        if(ImGui::BeginDragDropTarget()) {
            if(const auto* payload = ImGui::AcceptDragDropPayload(GearRoomPayload)) {
                if(payload->DataSize == sizeof(GhostHunter::TeamMember*)) {
                    auto* member = *static_cast<GhostHunter::TeamMember* const*>(payload->Data);
                    member->SetCurrentRoom(nullptr);
                    member->SetCurrentTool(nullptr);
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::SetCursorScreenPos(oldCursor);
    }

    void RenderUseLocation() {
        using namespace GhostHunter;
        const auto* investigation = life->GetCurrentInvestigation();
        auto& allRooms = life->GetLocation(investigation->GetLocation()).GetRooms();

        ImGui::Text("%s", investigation->Describe().c_str());
        ImGui::ProgressBar(
            1.0f - investigation->GetProgress(),
            ImVec2(-1, 0), 
            std::format("{}", investigation->Ttl).c_str()
        );
        
        auto teamMap = TeamMap{};
        for(auto& [name, member] : life->GetTeam().Members) {
            auto& list = member.GetCurrentRoom() ? teamMap[member.GetCurrentRoom()->Name] : teamMap["GearRoom"];
            list.push_back(&member);
        }

        RenderRoomSlots(allRooms, teamMap);
        std::vector<ToolName> ownedTools{};
        for(auto& [id, tool] : life->GetInventory().OwnedTools) {
            ownedTools.push_back(id);
        }
        RenderGearRoom(teamMap, ownedTools);
    }
}

namespace GhostHunter::Ui::Screens::Investigate {
    bool Initialize() { 
        life = &Life::Get();
        return true; 
    }

    void ShutDown() {}

    void Render() {
        if(life->GetCurrentInvestigation()) {
            RenderUseLocation();
        } else {
            RenderRentLocation();
        }
    }
} // namespace GhostHunter::Ui::Screens::Investigate