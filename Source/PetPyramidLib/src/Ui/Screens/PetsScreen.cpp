#include "Pets/Ui/Screens/PetsScreen.h"
#include "Pets/Character/Party.h"
#include "Pets/Pets/PetDetails.h"
#include "Pets/Ui/PetVisual.h"
#include "Pets/Ui/Ui.h"

#include <Animation/Animation.h>
#include <Manage/TickManager.h>
#include <Instrumentation/Logging.h>
#include <DesignPatterns/ServiceLocator.h>
#include <DesignPatterns/Tree.h>
#include <Ui/UiUtil.h>
#include <Ui/UiGeometry.h>
#include <Ui/Panel/TreePanel.h>
#include <Ui/Panel/ZoomFunc.h>

#include <algorithm>

namespace {
	using PetRenderNode = Ui::RenderNode<Pets::PetKind>;
	using PetTree = Tree<PetRenderNode>;
	using RenderFn = std::function<void(const Pets::PetKind&)>;
	using TreePanel = Ui::TreePanel<Pets::PetKind, RenderFn>;

	Ui::TreeConfig TreeConfig{
        .Growth = Ui::GrowthDir::TopDown, 
		.Connect = Ui::ConnectStyle::Corner, 
		.Spacing = {16.f, 16.f}
    };

    Ui::PanelConfig PanelConfig{
		.BackgroundColor = IM_COL32(32, 32, 32, 255), 
		.ZoomFn = Ui::Zoom::Discrete<f32, 0.25f, 1.0f, 4.0f>
    };

	std::vector<ScopedHandle> TickHandles;
	std::unique_ptr<TreePanel> PetTreePanel;
	PetTree PetTreeData;

	Pets::PetRoster* Roster{nullptr};
	Pets::Party* PetParty{nullptr};
	std::optional<Pets::PetKind> SelectedPet;

	constexpr ImVec2 ImageSize{64, 64};
	constexpr auto RosterHeightPercent = 0.15f;
	constexpr auto DetailsHeightPercent = 0.18f;

	PetRenderNode MakeNode(Pets::PetKind kind) {
        return PetRenderNode{.Value = kind, .Visible = true, .BaseSize = {32.f, 32.f}};
    }

	void AddPartyChildren(const Pets::PartyNode& partyNode, PetTree::Node& treeNode) {
		//auto capacity = PetParty->GetPetCapacity(partyNode);
		auto capacity = 2;

		for(size_t index = 0; index < capacity; ++index) {
			if(index < partyNode.Pets.size()) {
				const auto& partyPet = partyNode.Pets.at(index);
				auto& child = PetTreeData.EmplaceChild(treeNode, MakeNode(partyPet.Kind));
				AddPartyChildren(partyPet, child);
			} else {
				PetTreeData.EmplaceChild(treeNode, MakeNode(Pets::PetKind::Unset));
			}
		}
	}

	void RebuildTree(const Pets::Party& party) {
        PetTreeData = PetTree{};
		auto& root = PetTreeData.EmplaceRoot(MakeNode(party.Hero.Kind));
		AddPartyChildren(party.Hero, root);
    }

	void RenderPetNode(const Pets::PetKind& kind) {
		auto bounds = Ui::UiRect::FromPosSize(ImGui::GetWindowPos(), ImGui::GetWindowSize());

		if(kind == Pets::PetKind::Unset) {
			ImGui::GetWindowDrawList()->AddRect(bounds.Min, bounds.Max, IM_COL32_WHITE);
			return;
		}

		Pets::RenderVisualStill(Pets::GetVisual(kind), bounds);
	}

	void OnPetNodeActivate(PetRenderNode& node) {
		SelectedPet = node.Value; 
	}

	void RenderSelectedPet() {
		if(!SelectedPet || !Roster) return;
        auto pet = SelectedPet.value();
		if(pet == Pets::PetKind::Unset) return;

        auto& ownedPet = (*Roster)[pet];
		if(!ownedPet.has_value()) return;
        auto name = Pets::ToString(pet);
        const auto& details = Pets::Details::Get(pet);

		ImGui::PushFont(GetFont(FontSizes::H4));

		if(ImGui::BeginTable("##PetStats", 3)) {
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextUnformatted("Name");
			ImGui::TableNextColumn();
			ImGui::TextUnformatted("Level");
			ImGui::TableNextColumn();
            ImGui::TextUnformatted("Experience");

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
            ImGui::TextUnformatted(name.data(), name.data() + name.size());
			ImGui::TableNextColumn();
            ImGui::Text("%u", ownedPet->Level);
            ImGui::TableNextColumn();
            ImGui::Text("%u", ownedPet->Experience);
            ImGui::TableNextColumn();

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
            ImGui::TextUnformatted("Capacity");
            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Base Attack");
			ImGui::TableNextColumn();
            ImGui::TextUnformatted("Base Pierce");

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
            ImGui::Text("%u", details.PetCapacity);
			ImGui::TableNextColumn();
            ImGui::Text("%u", details.Attack);
            ImGui::TableNextColumn();
            ImGui::Text("%u", details.Piercing);

			ImGui::EndTable();
		}
		ImGui::PopFont();
	}

	void RenderRoster() {
		if(!Roster) return;
		auto available = ImGui::GetContentRegionAvail();
		auto visualSize = available.y;
		auto spacing = ImGui::GetStyle().ItemSpacing.x;

		auto first = true;
		ImGui::PushFont(GetFont(FontSizes::H4));

		for(auto& pet : Roster->Pets) {
			if(!pet ||
				pet->Kind == Pets::PetKind::Hero || 
				pet->Kind == Pets::PetKind::Unset) continue;
            auto name = std::string(Pets::ToString(pet->Kind));

			if(!first) ImGui::SameLine(0.f, spacing);
			first = false;

			ImGui::BeginGroup();
			auto bounds = ::Ui::UiRect::FromPosSize(ImGui::GetCursorScreenPos(), {visualSize, visualSize});
			if(ImGui::InvisibleButton(name.c_str(), {visualSize, visualSize})) {
				SelectedPet = pet->Kind;
			}

			Pets::RenderVisualStill(Pets::GetVisual(pet->Kind), bounds);

			ImGui::EndGroup();
		}
		ImGui::PopFont();
	}
} // namespace

namespace Pets::Ui::Screens::Pets {
	bool Initialize() {
        auto& services = ServiceLocator::Get();
		Roster = &services.GetRequired<PetRoster>();
		PetParty = &services.GetRequired<Party>();

		RebuildTree(*PetParty);
		/*
		        TreePanel(
            const PanelConfig& panelConfig,
            Tree<RenderNode<T>>& tree,
            const TreeConfig& treeConfig,
            RenderFn&& renderFn,
            std::function<void(RenderNode<T>&)> onActivate = nullptr
        )

		*/
        PetTreePanel = std::make_unique<TreePanel>(
			PanelConfig, 
			PetTreeData, 
			TreeConfig, 
			RenderPetNode,
			OnPetNodeActivate);
		return true;
	}

	void ShutDown() {
		TickHandles.clear();
		PetTreePanel.reset();
		Roster = nullptr;
		PetParty = nullptr;
	}

	void Render() {
		if(!PetTreePanel) return;

		auto origin = ImGui::GetCursorPos();
		auto available = ImGui::GetContentRegionAvail();
        auto gap = ImGui::GetStyle().ItemSpacing.y;

		auto rosterHeight = available.y * RosterHeightPercent;
        auto detailsHeight = available.y * DetailsHeightPercent;
		auto treeHeight = available.y - rosterHeight - detailsHeight - gap * 2.f;

		auto detailFlags = SelectedPet
			? ImGuiChildFlags_Borders 
			: ImGuiChildFlags_None;

        auto treeBounds = ::Ui::UiRect::FromPosSize(origin, {available.x, treeHeight});
		PetTreePanel->SetBounds(treeBounds);
		PetTreePanel->Render();

		ImGui::SetCursorPos({origin.x, origin.y + treeHeight + gap});
        ImGui::BeginChild("##PetDetails", {available.x, detailsHeight}, detailFlags);
		RenderSelectedPet();
		ImGui::EndChild();

		ImGui::SetCursorPos({origin.x, origin.y + treeHeight + detailsHeight + gap * 2.f});
		ImGui::BeginChild("##PetRoster", {available.x, rosterHeight}, ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);
		RenderRoster();
		ImGui::EndChild();


	}
} // namespace Pets::Ui::Screens::Pets
