#include "InventLib/Character/Death.h"
#include "InventLib/RandomEvents/RandomEvent.h"
#include "InventLib/Ui/Layout/NewsFeed.h"
#include "InventLib/Ui/Ui.h"

#include <DesignPatterns/PubSub.h>
#include <DesignPatterns/ServiceLocator.h>
#include <GameState/GameTime.h>
#include <GameState/FileOperation.h>
#include <Mechanics/Achievement.h>

#include <imgui.h>

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
        services.GetRequired<PubSub<RandomEvent>>().Subscribe(GHandles, [](const RandomEvent& event) {
            AddNews(event.Headline + " Has Started");
            AddNews(event.Description);
        });

        services.GetRequired<PubSub<Achievement>>().Subscribe(GHandles, [](const Achievement& achievement) {
            AddNews(achievement.Description);
        });

        services.GetRequired<PubSub<FileOperation>>().Subscribe(GHandles, [](const FileOperation& request) {
            switch (request) {
                using enum FileOperation;
                case Save:
                    AddNews("Saving the game");
                    break;
                case Load:
                    AddNews("Loading the save file");
                    break;
                case Delete:
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