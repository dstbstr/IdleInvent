#include "SampleUI/Screens/SampleScreen.h"

#include <imgui.h>
#include <implot.h>

#include <array>
#include <optional>
#include <span>
#include <vector>

namespace {
    constexpr s32 DataSize = 1000;

	static constexpr std::array<f64, DataSize> XVals = [] {
		std::array<f64, DataSize> arr{};
		for (int i = 0; i < DataSize; i++) {
			arr[i] = static_cast<f64>(i);
		}
		return arr;
	}();

    struct Threshold {
        s32 Index{};
        f64 Multiplier{};
    };

    struct CurveTerm {
        f64 Coef{ 0.0 };
        bool ScaleX{ false };

        f64 Evaluate(f64 x) const {
            return Coef * (ScaleX ? x : 1.0);
        }
    };

    struct Curve {
        CurveTerm ConstTerm{ 0.0, false };
        CurveTerm LinearTerm{ 1.0, false };
        CurveTerm QuadTerm{ 1.0, false };
        CurveTerm ExpTerm{ 1.0, false };

        // A + B * (C ^ D)
        f64 Evaluate(f64 x) const {
            return ConstTerm.Evaluate(x) +
                LinearTerm.Evaluate(x) *
                std::pow(QuadTerm.Evaluate(x), ExpTerm.Evaluate(x));
        }
    };

    struct Generator {
        Curve Income{};
        Curve Cost{};
		std::array<f64, DataSize> IncomeData{};
		std::array<f64, DataSize> CostData{};
        std::vector<Threshold> Thresholds{};

        std::string Name{};
        bool Dirty{true};

        f64 GetIncomeRate(s32 owned) const {
            auto rate = Income.Evaluate(static_cast<f64>(owned));
            for (const auto& threshold : Thresholds) {
                if (static_cast<f64>(owned) >= threshold.Index) {
                    rate *= threshold.Multiplier;
                }
            }
            return rate;
        }

        f64 GetCost(s32 owned) const {
            return Cost.Evaluate(static_cast<f64>(owned));
        }

		void Generate() {
            if(!Dirty) return;

			for (s32 i = 0; i < DataSize; i++) {
				IncomeData[i] = GetIncomeRate(i);
				CostData[i] = GetCost(i);
			}
            Dirty = false;
		}
    };

    std::vector<Generator> Generators{};

    void RenderGeneratorGraph(const Generator& gen, size_t index) {
        auto label = gen.Name + "###Generator" + std::to_string(index);
        auto t = Generators.size() > 1
            ? static_cast<f32>(index) / static_cast<f32>(Generators.size() - 1)
            : 0.5f;
        auto color = ImPlot::SampleColormap(t, ImPlotColormap_Viridis);
        ImPlotSpec incomeStyle, costStyle;
        incomeStyle.LineColor = color;
        incomeStyle.LineWeight = 3.f;
        costStyle.LineColor = color;
        costStyle.LineWeight = 1.f;

        ImPlot::PlotLine(label.c_str(), XVals.data(), gen.IncomeData.data(), DataSize, incomeStyle);
        ImPlot::PlotLine(label.c_str(), XVals.data(), gen.CostData.data(), DataSize, costStyle);
    }

    void RenderIncomeCostPlot() {
        ImGui::Text("Cost vs Income");
        if (ImPlot::BeginPlot("Cost vs Income Plot")) {
            ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
            for (size_t i = 0; i < Generators.size(); i++) {
                RenderGeneratorGraph(Generators[i], i);
            }

            ImPlot::EndPlot();
        }
    }

	void RenderThresholdControls(Generator& gen, Threshold& threshold) {
		ImGui::SetNextItemWidth(ImGui::GetFontSize() * 7.f);
		if (ImGui::InputInt("##Lvl", &threshold.Index, 0)) {
			gen.Dirty = true;
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6.f);
		if (ImGui::InputDouble("##Mul", &threshold.Multiplier, 0.0, 0.0, "%.2f")) {
			gen.Dirty = true;
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("-")) {
			threshold = gen.Thresholds.back();
			gen.Thresholds.pop_back();
			gen.Dirty = true;
		}
	}
        
    bool RenderTermControls(const char* label, CurveTerm& term) {
        ImGui::PushID(label);
        ImGui::TextUnformatted(label);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 5.f);
		auto changed = ImGui::InputDouble("##Coef", &term.Coef, 0.0, 0.0, "%.2f");
        ImGui::SameLine();
		changed |= ImGui::Checkbox("* x", &term.ScaleX);
        ImGui::PopID();
        return changed;
    }

    bool RenderCurveControls(const char* label, Curve& curve) {
        ImGui::PushID(label);
        ImGui::TextUnformatted(label);
        ImGui::TextUnformatted("A + B * (C ^ D)");

		auto changed = RenderTermControls("A", curve.ConstTerm);
		changed |= RenderTermControls("B", curve.LinearTerm);
		changed |= RenderTermControls("C", curve.QuadTerm);
		changed |= RenderTermControls("D", curve.ExpTerm);

        ImGui::PopID();
        return changed;
    }

    void RenderGeneratorControls(Generator& gen) {
        std::array<char, 64> buf{};
        gen.Name.copy(buf.data(), buf.size() - 1);

        if (ImGui::InputText("##Name", buf.data(), 64)) {
            gen.Name = buf.data();
        }

		if (ImGui::CollapsingHeader("Income Curve")) {
			gen.Dirty |= RenderCurveControls("Income Curve", gen.Income);
		}
        if(ImGui::CollapsingHeader("Cost Curve")) {
    		gen.Dirty |= RenderCurveControls("Cost Curve", gen.Cost);
        }

        if(ImGui::CollapsingHeader("Thresholds")) {
            for (size_t i = 0; i < gen.Thresholds.size(); ++i) {
                ImGui::PushID(i);
                RenderThresholdControls(gen, gen.Thresholds[i]);
                ImGui::PopID();
            }
            if (ImGui::SmallButton("+")) {
                gen.Thresholds.push_back({ 1, 1.0 });
                gen.Dirty = true;
            }
        }
    }

    void RenderControls() {
        for (size_t genIndex = 0; genIndex < Generators.size(); genIndex++) {
            ImGui::PushID(genIndex);
            auto& generator = Generators[genIndex];
            auto header = generator.Name + "###Generator";
            if (ImGui::CollapsingHeader(header.c_str())) {
				RenderGeneratorControls(generator);
            }

            generator.Generate();
            ImGui::PopID();
        }

        if (ImGui::Button("Add Generator")) {
            auto z = Generators.size();
            Generators.push_back({
                .Income = {},
                .Cost = {},
                .Name = "Gen" + std::to_string(z + 1)
                });
            Generators.back().Generate();
        }

    }
}

namespace SampleUI::Screens::SampleChart {
    bool Initialize() {
        Generators.push_back({
            .Income = {
                .ConstTerm = { 1.0, false },
                .LinearTerm = { 2.0, true }
            },
            .Cost = {
                .QuadTerm = { 1.1, false},
                .ExpTerm = {1.0, true }
            },
            .Name = "Gen1"
		});
		for (auto& generator : Generators) {
			generator.Generate();
		}

        return true;
    }

    void ShutDown() {}

    void Render() {
        RenderSampleScreen("Sample Chart", [] {
			RenderIncomeCostPlot();
            RenderControls();
        });
    }
}