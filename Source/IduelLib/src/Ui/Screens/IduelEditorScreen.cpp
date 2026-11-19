#include "Iduel/Ui/Screens/IduelScreens.h"
#include "Iduel/Economy/Generator.h"
#include "Iduel/Economy/EconomyState.h"
#include "Iduel/Simulation/Simulation.h"
#include "Iduel/Simulation/ParticipantState.h"
#include "Iduel/Simulation/IStrategy.h"
#include "Iduel/Curve.h"

#include "Manage/TickManager.h"
#include "Utilities/Handle.h"
#include "Ui/UiUtil.h"

#include <imgui.h>
#include <implot.h>

#include <array>
#include <cmath>
#include <map>
#include <memory>
#include <optional>
#include <vector>

namespace {
	using namespace Iduel;

	constexpr int DataSize = 1000;
    std::vector<ScopedHandle> TickHandles;
    int RandomPlayers = 3;
    f64 TargetAmount = 10'000.0;

    struct CurveEditState {
        std::string Draft;
        std::optional<std::string> ErrorMessage;
    };

    static constexpr std::array<f64, DataSize> XVals = [] {
        std::array<f64, DataSize> arr{};
        for (int i = 0; i < DataSize; i++) {
            arr[i] = static_cast<f64>(i);
        }
        return arr;
    }();

	struct GeneratorPlot {
		Generator Gen{
            .Thresholds = {{10}, {25}, {50}, {100}, {200}, {300}, {400}, {500}}
        };
		bool Dirty{true};
        CurveEditState IncomeState{Gen.Income.GetExpression(), std::nullopt};
		CurveEditState CostState{ Gen.Cost.GetExpression(), std::nullopt };
		std::array<f64, DataSize> IncomeData{};
		std::array<f64, DataSize> CostData{};

		void Generate() {
			if (!Dirty) return;
			for (int i = 0; i < DataSize; i++) {
				auto seconds = static_cast<f64>(Gen.GetTimePerPayout(i).count()) / 1000.0;
				IncomeData[i] = Gen.GetIncomePerPayout(i) / seconds;
				CostData[i] = Gen.GetCost(i);
			}
			Dirty = false;
		}
	};
    std::vector<GeneratorPlot> GenPlots{};

    struct ComparisonRun {
        std::vector<Generator> Generators{};
        std::vector<std::unique_ptr<IStrategy>> Strategies{};
        std::unique_ptr<Simulation> Sim;
        BaseTime Elapsed{ZeroTime};
        f64 Target{TargetAmount};
		std::map<BaseTime, std::vector<f64>> ParticipantWallets{};
        bool Finished{false};

        ComparisonRun(size_t randCount) {
            if (GenPlots.empty()) return;

            for (const auto& genPlot : GenPlots) {
                Generators.push_back(genPlot.Gen);
            }

            std::vector<size_t> genCounts(Generators.size(), 0);
            genCounts[0] = 1;
            std::vector<ParticipantState> participants{};
            std::vector<EconomyState> economies{};
            std::vector<ParticipantBinding> bindings{};

            Strategies.push_back(std::make_unique<NoPurchasesStrategy>());
            Strategies.push_back(std::make_unique<GreedyStrategy>());
            for (size_t i = 0; i < randCount; i++) {
                Strategies.push_back(std::make_unique<RandomStrategy>());
            }

            for (size_t i = 0; i < randCount + 2; i++) {
                participants.push_back({
                    .Strategy = Strategies[i].get(),
                    .Wallet = 0.0
                    });
                economies.push_back({
                    .GenCounts = genCounts,
                    .Generators = Generators,
                    .PayoutElapsed = std::vector<BaseTime>(Generators.size(), ZeroTime),
                    });
                bindings.push_back({ i, i });
            }

            Sim = std::make_unique<Simulation>( economies, participants, bindings, Target );
			ParticipantWallets[ZeroTime] = std::vector<f64>(participants.size(), 0.0);
        }

        void Tick(BaseTime elapsed) {
            if(!Sim || Finished || elapsed <= ZeroTime) return;

            // running at 5x speed
            for(size_t i = 0; i < 5; i++) {
                Sim->Tick(OneSecond);
                Elapsed += OneSecond;
                for(const auto& p : Sim->GetParticipants()) {
					ParticipantWallets[Elapsed].push_back(p.Wallet);
				}

                if(Sim->GetFinished().size() > 0) {
					Finished = true;
					break;
				}
            }
        }
    };

	std::optional<ComparisonRun> CurrentRun{};

    void RenderIncomeAnchors(GeneratorPlot& plot, size_t index, const ImVec4& color) {
        ImGui::PushID(static_cast<int>(index));

        for(size_t i = 0; i < plot.Gen.Thresholds.size(); i++) {
            auto& threshold = plot.Gen.Thresholds[i];
            auto x = static_cast<f64>(threshold.Index);
            auto seconds = static_cast<f64>(plot.Gen.GetTimePerPayout(threshold.Index).count()) / static_cast<f64>(OneSecond.count());
			auto y = plot.Gen.GetIncomePerPayout(threshold.Index) / seconds;
            if(!std::isfinite(y) || y <= 0.0) continue;

            auto oldY = y;
            if(ImPlot::DragPoint(static_cast<int>(i), &x, &y, color, 5.f, ImPlotDragToolFlags_Delayed)) {
                auto mul = threshold.IncomeMultiplier * (y / oldY);
                if(std::isfinite(mul) && mul > 0.0 && mul != threshold.IncomeMultiplier) {
					threshold.IncomeMultiplier = mul;
					plot.Dirty = true;
				}
            }
        }

        ImGui::PopID();
    }

    void RenderGeneratorGraph(GeneratorPlot& gen, size_t index) {
        auto label = gen.Gen.Name + "###Generator" + std::to_string(index);
        auto t = GenPlots.size() > 1
            ? static_cast<f32>(index) / static_cast<f32>(GenPlots.size() - 1)
            : 0.5f;
        auto color = ImPlot::SampleColormap(t, ImPlotColormap_Viridis);
        RenderIncomeAnchors(gen, index, color);
        gen.Generate();

        ImPlotSpec incomeStyle, costStyle;
        incomeStyle.LineColor = color;
        incomeStyle.LineWeight = 3.f;
        costStyle.LineColor = color;
        costStyle.LineWeight = 1.f;

        ImPlot::PlotLine(label.c_str(), XVals.data(), gen.IncomeData.data(), DataSize, incomeStyle);
        ImPlot::PlotLine(label.c_str(), XVals.data(), gen.CostData.data(), DataSize, costStyle);
    }

    void RenderComparisonPlot() {
        if(!CurrentRun || CurrentRun->ParticipantWallets.empty()) return;

        auto& run = *CurrentRun;
        std::vector<f64> times;
		std::vector<std::vector<f64>> wallets(run.Strategies.size());

        for(const auto& [time, balances] : run.ParticipantWallets) {
            times.push_back(static_cast<f64>(time.count()) / static_cast<f64>(OneSecond.count()));
            for(size_t i = 0; i < balances.size(); i++) {
                wallets[i].push_back(balances[i]);
            }
        }

        ImGui::PushFont(GetFont(FontSizes::H3));
        ImPlot::PushStyleVar(ImPlotStyleVar_LegendInnerPadding, ImVec2{4.f, 4.f});
        ImPlot::PushStyleVar(ImPlotStyleVar_LegendSpacing, ImVec2{4.f, 0.f});
        if(ImPlot::BeginPlot("Strategy Comparison")) {
            ImPlot::SetupAxes("Seconds", "Wallet", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
            for(size_t i = 0; i < wallets.size(); i++) {
                auto label = i == 0
                    ? std::string("No Purchases")
                    : i == 1
                        ? std::string("Greedy")
                        : "Random " + std::to_string(i - 1);

				ImPlot::PlotLine(label.c_str(), times.data(), wallets[i].data(), static_cast<int>(times.size()));
            }

			std::array<f64, 2> targetTimes{ 0.0, times.back() };
		    std::array<f64, 2> targetValues{run.Target, run.Target};
			ImPlot::PlotLine("Target", targetTimes.data(), targetValues.data(), 2);

            ImPlot::EndPlot();
        }

        ImGui::PopFont();
        ImPlot::PopStyleVar(2);
    }

    void RenderIncomeCostPlot() {
        ImGui::Text("Cost vs Income");
        if (ImPlot::BeginPlot("Cost vs Income Plot")) {
            ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_SymLog);
            for (size_t i = 0; i < GenPlots.size(); i++) {
                RenderGeneratorGraph(GenPlots[i], i);
            }

            ImPlot::EndPlot();
        }
    }

    bool RenderCurveControls(const char* label, Curve& curve, CurveEditState& editState) {
        ImGui::PushID(label);

        std::array<char, 64> buf{};
		editState.Draft.copy(buf.data(), buf.size() - 1);
		ImGui::SetNextItemWidth(ImGui::GetFontSize() * 20.f);
        bool changed = false;
        if(ImGui::InputText("##Curve", buf.data(), 64)) {
            editState.Draft = buf.data();
			auto result = Math::TryEval(editState.Draft, 1.0);
            if(result) {
				editState.ErrorMessage.reset();
                curve.SetExpression(editState.Draft);
                changed = true;
            } else {
                editState.ErrorMessage = result.error();
            }
        }

        ImGui::PopID();
        return changed;
    }

    void RenderControls() {
        if(ImGui::BeginTable("Generators", 3, ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 1.f);
            ImGui::TableSetupColumn("Income", ImGuiTableColumnFlags_WidthStretch, 2.f);
            ImGui::TableSetupColumn("Cost", ImGuiTableColumnFlags_WidthStretch, 2.f);
            ImGui::TableHeadersRow();
            for(size_t i = 0; i < GenPlots.size(); i++) {
                auto& plot = GenPlots[i];
                ImGui::PushID(static_cast<int>(i));
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                std::array<char, 64> name{};
                plot.Gen.Name.copy(name.data(), name.size() - 1);
                ImGui::SetNextItemWidth(-1.f);
                if(ImGui::InputText("##Name", name.data(), name.size())) {
					plot.Gen.Name = name.data();
				}

                ImGui::TableSetColumnIndex(1);
				plot.Dirty |= RenderCurveControls("Income", plot.Gen.Income, plot.IncomeState);
                ImGui::TableSetColumnIndex(2);
				plot.Dirty |= RenderCurveControls("Cost", plot.Gen.Cost, plot.CostState);

                ImGui::PopID();
            }

            ImGui::EndTable();
        }

        for(auto& plot : GenPlots) {
            plot.Generate();
        }

        if (ImGui::Button("Add Generator")) {
            auto z = GenPlots.size();
            GenPlots.push_back({});
			GenPlots.back().Gen.Name = "Gen" + std::to_string(z + 1);
            GenPlots.back().Generate();
        }

        ImGui::SeparatorText("Simulation");
		ImGui::InputDouble("Target Amount", &TargetAmount, 0.0, 0.0, "%.2f");
		ImGui::InputInt("Random Players", &RandomPlayers, 0);
		ImGui::BeginDisabled(CurrentRun.has_value());
        if(ImGui::Button("Run Simulation")) {
			CurrentRun = ComparisonRun{ static_cast<size_t>(RandomPlayers) };
		}
        ImGui::EndDisabled();
		ImGui::BeginDisabled(!CurrentRun.has_value());
        if(ImGui::Button("Reset Simulation")) {
            CurrentRun = std::nullopt;
        }
        ImGui::EndDisabled();
    }

    void TickComparison(BaseTime elapsed) {
		if (!CurrentRun.has_value()) return;
		CurrentRun->Tick(elapsed);
	}
}

namespace Iduel::IduelUi::Screens::Editor {
    bool Initialize() {
        GenPlots.reserve(8);
        GenPlots.push_back({});
		auto& gen = GenPlots.back().Gen;
        gen.Income.SetExpression("2x^2");
		gen.Cost.SetExpression("1.1 ^ x");
        gen.Name = "Gen1";
        auto& plot = GenPlots.back();
        plot.IncomeState.Draft = plot.Gen.Income.GetExpression();
		plot.CostState.Draft = plot.Gen.Cost.GetExpression();
	
        for (auto& generator : GenPlots) {
            generator.Generate();
        }

		TickManager::Get().Register(TickHandles, TickComparison);
        return true;
    }

    void ShutDown() {}

    void Render() {
        RenderIncomeCostPlot();
        RenderControls();
        RenderComparisonPlot();
    }
}