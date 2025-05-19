#include "TechMaze/Ui/Ui.h"
#include "TechMaze/Ui/TopContent.h"
#include "TechMaze/Ui/MainContent.h"
#include "TechMaze/Ui/BottomContent.h"

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
} // namespace

namespace TechMaze::Ui {
    bool Initialize() {
        static const auto headerFont = "DroidSans.ttf";
        DR_ASSERT(Graphics::TryLoadFont("H1", headerFont, 32.0f));
        DR_ASSERT(Graphics::TryLoadFont("H2", headerFont, 24.0f));
        DR_ASSERT(Graphics::TryLoadFont("H3", headerFont, 16.0f));
        DR_ASSERT(Graphics::TryLoadFont("H4", headerFont, 12.0f));

        return true;
    }
    void Render() {
        auto screenHeight = Graphics::ScreenHeight;
        auto emptyBar = []() {};
        UiBuilder()
            .AddPart(screenHeight * 0.05f, Screens::TopContent::Render)
            .AddPart(screenHeight * 0.75f, Screens::MainContent::Render)
            .AddPart(screenHeight * 0.2f, Screens::BottomContent::Render)
            .Build();
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
