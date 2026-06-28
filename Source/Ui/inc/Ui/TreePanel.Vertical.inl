// Vertical growth directions (TopDown, BottomUp).
namespace Ui::Details {
    template<typename T>
    static void PlaceLayersVertical(
        GrowthDir growth,
        typename Tree<RenderNode<T>>::Node* node,
        f32 left,
        f32 top,
        const TreeConfig& config,
        const std::unordered_map<typename Tree<RenderNode<T>>::Node*, LayoutNode<T>*>& layoutMap
    ) {
        if(!node || !node->Value.Visible) return;
        const auto it = layoutMap.find(node);
        if(it == layoutMap.end()) return;
        auto& ln = *it->second;

        const auto subtreeWidth = ln.SubtreeBounds.Size.x;
        const auto subtreeHeight = ln.SubtreeBounds.Size.y;
        const auto baseW = ln.Bounds.Size.x;
        const auto baseH = ln.Bounds.Size.y;

        const bool topDown = (growth == GrowthDir::TopDown);

        ln.SubtreeBounds.Pos = {left, top};
        ln.Bounds.Pos = {
            left + (subtreeWidth - baseW) / 2.f,
            topDown ? top : (top + subtreeHeight - baseH)
        };

        auto childrenWidth = 0.f;
        f32 childrenHeight = 0.f;
        size_t visibleChildren = 0;
        for(const auto& child: node->Children) {
            if(!child || !child->Value.Visible) continue;
            const auto cit = layoutMap.find(child.get());
            if(cit == layoutMap.end()) continue;
            childrenWidth += cit->second->SubtreeBounds.Size.x;
            childrenHeight = std::max(childrenHeight, cit->second->SubtreeBounds.Size.y);
            ++visibleChildren;
        }
        if(visibleChildren > 1) {
            childrenWidth += (visibleChildren - 1) * config.Spacing.x;
        }

        auto childLeft = left + (subtreeWidth - childrenWidth) / 2.f;
        const auto childBlockTop = top + baseH + config.Spacing.y;
        const auto childBlockBottom = top + childrenHeight;
        for(const auto& child: node->Children) {
            if(!child || !child->Value.Visible) continue;
            const auto cit = layoutMap.find(child.get());
            if(cit == layoutMap.end()) continue;
            const auto cw = cit->second->SubtreeBounds.Size.x;
            const auto ch = cit->second->SubtreeBounds.Size.y;
            const auto childTop = topDown ? childBlockTop : (childBlockBottom - ch);
            PlaceLayersVertical(growth, child.get(), childLeft, childTop, config, layoutMap);
            childLeft += cw + config.Spacing.x;
        }
    }

    template<typename T>
    static void BuildLayoutVertical(GrowthDir growth, Tree<RenderNode<T>>& tree, const TreeConfig& config, std::vector<LayoutLayer<T>>& outLayers) {
        outLayers.clear();
        auto* root = tree.Root();
        if(!root || !root->Value.Visible) return;

        BuildLayers(root, 0, growth, config, outLayers);

        std::unordered_map<typename Tree<RenderNode<T>>::Node*, LayoutNode<T>*> layoutMap;
        for(auto& layer: outLayers) {
            for(auto& ln: layer) {
                layoutMap.emplace(ln.TreeNode, &ln);
            }
        }

        const auto rootIt = layoutMap.find(root);
        if(rootIt == layoutMap.end()) return;
        const auto rootWidth = rootIt->second->SubtreeBounds.Size.x;
        const auto rootHeight = rootIt->second->SubtreeBounds.Size.y;
        const auto initialTop = (growth == GrowthDir::TopDown) ? 0.f : -rootHeight;
        PlaceLayersVertical(growth, root, -rootWidth / 2.f, initialTop, config, layoutMap);
    }
}
