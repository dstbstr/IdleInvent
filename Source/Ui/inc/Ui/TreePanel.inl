#include <DesignPatterns/Tree.h>
#include <imgui.h>
#include <Platform/NumTypes.h>

namespace Ui::TreePanel {
    namespace Details {

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
            }
        };

        template<typename T>
        static void BuildLayers(const typename Tree<RenderNode<T>>::Node* node, size_t depth, std::vector<LayoutLayer<T>>& outLayers) {
            if(!node) return;
            if(outLayers.size() <= depth) outLayers.emplace_back();

            outLayers[depth].push_back({.TreeNode = node, .Size = node->Value.Size});
            for(const auto& child: node->Children) {
                BuildLayers(child.get(), depth + 1, outLayers);
            }
        }

        template<typename T>
        static void BuildLayoutTopDown(const Tree<RenderNode<T>>& tree, const TreePanelConfig& config, std::vector<LayoutLayer<T>>& outLayers) {
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
        static void RenderTopDownConnectors(const std::vector<LayoutLayer<T>>& layers, const TreePanelConfig& config) {
            for(const auto& layer: layers) {
                for(const auto& node: layer) {
                    for(const auto& child: node.TreeNode->Children) {
                        if(!child->Value.Visible) continue;
                        const auto parentPoint = node.GetConnectPoint(ConnectPoint::South);
                        const auto childPoint = child->Value.GetConnectPoint(ConnectPoint::North);
                        auto connection = Connection{
                            .From = parentPoint, 
                            .To = childPoint,
                            .Color = config.ConnectorColor,
                            .Thickness = config.ConnectorThickness
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

        template<typename T>
        static void BuildLayout(const Tree<RenderNode<T>>& tree, const std::vector<LayoutLayer<T>>& layers, const TreePanelConfig& config) {
            switch(config.Growth) {
            case GrowthDir::TopDown: BuildLayoutTopDown(tree, config, layers); break;
            default: break;
            }
        }

        template<typename T>
        static void RenderConnectors(const std::vector<LayoutLayer<T>>& layers, const TreePanelConfig& config) {
            if(config.Connect == ConnectStyle::None) return;
            switch(config.Growth) {
            case GrowthDir::TopDown: RenderTopDownConnectors(layers, config); break;
            default: break;
            }
        }

        template<typename T, typename RenderFn>
        static void RenderNodes(const std::vector<LayoutLayer<T>>& layers, RenderFn& renderFn) {
            for(const auto& layer: layers) {
                for(const auto& node: layer) {
                    if(!node.TreeNode || !node.TreeNode->Value.Visible) continue;
                    ImGui::SetCursorPos(node.Pos);
                    renderFn(node.TreeNode->Value.Value, node.Size);
                }
            }
        }
    }
}