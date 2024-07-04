#include "Ui/Layout/Layout.h"

#include "Ui/Layout/EmptyBar.h"
#include "Ui/Layout/NavBar.h"
#include "Ui/Layout/NewsFeed.h"
#include "Ui/Layout/Popups.h"
#include "Ui/Layout/PrimaryResource.h"
#include "Ui/Layout/ResourceBar.h"
#include "Ui/Layout/TopBar.h"
#include "Ui/Screens/Screens.h"
#include "Platform/Graphics.h"

#include "Core/Instrumentation/Logging.h"

#include "imgui.h"

#include <functional>
#include <vector>

namespace {
    float screenWidth = 0;
    float screenHeight = 0;

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
                ImGui::SetNextWindowSize(ImVec2(screenWidth, m_Heights[i]));
                m_Parts[i]();
                startY += m_Heights[i];
                DR_ASSERT_MSG(startY <= screenHeight, "UI parts exceed window height");
            }
        }

    private:
        std::vector<float> m_Heights{};
        std::vector<std::function<void()>> m_Parts{};
    };
}

namespace Ui::Layout {
    bool Initialize() {
        auto [w, h] = Graphics::GetScreenSize();
        screenWidth = static_cast<float>(w);
        screenHeight = static_cast<float>(h);

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
        UiBuilder()
            .AddPart(screenHeight * 0.03F, EmptyBar::Render)
            .AddPart(screenHeight * 0.03F, TopBar::Render)
            .AddPart(screenHeight * 0.7F, Screens::Render)
            .AddPart(screenHeight * 0.07F, ResourceBar::Render)
            .AddPart(screenHeight * 0.065F, NavBar::Render)
            .AddPart(screenHeight * 0.08F, PrimaryResource::Render)
            .Build();

    }
}