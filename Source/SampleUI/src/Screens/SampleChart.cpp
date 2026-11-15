#include "SampleUI/Screens/SampleScreen.h"

#include <imgui.h>
#include <implot.h>

#include <array>

namespace {
    constexpr auto XVals = []{
        std::array<double, 100> data{};
        for(auto i = 0; i < data.size(); i++) {
			data[i] = static_cast<double>(i);
        }
        return data;
    }();

    constexpr auto LinearData = XVals;
	constexpr auto QuadraticData = [] {
		auto data = XVals;
		for (auto i = 0; i < data.size(); i++) {
			data[i] *= data[i];
        }
        return data;
    }();
}

namespace SampleUI::Screens::SampleChart {
    bool Initialize() { return true;}
    void ShutDown() {}

    void Render() {
        RenderSampleScreen("Sample Chart", [] {
		    ImGui::Text("Sample Chart");
            if(ImPlot::BeginPlot("Sample Plot")) {
				ImPlot::PlotLine("Linear", &XVals[0], &LinearData[0], LinearData.size());
				ImPlot::PlotLine("Quadratic", &XVals[0], &QuadraticData[0], QuadraticData.size());

                ImPlot::EndPlot();
            }
        });
    }
}