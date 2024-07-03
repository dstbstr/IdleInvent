#include "Ui/Ui.h"

#include "Platform/Graphics.h"
#include "Ui/Screens/Splash.h"
#include "Ui/EmptyBar.h"
#include "Ui/NavBar.h"
#include "Ui/ResourceBar.h"
#include "Ui/Screens.h"
#include "Ui/TopBar.h"
#include "Ui/Popups.h"
#include "Ui/NewsFeed.h"
#include "Ui/PrimaryResource.h"
#include "EngineState.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/Instrumentation/Logging.h"
#include "imgui.h"

#include <thread>
#include <functional>

namespace {
    bool hasInitialized {false};

    class UiBuilder {
    public:
        UiBuilder(float width, float height) : m_Width(width), m_Height(height) {}

        UiBuilder& AddPart(float height, std::function<void()> part) {
            m_Heights.push_back(height);
            m_Parts.push_back(part);
            return *this;
        }

        void Build() {
            float startY = 0;
            for(auto i = 0u; i < m_Parts.size(); ++i) {
                ImGui::SetNextWindowPos(ImVec2(0, startY));
                ImGui::SetNextWindowSize(ImVec2(m_Width, m_Heights[i]));
                m_Parts[i]();
                startY += m_Heights[i];
                DR_ASSERT_MSG(startY <= m_Height, "UI parts exceed window height");
            }
        }

    private:
        float m_Width;
        float m_Height;

        std::vector<float> m_Heights{};
        std::vector<std::function<void()>> m_Parts{};
    };
}

namespace Ui {
    bool Initialize() {
        return Screens::Splash::Initialize();
    }

    void ShutDown() {
        TopBar::ShutDown();
        ResourceBar::ShutDown();
        PrimaryResource::ShutDown();
        NavBar::ShutDown();
        Screens::ShutDownScreens();
        Popups::ShutDown();
        NewsFeed::ShutDown();
        hasInitialized = false;
    }

    void Render() {
        auto [w, h] = Graphics::GetScreenSize();
        auto width = static_cast<float>(w);
        auto height = static_cast<float>(h);

        if(Screens::GetActiveScreen() == Screen::Splash) {
            if(ServiceLocator::Get().GetRequired<EngineState>().FrameNum == 2) {
                if(!hasInitialized) {
                    bool success = Screens::InitializeScreens() &&
                            TopBar::Initialize() &&
                            ResourceBar::Initialize() &&
                            NavBar::Initialize() &&
                            PrimaryResource::Initialize() &&
                            NewsFeed::Initialize();
                    DR_ASSERT_MSG(success, "Failed to initialize UI");
                    hasInitialized = true;
                }
            }
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(width, height));
            Ui::Screens::RenderActiveScreen();
            return;
        }

        UiBuilder(width, height)
            .AddPart(height * 0.03F, EmptyBar::Render)
            .AddPart(height * 0.03F, TopBar::Render)
            .AddPart(height * 0.7F, Screens::RenderActiveScreen)
            .AddPart(height * 0.07F, ResourceBar::Render)
            .AddPart(height * 0.065F, NavBar::Render)
            .AddPart(height * 0.08F, PrimaryResource::Render)
            .Build();

        // static bool WantTextInputLast = false;
        // if(io.WantTextInput && !WantTextInputLast) { ShowSoftKeyboardInput(); }
        // WantTextInputLast = io.WantTextInput;
    }

} // namespace Ui