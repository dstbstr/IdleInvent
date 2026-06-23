#include <SampleUI/Ui/Screens/SampleTreePanel.h>

#include <SampleUI/Ui/Screens/Screens.h>

#include <Platform/Graphics.h>
#include <Ui/UiUtil.h>
#include <Ui/TreePanel.h>

#include <imgui.h>
#include <functional>
#include <queue>
#include <string>

namespace {
	constexpr auto HeaderOffsetY = 32.f;
    constexpr auto ControlsOffsetY = 92.f;
    constexpr auto PanelTopOffsetY = 300.f;

	s32 NodeCount = 40;
	s32 FanOut = 3;
	ImVec2 NodeSize{64.f, 64.f};
	f32 NodePadding = 12.f;
	f32 NodeSpacing = 16.f;
	ImVec2 PanelPanOffset{0.f, 0.f};

    Ui::TreeConfig TreeConfig{};
    Ui::PanelConfig PanelConfig{};

	struct SampleNode {
        std::string Name;
	};

	using SampleRenderNode = Ui::RenderNode<SampleNode>;
	using SampleTree = Tree<SampleRenderNode>;
	using SampleRenderFn = std::function<void(const SampleNode&, ImVec2, ImVec2)>;

	SampleTree s_Tree{};

	SampleRenderNode MakeNode(std::string name) {
		return SampleRenderNode{
			.Value = SampleNode{std::move(name)},
			.Visible = true,
			.BaseSize = NodeSize
		};
	}

	void RebuildTree() {
		s_Tree = SampleTree{};
        if(NodeCount <= 0) return;
        auto& root = s_Tree.EmplaceRoot(MakeNode("0"));
        size_t created = 1;
        std::queue<SampleTree::Node*> q;
        q.push(&root);
		while(created < NodeCount && !q.empty()) {
            auto* parent = q.front();
            q.pop();

			const auto& parentName = parent->Value.Value.Name;
			for(size_t i = 0; i < FanOut && created < NodeCount; ++i) {
                auto childName = parentName + "." + std::to_string(i);
                auto& child = s_Tree.EmplaceChild(*parent, MakeNode(childName));
                q.push(&child);
                ++created;
			}
		}
	}

	const char* ToLabel(Ui::GrowthDir dir) {
		switch(dir) {
			using enum Ui::GrowthDir;
			case TopDown: return "Top Down";
			case BottomUp: return "Bottom Up";
			case LeftRight: return "Left to Right";
			case CenterOut: return "Center Out";
		}
        return "Unknown";
	}

	const char* ToLabel(Ui::ConnectStyle style) {
        switch(style) {
            using enum Ui::ConnectStyle;
			case None: return "None";
			case Line: return "Line";
			case Corner: return "Corner";
        }

        return "Unknown";
	}
}

namespace SampleUI::Ui::Screens::SampleTreePanel {
	bool Initialize() {
        TreeConfig.Growth = ::Ui::GrowthDir::TopDown;
        TreeConfig.Connect = ::Ui::ConnectStyle::Line;
        TreeConfig.Padding = {NodePadding, NodePadding};
        TreeConfig.Spacing = {NodeSpacing, NodeSpacing};
        PanelConfig.Position = {0.f, PanelTopOffsetY};
        PanelConfig.Size = {Graphics::ScreenWidth, Graphics::ScreenHeight - PanelTopOffsetY};
        PanelConfig.BackgroundColor = IM_COL32(32, 32, 32, 255);

		RebuildTree();
		return true;
	}

	void ShutDown() {}

	void Render() {
		ImGui::SetNextWindowPos({0.f, 0.f});
		ImGui::SetNextWindowSize({Graphics::ScreenWidth, Graphics::ScreenHeight});

		ImGui::Begin("Sample TreePanel", nullptr, BaseUiFlags);

		if(ImGui::Button("Back")) {
			Screens::SetActiveScreen(Screen::Landing);
		}

		ImGui::SetCursorPosY(HeaderOffsetY);
        ImGui::PushFont(GetFont(FontSizes::H1));
		TextCenteredX("TreePanel Sample");
		ImGui::PopFont();

        ImGui::PushFont(GetFont(FontSizes::H4));
		ImGui::SetCursorPosY(ControlsOffsetY);
        bool rebuild = false;
        rebuild |= ImGui::SliderInt("Total Nodes", &NodeCount, 0, 1'000);
        rebuild |= ImGui::SliderInt("Fan Out", &FanOut, 1, 10);
        rebuild |= ImGui::SliderFloat2("Node Size", &NodeSize.x, 16.f, 256.f);
        rebuild |= ImGui::SliderFloat("Node Padding", &NodePadding, 0.f, 64.f);
        rebuild |= ImGui::SliderFloat("Node Spacing", &NodeSpacing, 0.f, 64.f);

		int growthSelect = static_cast<int>(TreeConfig.Growth);
		const char* growthLabels = "Top Down\0Bottom Up\0Left to Right\0Center Out";

		if(ImGui::Combo("Growth Direction", &growthSelect, growthLabels, 4)) {
			TreeConfig.Growth = static_cast<::Ui::GrowthDir>(growthSelect);
			rebuild = true;
		}
		ImGui::Text("Current: %s", ToLabel(TreeConfig.Growth));
		ImGui::TextUnformatted("Pan: drag with right mouse over panel area");
		if(ImGui::Button("Reset Pan")) {
			PanelPanOffset = {0.f, 0.f};
		}
		ImGui::PopFont();
		if(rebuild) {
			NodeSize.y = NodeSize.x;
			TreeConfig.Spacing = {NodeSpacing, NodeSpacing};
			RebuildTree();
		}

		PanelConfig.Position = {0.f, PanelTopOffsetY};
		PanelConfig.Size = {Graphics::ScreenWidth, Graphics::ScreenHeight - PanelTopOffsetY};

		const auto panelMin = PanelConfig.Position;
		const auto panelMax = ImVec2{PanelConfig.Position.x + PanelConfig.Size.x, PanelConfig.Position.y + PanelConfig.Size.y};
		if(ImGui::IsMouseHoveringRect(panelMin, panelMax, false) && ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0.f)) {
			const auto mouseDelta = ImGui::GetIO().MouseDelta;
			PanelPanOffset.x += mouseDelta.x;
			PanelPanOffset.y += mouseDelta.y;
		}

		TreeConfig.Padding = {NodePadding + PanelPanOffset.x, NodePadding + PanelPanOffset.y};

		::Ui::TreePanel<SampleNode, SampleRenderFn> panel(
			s_Tree,
			PanelConfig,
			TreeConfig,
			[](const SampleNode& node, ImVec2 size, ImVec2 pos) {
				const auto bottomRight = ImVec2{pos.x + size.x, pos.y + size.y};
				ImGui::GetWindowDrawList()->AddRect(pos, bottomRight, IM_COL32(255, 255, 255, 255), 4.f);

				ImGui::SetCursorPos(pos);
				ImGui::TextUnformatted(node.Name.c_str());
			}
		);
		panel.Render();
		ImGui::End();
	}
} // namespace SampleUI::Ui::Screens::SampleTreePanel
