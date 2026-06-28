#pragma once

#include "Ui/Panel.h"
#include "Ui/Connection.h"

#include <DesignPatterns/Tree.h>
#include <Platform/NumTypes.h>

#include <imgui.h>
#include <functional>

namespace Ui::Details {
    template<typename T>
    struct LayoutNode;

    template<typename T>
    using LayoutLayer = std::vector<LayoutNode<T>>;
}

namespace Ui {
    enum struct GrowthDir : u8 { TopDown, BottomUp, LeftRight, CenterOut };

    struct TreeConfig {
        GrowthDir Growth{GrowthDir::TopDown};
        ConnectStyle Connect{ConnectStyle::Line};
        Anchor Anchor{Anchor::TopCenter};
        ImVec2 Spacing{4.f, 4.f};
        f32 ConnectorThickness{1.f};
        ImU32 ConnectorColor{0xFFFFFFFF};
    };

    template<typename T>
    struct RenderNode {
        T Value;
        bool Visible{true};
        ImVec2 BaseSize{32.f, 32.f};
    };

    template<typename T, typename RenderFn>
    class TreePanel : public Panel {
        Tree<RenderNode<T>>* m_Tree;
        RenderFn m_RenderFn;
        const TreeConfig* m_TreeConfig;
        std::function<void(RenderNode<T>&)> m_OnActivate;
        std::vector<Details::LayoutLayer<T>> m_Layers;

    public:
        TreePanel(
            Rect bounds,
            std::optional<ImU32> backgroundColor,
            std::optional<ImTextureID> backgroundTexture,
            Tree<RenderNode<T>>& tree,
            const TreeConfig& treeConfig,
            RenderFn&& renderFn,
            std::function<void(RenderNode<T>&)> onActivate = nullptr
        )
            : Panel(bounds, backgroundColor, backgroundTexture)
            , m_Tree(&tree)
            , m_RenderFn(std::move(renderFn))
            , m_TreeConfig(&treeConfig)
            , m_OnActivate(std::move(onActivate)) {}

        TreePanel(
            const PanelConfig& panelConfig,
            Tree<RenderNode<T>>& tree,
            const TreeConfig& treeConfig,
            RenderFn&& renderFn,
            std::function<void(RenderNode<T>&)> onActivate = nullptr
        )
            : Panel(panelConfig)
            , m_Tree(&tree)
            , m_RenderFn(std::move(renderFn))
            , m_TreeConfig(&treeConfig)
            , m_OnActivate(std::move(onActivate)) {}

        void RenderImpl() override {
            m_Layers.clear();

            BuildLayout();

            RenderConnectors();

            RenderNodes();
        }

    private:
        void BuildLayout();
        void RenderConnectors() const;
        void RenderNodes();
    };
}
#include "TreePanel.inl"