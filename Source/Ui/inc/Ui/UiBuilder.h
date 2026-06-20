#pragma once

#include "Platform/Graphics.h"
#include "Ui/UiUtil.h"

#include <vector>
#include <functional>

class UiBuilder {
public:
    UiBuilder& AddPart(float height, const std::function<void()>& part) {
        m_Heights.push_back(height);
        m_Parts.push_back(part);
        return *this;
    }

    void Build() {
        float startY = 0;
        std::string label;
        for(auto i = 0u; i < m_Parts.size(); ++i) {
            ImGui::SetNextWindowPos(ImVec2(0, startY));
            ImGui::SetNextWindowSize(ImVec2(Graphics::ScreenWidth, m_Heights.at(i)));
            label = std::format("Section {}", i);
            ImGui::Begin(label.c_str(), nullptr, BaseUiFlags);
            m_Parts.at(i)();
            ImGui::End();
            startY += m_Heights.at(i);
            // DR_ASSERT_MSG(startY <= Graphics::ScreenHeight, "UI parts exceed window height");
        }
    }

private:
    std::vector<float> m_Heights{};
    std::vector<std::function<void()>> m_Parts{};
};
