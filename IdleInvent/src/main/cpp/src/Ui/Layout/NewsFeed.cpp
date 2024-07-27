#include "Ui/Layout/NewsFeed.h"

#include "Ui/Ui.h"

#include "InventLib/Mechanics/Achievement.h"
#include "InventLib/Mechanics/RandomEvent.h"
#include "InventLib/Character/Death.h"
#include "InventLib/GameState/GameTime.h"
#include "InventLib/GameState/FileOperation.h"

#include "Core/DesignPatterns/PubSub.h"
#include "Core/DesignPatterns/ServiceLocator.h"

#include "imgui.h"

namespace {
    static constexpr size_t MaxNewsFeed = 20;

    std::vector<std::string> newsFeed{};

    void AddNews(const std::string& news) {
        newsFeed.push_back(news);
        if(newsFeed.size() > MaxNewsFeed) {
            std::rotate(newsFeed.begin(), newsFeed.begin() + 1, newsFeed.end());
            newsFeed.pop_back();
        }
    }
}

namespace Ui::NewsFeed {
    bool Initialize() {
        
        auto& services = ServiceLocator::Get();
        services.GetRequired<PubSub<Invent::RandomEvent>>().Subscribe([](const Invent::RandomEvent& event) {
            AddNews(event.Headline + " Has Started");
            AddNews(event.Description);
        });

        services.GetRequired<PubSub<Invent::Achievement>>().Subscribe([](const Invent::Achievement& achievement) {
            AddNews(achievement.Description);
        });

        services.GetRequired<PubSub<Invent::FileOperation>>().Subscribe([](const Invent::FileOperation& request) {
            switch (request) {
                case Invent::FileOperation::Save:
                    AddNews("Saving the game");
                    break;
                case Invent::FileOperation::Load:
                    AddNews("Loading the save file");
                    break;
                case Invent::FileOperation::Delete:
                    AddNews("Deleting the save file");
                    break;
            }
        });

        return true;
    }

    void ShutDown() {
        newsFeed.clear();
    }

    void Render() {
        ImGui::Begin("News Feed", nullptr, BaseFlags);
        for(auto it = newsFeed.rbegin(); it != newsFeed.rend(); ++it) {
            ImGui::TextWrapped("%s", it->c_str());
            if(it + 1 != newsFeed.rend()) {
                ImGui::Separator();
            }
        }
        ImGui::End();
    }
}