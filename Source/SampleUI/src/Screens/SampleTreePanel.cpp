#include <SampleUI/Screens/SampleTreePanel.h>
#include <SampleUI/Screens/Screens.h>
#include <SampleUI/Screens/SampleScreen.h>

#include <Platform/Graphics.h>
#include <Ui/UiUtil.h>
#include <Ui/Panel/TreePanel.h>
#include <Ui/Panel/ZoomFunc.h>

#include <imgui.h>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <utility>

namespace {
    constexpr auto ControlsOffsetY = 92.f;
    constexpr auto PanelTopMargin = 8.f;

	struct SampleNode {std::string Name;};
	using SampleRenderNode = Ui::RenderNode<SampleNode>;
	using SampleTree = Tree<SampleRenderNode>;
	using SampleRenderFn = std::function<void(const SampleNode&)>;
    using SamplePanel = Ui::TreePanel<SampleNode, SampleRenderFn>;

	enum struct ZoomMode : u8 { Fluid = 0, Discrete = 1 };
    auto FluidZoom = Ui::Zoom::Exponential<f32, 1.1f>;
    auto DiscreteZoom = Ui::Zoom::Discrete<f32, 0.25f, 0.5f, 1.0f, 2.f, 4.0f>;

	s32 NodeCount = 40;
	s32 FanOut = 3;
	ImVec2 NodeSize{64.f, 64.f};
	ImVec2 NodeSpacing{16.f, 16.f};
	ZoomMode CurrentZoomMode = ZoomMode::Fluid;

	Ui::TreeConfig TreeConfig{
        .Growth = Ui::GrowthDir::TopDown,
        .Connect = Ui::ConnectStyle::Line,
        .Spacing = NodeSpacing
    };
	Ui::PanelConfig PanelConfig{
        .BackgroundColor = IM_COL32(32, 32, 32, 255),
        .ZoomFn = FluidZoom
    };

	SampleTree TreeData{};
    std::unique_ptr<SamplePanel> Panel;

	SampleRenderNode MakeNode(std::string name) {
		return SampleRenderNode{
			.Value = SampleNode{std::move(name)},
			.Visible = true,
			.BaseSize = NodeSize
		};
	}

	void RebuildTree() {
		TreeData = SampleTree{};
		if(NodeCount <= 0) return;
		auto& root = TreeData.EmplaceRoot(MakeNode("0"));
		auto created = 1;
		std::queue<SampleTree::Node*> q;
		q.push(&root);
		while(created < NodeCount && !q.empty()) {
			auto* parent = q.front();
			q.pop();

			const auto& parentName = parent->Value.Value.Name;
			for(auto i = 0; i < FanOut && created < NodeCount; ++i) {
				auto childName = parentName + "." + std::to_string(i);
				auto& child = TreeData.EmplaceChild(*parent, MakeNode(childName));
				q.push(&child);
				++created;
			}
		}
	}

	void ShowAllNodes() {
		TreeData.ForEach([](SampleRenderNode& node) {
			node.Visible = true;
		});
	}

	void ApplyZoomMode() {
		switch(CurrentZoomMode) {
			using enum ZoomMode;
		    case Fluid: PanelConfig.ZoomFn = FluidZoom; break;
		    case Discrete: PanelConfig.ZoomFn = DiscreteZoom; break;
		}
	}

    void RenderNode(const SampleNode& node) {
        auto pos = ImGui::GetWindowPos();
        auto size = ImGui::GetWindowSize();
        ImGui::PushFont(GetFont(FontSizes::H4));

        ImGui::GetWindowDrawList()->AddRect(pos, pos + size, IM_COL32_WHITE, 4.f);
        ImGui::TextUnformatted(node.Name.c_str());

        ImGui::PopFont();
    }

    void OnNodeClick(Ui::RenderNode<SampleNode>& node) { node.Visible = false; }

	void RebuildPanel() {
		Panel = std::make_unique<Ui::TreePanel<SampleNode, SampleRenderFn>>(
			PanelConfig, TreeData, TreeConfig, RenderNode, OnNodeClick
		);
	}

	void RenderControls() {
        ImGui::PushFont(GetFont(FontSizes::H4));
        ImGui::SetCursorPosY(ControlsOffsetY);
        bool rebuild = false;
        rebuild |= ImGui::SliderInt("Total Nodes", &NodeCount, 0, 1'000);
        rebuild |= ImGui::SliderInt("Fan Out", &FanOut, 1, 10);
        rebuild |= ImGui::SliderFloat2("Node Size", &NodeSize.x, 16.f, 256.f);

        if(ImGui::SliderFloat2("Node Spacing", &NodeSpacing.x, 0.f, 64.f)) {
            TreeConfig.Spacing = NodeSpacing;
        }

        if(rebuild) {
            RebuildTree();
        }

        int growthSelect = static_cast<int>(TreeConfig.Growth);
        const char* growthLabels = "Top Down\0Bottom Up\0Left to Right";
        if(ImGui::Combo("Growth Direction", &growthSelect, growthLabels, 3)) {
            TreeConfig.Growth = static_cast<::Ui::GrowthDir>(growthSelect);
        }

        int connectSelect = static_cast<int>(TreeConfig.Connect);
        const char* connectLabels = "None\0Line\0Corner";
        if(ImGui::Combo("Connect Style", &connectSelect, connectLabels, 3)) {
            TreeConfig.Connect = static_cast<::Ui::ConnectStyle>(connectSelect);
        }

        ImVec4 connectorColor = ImGui::ColorConvertU32ToFloat4(TreeConfig.ConnectorColor);
        if(ImGui::ColorEdit4("Connector Color", &connectorColor.x)) {
            TreeConfig.ConnectorColor = ImGui::ColorConvertFloat4ToU32(connectorColor);
        }
        ImGui::SliderFloat("Connector Thickness", &TreeConfig.ConnectorThickness, 1.f, 10.f);

        int anchorSelect = static_cast<int>(TreeConfig.Anchor);
        const char* anchorLabels = "Top Left\0Top Center\0Top Right\0Left Center\0Center\0Right Center\0Bottom "
                                   "Left\0Bottom Center\0Bottom Right";
        if(ImGui::Combo("Anchor", &anchorSelect, anchorLabels, 9)) {
            TreeConfig.Anchor = static_cast<::Ui::Anchor>(anchorSelect);
        }

        int zoomModeSelect = static_cast<int>(CurrentZoomMode);
        const char* zoomModeLabels = "Fluid\0Discrete";
        if(ImGui::Combo("Zoom Mode", &zoomModeSelect, zoomModeLabels, 2)) {
            CurrentZoomMode = static_cast<ZoomMode>(zoomModeSelect);
            ApplyZoomMode();
            RebuildPanel();
        }

        ImGui::TextUnformatted("Pan: drag with right mouse over panel area");
        if(ImGui::Button("Reset Pan")) {
            if(Panel) Panel->ResetPan();
        }
        ImGui::SameLine();
        if(ImGui::Button("Zoom In")) {
            if(Panel) Panel->ZoomIn();
        }
        ImGui::SameLine();
        if(ImGui::Button("Zoom Out")) {
            if(Panel) Panel->ZoomOut();
        }
        ImGui::SameLine();
        if(ImGui::Button("Reset Zoom")) {
            if(Panel) Panel->ResetZoom();
        }
        ImGui::SameLine();
        if(ImGui::Button("Show All")) {
            ShowAllNodes();
        }
        ImGui::PopFont();
	}

	void RenderContent() {
        if(!Panel) return;

        auto panelTop = ImGui::GetCursorPosY() + PanelTopMargin;
        auto bounds = Ui::UiRect{{0.f, panelTop}, {Graphics::ScreenWidth, Graphics::ScreenHeight}};

        if(Panel) {
            Panel->SetBounds(bounds);
            Panel->Render();
        }
	}
}

namespace SampleUI::Screens::SampleTreePanel {
	bool Initialize() {
		RebuildTree();
        RebuildPanel();

		return true;
	}

	void ShutDown() {
        Panel.reset();
    }

	void Render() {
        RenderSampleScreen("Tree Panel", [] {
            RenderControls();
            RenderContent();
        });
	}
} // namespace SampleUI::Screens::SampleTreePanel
