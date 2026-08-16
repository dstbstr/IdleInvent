#pragma once
#include <algorithm>
#include <limits>
#include <map>
#include <queue>
#include <optional>
#include <vector>

/*
How to use:
*/

namespace World {
    namespace AStarPrivate {
        template<typename T>
        struct FrontierEntry {
            size_t Known = std::numeric_limits<size_t>::max();
            size_t Forecast = std::numeric_limits<size_t>::max();
            T Node{};

            constexpr FrontierEntry() {}
            constexpr explicit FrontierEntry(T t) : Node(t) {}

            constexpr bool operator<(const FrontierEntry& other) const {
                return other.Forecast != Forecast ? other.Forecast < Forecast : Known < other.Known;
            }
        };

        template<typename T>
        std::vector<T> ConstructPath(const std::map<T, T>& cameFrom, const T& start, const T& end) {
            std::vector<T> result{end};
            auto current = end;
            while(current != start) {
                auto next = cameFrom.at(current);
                result.push_back(next);
                current = next;
            }
            std::reverse(result.begin(), result.end());
            return result;
        }
    } // namespace AStarPrivate

    template<typename T>
    std::optional<std::vector<T>> AStar(
        T start, 
        const auto& costFunc, 
        const auto& doneFunc, 
        const auto& hFunc, 
        const auto& nFunc) {
        using State = AStarPrivate::FrontierEntry<T>;

        std::map<T, T> cameFrom{};
        std::map<T, size_t> knownCost{};
        std::priority_queue<State> queue{};

        auto startState = State(start);
        startState.Known = 0;
        startState.Forecast = hFunc(start);
        knownCost[start] = 0;
        cameFrom[start] = start;
        queue.push(startState);

        while(!queue.empty()) {
            auto current = queue.top(); queue.pop();
            auto node = current.Node;
            if(knownCost.at(node) != current.Known) continue; // stale

            if(doneFunc(node)) {
                return AStarPrivate::ConstructPath(cameFrom, start, node);
            }

            for(auto neighbor: nFunc(node)) {
                auto known = current.Known + costFunc(node, neighbor);
                auto previous = knownCost.find(neighbor);
                if(previous != knownCost.end() && previous->second <= known) continue;

                knownCost[neighbor] = known;
                auto next = State(neighbor);
                next.Known = known;
                next.Forecast = known + hFunc(neighbor);

                cameFrom[neighbor] = node;
                queue.push(next);
            }
        }

        return std::nullopt;
    }

    template<typename T>
    std::optional<std::vector<T>> AStar(T start, T end, auto nFunc) {
        auto costFunc = [](const T&, const T&) { return 1; };
        auto doneFunc = [&end](const T& pos) { return pos == end; };
        auto hFunc = [&end](const T& pos) { return static_cast<size_t>(MDistance(pos, end)); };

        return AStar<T>(start, costFunc, doneFunc, hFunc, nFunc);
    }
}