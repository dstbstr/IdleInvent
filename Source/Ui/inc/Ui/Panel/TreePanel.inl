#include <DesignPatterns/Tree.h>
#include <imgui.h>
#include <Platform/NumTypes.h>
#include <optional>
#include <unordered_map>
#include <utility>

namespace Ui::Details {
    template<typename T>
    struct LayoutNode {
        typename Tree<RenderNode<T>>::Node* TreeNode{nullptr};
        UiRect Bounds;
        UiRect SubtreeBounds;

        constexpr ImVec2 GetConnectPoint(ConnectPoint point) const {
            auto [min, max] = Bounds;
            switch(point) {
                using enum ConnectPoint;
                case North: return {Bounds.CenterX(), Bounds.Min.y};
                case East: return {Bounds.Max.x, Bounds.CenterY()};
                case South: return {Bounds.CenterX(), Bounds.Max.y};
                case West: return {Bounds.Min.x, Bounds.CenterY()};
            }

            return {};
        }
    };

    static void UpdateBounds(GrowthDir growth, const UiRect& child, UiRect& parent) {
        switch(growth) {
            using enum GrowthDir;
        case TopDown:
        case BottomUp:
            parent.Max.x += child.GetWidth();
            parent.Max.y = std::max(parent.Max.y, child.GetHeight());
            break;
        case LeftRight:
            parent.Max.y += child.GetHeight();
            parent.Max.x = std::max(parent.Max.x, child.GetWidth());
            break;
        }
    }

    static void UpdateSubtreeBounds(
        GrowthDir growth,
        const UiRect& children,
        size_t visibleChildren,
        const ImVec2& spacing,
        UiRect& subtree
    ) {
        if(visibleChildren == 0) return;

        const auto siblingGaps = static_cast<f32>(visibleChildren - 1);
        switch(growth) {
            using enum GrowthDir;
        case TopDown:
        case BottomUp:
            subtree.Max.x = std::max(subtree.Max.x, children.GetWidth() + siblingGaps * spacing.x);
            subtree.Max.y += spacing.y + children.GetHeight();
            break;
        case LeftRight:
            subtree.Max.y = std::max(subtree.Max.y, children.GetHeight() + siblingGaps * spacing.y);
            subtree.Max.x += spacing.x + children.GetWidth();
            break;
        }
    }

    template<typename T>
    static UiRect BuildLayers(
        typename Tree<RenderNode<T>>::Node* node,
        size_t depth,
        GrowthDir growth,
        const TreeConfig& config,
        std::vector<LayoutLayer<T>>& outLayers
    ) {
        if(!node || !node->Value.Visible) return {};
        if(outLayers.size() <= depth) outLayers.emplace_back();

        const auto& baseSize = node->Value.BaseSize;

        UiRect childrenBounds{};
        size_t visibleChildren = 0;
        for(const auto& child: node->Children) {
            if(!child || !child->Value.Visible) continue;
            const auto childSubtree = BuildLayers(child.get(), depth + 1, growth, config, outLayers);
            UpdateBounds(growth, childSubtree, childrenBounds);
            ++visibleChildren;
        }

        UiRect subtreeBounds{.Min = {0.f, 0.f}, .Max = baseSize};
        UpdateSubtreeBounds(growth, childrenBounds, visibleChildren, config.Spacing, subtreeBounds);

        outLayers.at(depth).push_back({
            .TreeNode = node,
            .Bounds = {.Min = {0.f, 0.f}, .Max = baseSize},
            .SubtreeBounds = subtreeBounds
        });

        return subtreeBounds;
    }

    // Maps each GrowthDir to the (parent port, child port) pair that connectors should
    // attach to. Single source of truth -- if you add a new GrowthDir, the compiler will
    // remind you to add a case here and connectors will Just Work for it.
    // Returns std::nullopt for layouts that have no canonical port pair.
    static constexpr std::optional<std::pair<ConnectPoint, ConnectPoint>> PortsForGrowth(GrowthDir growth) {
        switch(growth) {
            using enum GrowthDir;
            case TopDown:   return std::pair{ConnectPoint::South, ConnectPoint::North};
            case BottomUp:  return std::pair{ConnectPoint::North, ConnectPoint::South};
            case LeftRight: return std::pair{ConnectPoint::East,  ConnectPoint::West};
        }
        return std::nullopt;
    }

    template<typename T>
    static void RenderConnectors(
        const std::vector<LayoutLayer<T>>& layers,
        const TreeConfig& config,
        GrowthDir growth,
        f32 zoom = 1.f
    ) {
        const auto ports = PortsForGrowth(growth);
        if(!ports) return;
        const auto [parentPort, childPort] = *ports;

        using TreeNode = typename Tree<RenderNode<T>>::Node;
        std::unordered_map<TreeNode*, const LayoutNode<T>*> layoutByTreeNode;
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

                    const auto parentPoint = node.GetConnectPoint(parentPort);
                    const auto childPoint = childIt->second->GetConnectPoint(childPort);

                    auto connection = Connection{
                        .From = parentPoint,
                        .To = childPoint,
                        .Thickness = config.ConnectorThickness * zoom,
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

    template<typename T>
    static void BuildLayoutVertical(GrowthDir growth, Tree<RenderNode<T>>& tree, const TreeConfig& config, std::vector<LayoutLayer<T>>& outLayers);

    template<typename T>
    static void BuildLayoutLeftRight(Tree<RenderNode<T>>& tree, const TreeConfig& config, std::vector<LayoutLayer<T>>& outLayers);
}

#include "TreePanel.Vertical.inl"
#include "TreePanel.Horizontal.inl"

namespace Ui {
    template<typename T, typename RenderFn>
    void TreePanel<T, RenderFn>::BuildLayout() {
        switch(m_TreeConfig->Growth) {
            using enum GrowthDir;
        case TopDown:
        case BottomUp:
            Details::BuildLayoutVertical(m_TreeConfig->Growth, *m_Tree, *m_TreeConfig, m_Layers);
            break;
        case LeftRight:
            Details::BuildLayoutLeftRight(*m_Tree, *m_TreeConfig, m_Layers);
            break;
        }

        const auto pan = GetPanOffset();
        const auto anchor = GetAnchor(m_TreeConfig->Anchor);
        const auto zoom = GetZoom();
        // Anchor stays in unscaled panel-local coords, so it's also the layout origin
        // that wheel-zoom math needs to compensate around.
        SetContentOrigin(anchor);

        auto TransformRect = [&](UiRect& rect) {
            rect.Min = {rect.Min.x * zoom + pan.x + anchor.x, rect.Min.y * zoom + pan.y + anchor.y};
            rect.Max = {rect.Max.x * zoom + pan.x + anchor.x, rect.Max.y * zoom + pan.y + anchor.y};
        };
        for(auto& layer : m_Layers) {
            for(auto& node : layer) {
                TransformRect(node.Bounds);
                TransformRect(node.SubtreeBounds);
            }
        }
    }

    template<typename T, typename RenderFn>
    void TreePanel<T, RenderFn>::RenderConnectors() const {
        if(m_TreeConfig->Connect == ConnectStyle::None) return;
        Details::RenderConnectors(m_Layers, *m_TreeConfig, m_TreeConfig->Growth, GetZoom());
    }

    template<typename T, typename RenderFn>
    void TreePanel<T, RenderFn>::RenderNodes() {
        const auto zoom = GetZoom();
        for(const auto& layer: m_Layers) {
            for(const auto& node: layer) {
                if(!node.TreeNode || !node.TreeNode->Value.Visible) continue;
                ImGui::SetCursorPos(node.Bounds.Min);
                ImGui::BeginChild(
                    ImGui::GetID(static_cast<const void*>(node.TreeNode)),
                    node.Bounds.GetSize(),
                    0,
                    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
                );
                ImGui::SetWindowFontScale(zoom);
                m_RenderFn(node.TreeNode->Value.Value);
                if(m_OnActivate && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0)) {
                    m_OnActivate(node.TreeNode->Value);
                }
                ImGui::EndChild();
            }
        }
    }
}
