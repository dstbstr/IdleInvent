#include "InventLib/Ui/Layout/Layout.h"
#include "InventLib/Ui/Layout/EmptyBar.h"
#include "InventLib/Ui/Layout/NavBar.h"
#include "InventLib/Ui/Layout/NewsFeed.h"
#include "InventLib/Ui/Layout/Popups.h"
#include "InventLib/Ui/Layout/PrimaryResource.h"
#include "InventLib/Ui/Layout/ResourceBar.h"
#include "InventLib/Ui/Layout/TopBar.h"
#include "InventLib/Ui/Screens/Screens.h"

#include <Platform/Graphics.h>
#include <Instrumentation/Logging.h>
#include <imgui.h>

#include <functional>
#include <vector>

namespace {

    class UiBuilder {
    public:
        UiBuilder& AddPart(float height, std::function<void()> part) {
            m_Heights.push_back(height);
            m_Parts.push_back(part);
            return *this;
        }

        void Build() {
            float startY = 0;
            for(auto i = 0u; i < m_Parts.size(); ++i) {
                ImGui::SetNextWindowPos(ImVec2(0, startY));
                ImGui::SetNextWindowSize(ImVec2(Graphics::ScreenWidth, m_Heights[i]));
                m_Parts[i]();
                startY += m_Heights[i];
                DR_ASSERT_MSG(startY <= Graphics::ScreenHeight, "UI parts exceed window height");
            }
        }

    private:
        std::vector<float> m_Heights{};
        std::vector<std::function<void()>> m_Parts{};
    };
}

namespace Ui::Layout {
    bool Initialize() {
        return EmptyBar::Initialize() &&
                NavBar::Initialize() &&
                NewsFeed::Initialize() &&
                Popups::Initialize() &&
                PrimaryResource::Initialize() &&
                ResourceBar::Initialize() &&
                TopBar::Initialize() &&
                Screens::Initialize();
    }

    void ShutDown() {
        Screens::ShutDown();
        EmptyBar::ShutDown();
        NavBar::ShutDown();
        NewsFeed::ShutDown();
        Popups::ShutDown();
        PrimaryResource::ShutDown();
        ResourceBar::ShutDown();
        TopBar::ShutDown();
    }

    void Render() {
        auto screenHeight = Graphics::ScreenHeight;
        UiBuilder()
            .AddPart(screenHeight * 0.03F, EmptyBar::Render)
            .AddPart(screenHeight * 0.08F, TopBar::Render)
            .AddPart(screenHeight * 0.65F, Screens::Render)
            .AddPart(screenHeight * 0.07F, ResourceBar::Render)
            .AddPart(screenHeight * 0.065F, NavBar::Render)
            .AddPart(screenHeight * 0.08F, PrimaryResource::Render)
            .Build();
    }
}