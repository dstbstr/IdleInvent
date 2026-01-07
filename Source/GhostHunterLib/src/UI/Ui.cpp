#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/UI/TopContent.h"
#include "GhostHunter/UI/MainContent.h"
#include "GhostHunter/UI/BottomContent.h"

#include "Platform/Graphics.h"
#include "Instrumentation/Logging.h"
#include <functional>
#include <string>

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
            std::string label;
            for(auto i = 0u; i < m_Parts.size(); ++i) {
                ImGui::SetNextWindowPos(ImVec2(0, startY));
                ImGui::SetNextWindowSize(ImVec2(Graphics::ScreenWidth, m_Heights[i]));
                label = std::format("Section {}", i);
                ImGui::Begin(label.c_str(), nullptr, GhostHunter::Ui::BaseFlags);
                m_Parts[i]();
                ImGui::End();
                startY += m_Heights[i];
                DR_ASSERT_MSG(startY <= Graphics::ScreenHeight, "UI parts exceed window height");
            }
        }

    private:
        std::vector<float> m_Heights{};
        std::vector<std::function<void()>> m_Parts{};
    };
} // namespace

namespace GhostHunter::Ui {
    bool Initialize() {
        static const auto headerFont = "DroidSans.ttf";
        DR_ASSERT(Graphics::TryLoadFont("H1", headerFont, 32.0f));
        DR_ASSERT(Graphics::TryLoadFont("H2", headerFont, 24.0f));
        DR_ASSERT(Graphics::TryLoadFont("H3", headerFont, 16.0f));
        DR_ASSERT(Graphics::TryLoadFont("H4", headerFont, 12.0f));

        DR_ASSERT(Screens::BottomContent::Initialize());
        DR_ASSERT(Screens::MainContent::Initialize());
        DR_ASSERT(Screens::TopContent::Initialize());
        return true;
    }
    void Render() {
        auto screenHeight = Graphics::ScreenHeight;
        auto emptyBar = []() {};
        UiBuilder()
            .AddPart(screenHeight * 0.15f, Screens::TopContent::Render)
            .AddPart(screenHeight * 0.65f, Screens::MainContent::Render)
            .AddPart(screenHeight * 0.2f, Screens::BottomContent::Render)
            .Build();
    }
    void ShutDown() {
        Screens::TopContent::ShutDown();
        Screens::MainContent::ShutDown();
        Screens::BottomContent::ShutDown();
    }
    ImFont* GetFont(FontSizes fontSize) {
        switch(fontSize) {
            using enum FontSizes;
            case H1: return Graphics::GetFont("H1");
            case H2: return Graphics::GetFont("H2");
            case H3: return Graphics::GetFont("H3");
            case H4: return Graphics::GetFont("H4");
            default : return nullptr;
        }
    }
}
