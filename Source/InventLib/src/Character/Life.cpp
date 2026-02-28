#include "InventLib/Character/Life.h"

#include "InventLib/Character/Society.h"
#include "InventLib/Effects/Effect.h"
#include "InventLib/GameState/GameSettings.h"
#include "InventLib/Projects/Building.h"
#include "InventLib/Projects/Expeditions.h"
#include "InventLib/Projects/Population.h"
#include "InventLib/Resources/InventResources.h"

#include <DesignPatterns/PubSub.h>
#include <DesignPatterns/ServiceLocator.h>
#include <GameState/GameTime.h>
#include <Instrumentation/Logging.h>
#include <Mechanics/Modifier.h>
#include <Mechanics/Progression.h>
#include <Platform/NumTypes.h>
#include <Resources/Resource.h>
#include <Resources/ResourceConversion.h>

#include <algorithm>

namespace {
    void TickResources(BaseTime elapsed, Invent::Life& life) {
        for(auto& [id, resource]: life.Resources) {
            auto name = static_cast<Invent::ResourceName>(id);
            if(name == Invent::ResourceName::Primary) continue; // Primary is handled by Work
            if(resource.Current >= resource.Capacity) continue; // Allow overfilling in case capacity decreases
            auto progress = life.ResourceProgressions.at(name).GetProgress(elapsed);
            resource.Current += progress;
            resource.Clamp();
        }
    }

    void AddProject(Invent::Life& life, Invent::Project&& project) {
        project.TimeCost =
            BaseTime(life.ProjectTimeCostModifiers.at(project.Type).Apply(project.TimeCost.count()));
        for(auto& [name, resource]: project.ResourceCost) {
            resource.Current = life.ProjectResourceCostModifiers.at(project.Type).Apply(resource.Current);
        }

        life.Projects.at(project.Type).push_back(std::move(project));
    }

    void CompleteInvention(
        const Invent::Project& project, Invent::Life& life, std::vector<Invent::Project>& outNewProjects
    ) {
        life.AvailableWorkers += project.CurrentWorkers;
        auto invention = Invent::InventionFromString(project.Name);
        if(!invention.has_value()) {
            DR_ASSERT_MSG(false, std::format("Invention {} not found", project.Name));
            return;
        }

        auto v = invention.value();
        life.Inventions.push_back(v);

        for(auto b: Invent::GetBuildings({v})) {
            life.Buildings[ToString(b)] = 0;
            outNewProjects.push_back(Invent::GetBuildingProject(b, 0));
        }

        outNewProjects.push_back(Invent::CreateExpedition(v));
    }

    void
    CompleteBuilding(const Invent::Project& project, Invent::Life& life, std::vector<Invent::Project>& outNewProjects) {
        auto building = Invent::BuildingFromString(project.Name);
        if(!building.has_value()) {
            DR_ASSERT_MSG(false, std::format("Building {} not found", project.Name));
            return;
        }
        auto b = building.value();

        life.Buildings.at(project.Name)++;
        auto effects = Invent::GetEffectsForBuilding(b);
        Log::Info(std::format("Finished building {}, now at level {}", ToString(b), life.Buildings.at(project.Name)));
        for(const auto& effect: effects) {
            life.ApplyEffect(effect);
        }

        outNewProjects.push_back(Invent::GetBuildingProject(b, life.Buildings.at(project.Name)));
        outNewProjects.back().CurrentWorkers = project.CurrentWorkers;
    }

    void CompletePopulation(
        const Invent::Project& project,
        Invent::Life& life,
        const Invent::Society& society,
        std::vector<Invent::Project>& outNewProjects
    ) {
        if(project.Name == Invent::Population::PopulationName) {
            life.CurrentPopulation++;
            auto newMaxWorkers = life.CurrentPopulation / life.WorkerDensity;
            if(newMaxWorkers > life.MaxWorkers) {
                life.AvailableWorkers += newMaxWorkers - life.MaxWorkers;
                life.MaxWorkers = newMaxWorkers;
            }
            if(life.CurrentPopulation < life.MaxPopulation) {
                outNewProjects.push_back(Invent::GetPopulationIncreaseProject(life.CurrentPopulation));
                outNewProjects.back().CurrentWorkers = project.CurrentWorkers;
            } else {
                life.AvailableWorkers += project.CurrentWorkers;
            }
        } else {
            if(!society.AlwaysMaxPopulation && life.CurrentPopulation == life.MaxPopulation) {
                outNewProjects.push_back(Invent::GetPopulationIncreaseProject(life.CurrentPopulation));
            }
            const auto populationIncrease = 10;
            life.MaxPopulation *= populationIncrease;
            outNewProjects.push_back(Invent::GetPopulationCapacityProject(life.MaxPopulation));
            outNewProjects.back().CurrentWorkers = project.CurrentWorkers;

            if(society.AlwaysMaxPopulation) {
                life.CurrentPopulation = life.MaxPopulation;
            }
        }
    }

    void CompleteExploration(
        const Invent::Project& project,
        Invent::Life& life,
        const Invent::Society& society,
        std::vector<Invent::Project>& outNewProjects
    ) {
        auto expedition = Invent::ExpeditionFromString(project.Name);
        if(!expedition.has_value()) {
            DR_ASSERT_MSG(false, std::format("Expedition {} not found", project.Name));
            return;
        }
        auto artifactFound =
            std::find(life.Artifacts.begin(), life.Artifacts.end(), project.EffectDescription) != life.Artifacts.end();
        auto outcome = Invent::GetExpeditionOutcome(expedition.value(), artifactFound, life.CurrentExplorationSuccessModifier);
        life.CurrentExplorationSuccessModifier = life.BaseExplorationSuccessModifier; // reset for next time

        outNewProjects.push_back(Invent::CreateExpedition(life.Inventions.back()));
        life.AvailableWorkers += project.CurrentWorkers;

        ServiceLocator::Get().GetRequired<PubSub<Invent::ExpeditionOutcome>>().Publish(outcome);
        switch(outcome) {
        case Invent::ExpeditionOutcome::Artifact: {
            life.Artifacts.push_back(project.EffectDescription);
            auto next = Invent::GetResearchProject(society.Specialty, life.Inventions.size());
            if(next.has_value()) {
                outNewProjects.push_back(next.value());
            }
            break;
        }
        case Invent::ExpeditionOutcome::Resources: {
            auto reward = project.ResourceCost * size_t(3);
            life.Resources += reward;
            break;
        }
        case Invent::ExpeditionOutcome::Nothing: {
            break;
        }
        case Invent::ExpeditionOutcome::Trajedy: {
            // Lost the workers
            life.AvailableWorkers -= project.CurrentWorkers;
            break;
        }
        }
    }

    void CompleteProject(
        const Invent::Project& project,
        Invent::Life& life,
        const Invent::Society& society,
        std::vector<Invent::Project>& outNewProjects
    ) {
        switch(project.Type) {
        case Invent::ProjectType::Research: CompleteInvention(project, life, outNewProjects); break;
        case Invent::ProjectType::Build: CompleteBuilding(project, life, outNewProjects); break;
        case Invent::ProjectType::Population: CompletePopulation(project, life, society, outNewProjects); break;
        case Invent::ProjectType::Explore: CompleteExploration(project, life, society, outNewProjects); break;
        default: DR_ASSERT_MSG(false, "Unsupported project completed"); break;
        }
    }

    void TickProjects(BaseTime elapsed, Invent::Life& life, const Invent::Society& society) {
        std::vector<Invent::Project> newProjects;
        bool dirty = false;

        for(auto& [type, projects]: life.Projects) {
            std::vector<Invent::Project> completedProjects;
            for(auto& project: projects) {
                project.Tick(elapsed);
                if(project.IsComplete()) {
                    completedProjects.push_back(project);
                    CompleteProject(project, life, society, newProjects);
                }
            }
            for(auto& project: completedProjects) {
                dirty = true;
                projects.erase(std::remove(projects.begin(), projects.end(), project), projects.end());
            }
        }
        for(auto& project: newProjects) {
            dirty = true;
            AddProject(life, std::move(project));
        }

        if(dirty) {
            life.SortProjects();
            life.ShiftWorkers();
        }
    }
} // namespace

namespace Invent {
    Life::Life(Society* society, const GameSettings& settings) : m_Society(society), m_Settings(&settings) {
        if(!society) return;

        for(auto name: AllResources()) {
            ResourceProgressions[name] = Progression{};
        }
        Modifier mod = {.Add = 0, .Mul = 1.0F};
        ResourceProgressions[ResourceName::Primary].AddPermanent({.Add = 1, .Mul = 1.0F});

        for(auto type: AllProjectTypes()) {
            ProjectTimeCostModifiers[type] = mod;
            ProjectResourceCostModifiers[type] = mod;
            Projects[type] = {};
        }

        AddProject(*this, GetResearchProject(society->Specialty, 0).value_or({}));
        AddProject(*this, GetPopulationCapacityProject(MaxPopulation));
        if(!society->AlwaysMaxPopulation) {
            AddProject(*this, GetPopulationIncreaseProject(CurrentPopulation));
        } else {
            CurrentPopulation = MaxPopulation;
        }

        SortProjects();
        auto GetCost = [&](ResourceName name) -> s64 {
            const s64 small = 5, medium = 10, large = 15;
            return society->Specialty == name ? small : society->Weakness == name ? large : medium;
        };
        auto MakeConverter = [&](ResourceName resource, const std::string& name) -> ResourceConversion {
            const s64 productAmt = 5;
            return ResourceConversionBuilder(name, GetAllResourceIds())
                .WithCost(static_cast<u16>(ResourceName::Primary), GetCost(resource))
                .WithProduct(static_cast<u16>(resource), productAmt)
                .Build();
        };
        ResourceConverters.push_back(MakeConverter(ResourceName::Followers, "Recruit"));
        ResourceConverters.push_back(MakeConverter(ResourceName::Knowledge, "Study"));
        ResourceConverters.push_back(MakeConverter(ResourceName::Money, "Earn"));
        ResourceConverters.push_back(MakeConverter(ResourceName::Power, "Train"));
    }

    void Life::Tick(BaseTime elapsed) {
        elapsed = BaseTime(TickModifier.Apply(elapsed.count()));
        Work(elapsed);
        TickResources(elapsed, *this);
        TickProjects(elapsed, *this, *this->m_Society);
    }

    void Life::Work(BaseTime elapsed) {
        auto& primary = Resources.at(ResourceName::Primary);
        if(primary.Current >= primary.Capacity) return;

        auto progress = ResourceProgressions.at(ResourceName::Primary).GetProgress(elapsed);
        progress *= MaxWorkers;
        // progress *= CurrentPopulation;
        primary.Current += progress;
        primary.Clamp();
    }

    void Life::ApplyEffect(const Effect& effect) {
        auto modSecondaryRate = [this](const Modifier& mod) {
            for(auto resource: SecondaryResources()) {
                ResourceProgressions[resource].AddPermanent(mod);
            }
        };
        auto modSecondaryCap = [this](const Modifier& mod) {
            for(auto resource: SecondaryResources()) {
                Resources[resource].AddCapacityModifier(mod);
            }
        };

        auto modProjectsRate = [this](const Modifier& mod) {
            for(auto& [type, modifier]: ProjectTimeCostModifiers) {
                modifier += mod;
            }
        };
        auto modProjectsEfficiency = [this](const Modifier& mod) {
            for(auto& [type, modifier]: ProjectResourceCostModifiers) {
                modifier += mod;
            }
        };

        auto modConversionPower = [this](const Modifier& mod) {
            for(auto& converter: ResourceConverters) {
                converter.ModifyTo(mod);
            }
        };

        auto& primary = Resources[ResourceName::Primary];
        auto mod = effect.Modifier;

        Log::Info(std::format("Applying effect: {} with modifier: {}", ToString(effect.Target), ToString(mod)));

        switch(effect.Target) {
        case EffectTarget::AllProjectEfficiency: modProjectsEfficiency(mod); break;
        case EffectTarget::AllProjectRate: modProjectsRate(mod); break;
        case EffectTarget::BuildRate: ProjectTimeCostModifiers[ProjectType::Build] += mod; break;
        case EffectTarget::BuildEfficiency: ProjectResourceCostModifiers[ProjectType::Build] += mod; break;
        case EffectTarget::ExploreRate: ProjectTimeCostModifiers[ProjectType::Explore] += mod; break;
        case EffectTarget::ExploreEfficiency: ProjectResourceCostModifiers[ProjectType::Explore] += mod; break;
        case EffectTarget::ResearchRate: ProjectTimeCostModifiers[ProjectType::Research] += mod; break;
        case EffectTarget::ResearchEfficiency: ProjectResourceCostModifiers[ProjectType::Research] += mod; break;
        case EffectTarget::PopulationRate: ProjectTimeCostModifiers[ProjectType::Population] += mod; break;
        case EffectTarget::PopulationCap: MaxPopulation = mod.Apply(MaxPopulation); break;
        case EffectTarget::WorkerDensity: {
            auto working = MaxWorkers - AvailableWorkers;
            WorkerDensity = std::max(u8(1), mod.Apply(WorkerDensity));
            MaxWorkers = CurrentPopulation / WorkerDensity;
            AvailableWorkers = MaxWorkers - working;
            break;
        }
        case EffectTarget::PrimaryRate: ResourceProgressions[ResourceName::Primary].AddPermanent(mod); break;
        case EffectTarget::PrimaryCap: primary.AddCapacityModifier(mod); break;
        case EffectTarget::AllSecondaryRate: modSecondaryRate(mod); break;
        case EffectTarget::AllSecondaryCap: modSecondaryCap(mod); break;
        case EffectTarget::FollowerCap: Resources[ResourceName::Followers].AddCapacityModifier(mod); break;
        case EffectTarget::FollowerRate: ResourceProgressions[ResourceName::Followers].AddPermanent(mod); break;
        case EffectTarget::KnowledgeRate: ResourceProgressions[ResourceName::Knowledge].AddPermanent(mod); break;
        case EffectTarget::KnowledgeCap: Resources[ResourceName::Knowledge].AddCapacityModifier(mod); break;
        case EffectTarget::MoneyRate: ResourceProgressions[ResourceName::Money].AddPermanent(mod); break;
        case EffectTarget::MoneyCap: Resources[ResourceName::Money].AddCapacityModifier(mod); break;
        case EffectTarget::PowerRate: ResourceProgressions[ResourceName::Power].AddPermanent(mod); break;
        case EffectTarget::PowerCap: Resources[ResourceName::Power].AddCapacityModifier(mod); break;
        case EffectTarget::ConversionPower: modConversionPower(mod); break;
        case EffectTarget::TimeShardCap: MaxTimeShards = mod.Apply(MaxTimeShards); break;
        case EffectTarget::TickPower: TickModifier += mod; break;
        case EffectTarget::ExploreSuccessRate: BaseExplorationSuccessModifier += mod; break;
        }
    }

    void Life::ShiftWorkers() {
        if(!m_Society->HasWorkerShift) return;

        // first unassign any workers that are no longer needed
        for(auto& [type, projects]: Projects) {
            for(auto& project: projects) {
                if(project.CurrentWorkers > 0 && project.TimeProgress == project.TimeCost) {
                    AvailableWorkers += project.CurrentWorkers;
                    project.CurrentWorkers = 0;
                }
            }
        }

        while(AvailableWorkers > 0) {
            // then assign workers to the highest priority projects (skipping projects that don't need more time)
            for(const auto type: m_Settings->ProjectPriority) {
                for(auto& project: Projects.at(type)) {
                    if(project.TimeProgress == project.TimeCost) continue;

                    project.CurrentWorkers++;
                    AvailableWorkers--;
                    if(AvailableWorkers == 0) return;
                }
            }
        }
    }

    void Life::ClearWorkers() {
        for(auto& [type, projects]: Projects) {
            for(auto& project: projects) {
                project.CurrentWorkers = 0;
            }
        }
        AvailableWorkers = MaxWorkers;
    }

    void Life::SortProjects() {
        for(auto& [type, projects]: Projects) {
            std::sort(projects.begin(), projects.end(), [](const Invent::Project& a, const Invent::Project& b) {
                return a.Name < b.Name;
            });
        }
    }
} // namespace Invent