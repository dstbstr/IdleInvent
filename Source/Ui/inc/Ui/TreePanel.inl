#include <DesignPatterns/Tree.h>
#include <imgui.h>
#include <Platform/NumTypes.h>
#include <unordered_map>

namespace Ui::Details {
    template<typename T>
    struct LayoutNode {
        const typename Tree<RenderNode<T>>::Node* TreeNode{nullptr};
        ImVec2 Pos{0.f, 0.f};
        ImVec2 Size{0.f, 0.f};

        constexpr ImVec2 GetConnectPoint(ConnectPoint point) const {
            switch(point) {
                using enum ConnectPoint;
                case North: return {Pos.x + Size.x * 0.5f, Pos.y};
                case East: return {Pos.x + Size.x, Pos.y + Size.y * 0.5f};
                case South: return {Pos.x + Size.x * 0.5f, Pos.y + Size.y};
                case West: return {Pos.x, Pos.y + Size.y * 0.5f};
            }

            return {};
        }
    };

    template<typename T>
    static void BuildLayers(const typename Tree<RenderNode<T>>::Node* node, size_t depth, std::vector<LayoutLayer<T>>& outLayers) {
        if(!node) return;
        if(outLayers.size() <= depth) outLayers.emplace_back();

        outLayers.at(depth).push_back({.TreeNode = node, .Size = node->Value.BaseSize});
        for(const auto& child: node->Children) {
            BuildLayers(child.get(), depth + 1, outLayers);
        }
    }

    template<typename T>
    static void BuildLayoutTopDown(const Tree<RenderNode<T>>& tree, const TreeConfig& config, std::vector<LayoutLayer<T>>& outLayers) {
        outLayers.clear();
        const auto* root = tree.Root();
        if(!root || !root->Value.Visible) return;

        BuildLayers(root, 0, outLayers);
        auto y = config.Padding.y;
        for(auto& layer: outLayers) {
            auto layerHeight = 0.f;
            for(const auto& item: layer) {
                layerHeight = std::max(layerHeight, item.Size.y);
            }
            auto x = config.Padding.x;
            for(auto& item: layer) {
                item.Pos = {x, y};
                x += item.Size.x + config.Spacing.x;
            }

            y += layerHeight + config.Spacing.y;
        }
    }

    template<typename T>
    static void RenderTopDownConnectors(const std::vector<LayoutLayer<T>>& layers, const TreeConfig& config) {
        using TreeNode = typename Tree<RenderNode<T>>::Node;
        std::unordered_map<const TreeNode*, const LayoutNode<T>*> layoutByTreeNode;
        for(const auto& layer: layers) {
            for(const auto& layoutNode: layer) {
                layoutByTreeNode.emplace(layoutNode.TreeNode, &layoutNode);
            }
        }

        for(const auto& layer: layers) {
            for(const auto& node: layer) {
                for(const auto& child: node.TreeNode->Children) {
                    if(!child->Value.Visible) continue;

                    const auto childIt = layoutByTreeNode.find(child.get());
                    if(childIt == layoutByTreeNode.end() || childIt->second == nullptr) continue;

                    const auto parentPoint = node.GetConnectPoint(ConnectPoint::South);
                    const auto childPoint = childIt->second->GetConnectPoint(ConnectPoint::North);

                    auto connection = Connection{
                        .From = parentPoint,
                        .To = childPoint,
                        .Thickness = config.ConnectorThickness,
                        .Color = config.ConnectorColor
                    };
                    switch(config.Connect) {
                        using enum ConnectStyle;
                    case ConnectStyle::Line: DrawLine(connection); break;
                    case ConnectStyle::Corner: DrawCorner(connection); break;
                    case ConnectStyle::None: break;
                    }
                }
            }
        }
    }
}

namespace Ui {
    template<typename T, typename RenderFn>
    void TreePanel<T, RenderFn>::BuildLayout() {
        switch(m_TreeConfig.Growth) {
        case GrowthDir::TopDown: Details::BuildLayoutTopDown(*m_Tree, m_TreeConfig, m_Layers); break;
        default: break;
        }

        for(auto& layer : m_Layers) {
            for(auto& node : layer) {
                node.Pos.x += m_PanelConfig.Position.x;
                node.Pos.y += m_PanelConfig.Position.y;
            }
        }
    }

    template<typename T, typename RenderFn>
    void TreePanel<T, RenderFn>::RenderBackground() const {
        const auto panelMin = Details::ToScreenSpace(m_PanelConfig.Position);
        const auto panelMax = Details::ToScreenSpace({m_PanelConfig.Position.x + m_PanelConfig.Size.x, m_PanelConfig.Position.y + m_PanelConfig.Size.y});

        if(m_PanelConfig.BackgroundColor) {
            ImGui::GetWindowDrawList()->AddRectFilled(panelMin, panelMax, *m_PanelConfig.BackgroundColor);
        }
        if(m_PanelConfig.BackgroundTexture) {
            ImGui::GetWindowDrawList()->AddImage(*m_PanelConfig.BackgroundTexture, panelMin, panelMax);
        }
    }

    template<typename T, typename RenderFn>
    void TreePanel<T, RenderFn>::RenderConnectors() const {
        if(m_TreeConfig.Connect == ConnectStyle::None) return;
        switch(m_TreeConfig.Growth) {
        case GrowthDir::TopDown: Details::RenderTopDownConnectors(m_Layers, m_TreeConfig); break;
        default: break;
        }
    }

    template<typename T, typename RenderFn>
    void TreePanel<T, RenderFn>::RenderNodes() const {
        for(const auto& layer: m_Layers) {
            for(const auto& node: layer) {
                if(!node.TreeNode || !node.TreeNode->Value.Visible) continue;
                ImGui::SetCursorPos(node.Pos);
                m_RenderFn(node.TreeNode->Value.Value, node.Size, node.Pos);
            }
        }
    }
}
