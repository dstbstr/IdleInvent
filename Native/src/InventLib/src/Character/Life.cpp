#include "InventLib/Character/Life.h"

#include <algorithm>

#include "InventLib/Character/Society.h"
#include "InventLib/Mechanics/Effect.h"
#include "InventLib/Projects/Population.h"

namespace {
    void TickResources(Invent::BaseTime elapsed, Invent::Life& life) {
        for(auto& [name, resource]: life.Resources) {
            if(name == Invent::ResourceName::Primary) continue; // Primary is handled by Work
            if(resource.Current >= resource.Capacity) continue; // Allow overfilling in case capacity decreases
            auto progress = life.ResourceProgressions.at(name).GetProgress(elapsed);
            resource.Current += progress;
            resource.Clamp();
        }
    }

    void AddProject(Invent::Life& life, Invent::Project&& project) {
        project.TimeCost = Invent::BaseTime(life.ProjectTimeCostModifiers.at(project.Type).Apply(project.TimeCost.count()));
        for(auto& [name, resource]: project.ResourceCost) {
			resource.Current = life.ProjectResourceCostModifiers.at(project.Type).Apply(resource.Current);
		}

        life.Projects.at(project.Type).push_back(project);
    }

    void CompleteInvention(
        const Invent::Project& project,
        Invent::Life& life,
        const Invent::Society& society,
        std::vector<Invent::Project>& outNewProjects
    ) {
        auto invention = Invent::InventionFromString(project.Name);
        if(!invention.has_value()) {
            DR_ASSERT_MSG(false, std::format("Invention {} not found", project.Name));
            return;
        }

        auto v = invention.value();
        life.Inventions.push_back(v);
        auto next = Invent::GetResearchProject(society.Specialty, life.Inventions.size());
        if(next.has_value()) {
            outNewProjects.push_back(next.value());
            outNewProjects.back().Active = project.Active;
        } else {
            life.AvailableWorkers += project.Active;
        }
        for(auto b: Invent::GetBuildings({v})) {
            life.Buildings[b] = 0;
            outNewProjects.push_back(Invent::GetBuildingProject(b, 0));
        }
    }

    void
    CompleteBuilding(const Invent::Project& project, Invent::Life& life, std::vector<Invent::Project>& outNewProjects) {
        auto building = Invent::BuildingFromString(project.Name);
        if(!building.has_value()) {
            DR_ASSERT_MSG(false, std::format("Building {} not found", project.Name));
            return;
        }
        auto b = building.value();

        life.Buildings.at(b)++;
        auto effects = Invent::GetEffectsForBuilding(b);
        Log::Info(std::format("Finished building {}, now at level {}", ToString(b), life.Buildings.at(b)));
        for(const auto& effect: effects) {
            life.ApplyEffect(effect);
        }

        outNewProjects.push_back(Invent::GetBuildingProject(b, life.Buildings.at(b)));
        outNewProjects.back().Active = project.Active;
    }

    void CompletePopulation(
        const Invent::Project& project, Invent::Life& life, std::vector<Invent::Project>& outNewProjects
    ) {
        if(project.Name == Invent::Population::PopulationName) {
            life.CurrentPopulation++;
            if(life.CurrentPopulation < life.MaxPopulation) {
                outNewProjects.push_back(Invent::GetPopulationIncreaseProject(life.CurrentPopulation));
                outNewProjects.back().Active = project.Active;
            } else {
                life.AvailableWorkers += project.Active;
            }
        } else {
            if(life.CurrentPopulation == life.MaxPopulation) {
                outNewProjects.push_back(Invent::GetPopulationIncreaseProject(life.CurrentPopulation));
            }
            life.MaxPopulation *= 10;
            outNewProjects.push_back(Invent::GetPopulationCapacityProject(life.MaxPopulation));
            outNewProjects.back().Active = project.Active;
        }
    }

    void CompleteExploration(
        const Invent::Project& project, Invent::Life& life, std::vector<Invent::Project>& outNewProjects
    ) {
        (void)project;
        (void)life;
        (void)outNewProjects;
        DR_ASSERT_MSG(false, "Exploration not implemented");
    }

    void CompleteProject(
        const Invent::Project& project,
        Invent::Life& life,
        Invent::Society& society,
        std::vector<Invent::Project>& outNewProjects
    ) {
        switch(project.Type) {
        case Invent::ProjectType::Research: CompleteInvention(project, life, society, outNewProjects); break;
        case Invent::ProjectType::Build: CompleteBuilding(project, life, outNewProjects); break;
        case Invent::ProjectType::Population: CompletePopulation(project, life, outNewProjects); break;
        case Invent::ProjectType::Explore: CompleteExploration(project, life, outNewProjects); break;
        default: DR_ASSERT_MSG(false, "Unsupported project completed"); break;
        }
    }

    void TickProjects(Invent::BaseTime elapsed, Invent::Life& life, Invent::Society& society) {
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
        }
    }
} // namespace

namespace Invent {
    Life::Life(Society* society) : m_Society(society) {
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

        AddProject(*this, GetResearchProject(society->Specialty, 0).value());
        AddProject(*this, GetPopulationCapacityProject(MaxPopulation));
        AddProject(*this, GetPopulationIncreaseProject(CurrentPopulation));

        SortProjects();
        auto GetCost = [&](ResourceName name) -> size_t {
            return society->Specialty == name ? 5 : society->Weakness == name ? 15 : 10;
		};
        auto MakeConverter = [&](ResourceName resource, const std::string name) -> ResourceConversion {
			return ResourceConversionBuilder(name)
				.WithCost(ResourceName::Primary, GetCost(resource))
				.WithProduct(resource, 5)
				.Build();
		};
        ResourceConverters.push_back(MakeConverter(ResourceName::Followers, "Recruit"));
        ResourceConverters.push_back(MakeConverter(ResourceName::Knowledge, "Study"));
        ResourceConverters.push_back(MakeConverter(ResourceName::Money, "Earn"));
        ResourceConverters.push_back(MakeConverter(ResourceName::Power, "Train"));
    }

    void Life::Tick(BaseTime elapsed) {
        Work(elapsed);
        TickResources(elapsed, *this);
        TickProjects(elapsed, *this, *this->m_Society);
    }

    void Life::Work(BaseTime elapsed) {
        auto& primary = Resources.at(ResourceName::Primary);
        if(primary.Current >= primary.Capacity) return;

        auto progress = ResourceProgressions.at(ResourceName::Primary).GetProgress(elapsed);
        progress *= CurrentPopulation;
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
        case EffectTarget::ProjectCount: {
            auto working = MaxProjects - AvailableWorkers;
            MaxProjects = mod.Apply(MaxProjects);
            AvailableWorkers = MaxProjects - working;
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
        case EffectTarget::ExploreSuccessRate: break;
        }
    }

    void Life::SortProjects() {
        for(auto& [type, projects]: Projects) {
            std::sort(projects.begin(), projects.end(), [](const Invent::Project& a, const Invent::Project& b) {
                return a.Name < b.Name;
            });
        }
    }
} // namespace Invent