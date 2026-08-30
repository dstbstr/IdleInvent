#include "Ui/Combat/BattleFormation.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <span>

namespace {
    using namespace Ui::Formation;

    std::vector<Slot> BuildLines(std::span<const size_t> lineCounts, Ui::UiRect region, Front front) {
        auto maxCount = *std::max_element(lineCounts.begin(), lineCounts.end());
        if(maxCount == 0) return {};

        auto verticalFront = front == Front::Left || front == Front::Right;

        auto depth = lineCounts.size();
        auto rows = verticalFront ? maxCount : depth;
        auto cols = verticalFront ? depth : maxCount;


        auto regionSize = region.GetSize();
        auto fRows = static_cast<f32>(rows);
        auto fCols = static_cast<f32>(cols);
        auto slotSize = std::min(regionSize.x / fCols, regionSize.y / fRows);
        auto formationSize = ImVec2{slotSize * fCols, slotSize * fRows};
        auto origin = region.GetCenter() - (formationSize * 0.5f);

        switch(front) {
            using enum Front;
            case Left: origin.x = region.Min.x; break;
            case Right: origin.x = region.Max.x - formationSize.x; break;
            case Top: origin.y = region.Min.y; break;
            case Bottom: origin.y = region.Max.y - formationSize.y; break;
        }

        size_t totalSlots = std::accumulate(lineCounts.begin(), lineCounts.end(), static_cast<size_t>(0));
        std::vector<Slot> slots{};
        slots.reserve(totalSlots);

        size_t line = 0;
        for(auto entriesInLine : lineCounts) {
            for(size_t entry = 0; entry < entriesInLine; entry++) {
                ImVec2 center{};

                if(verticalFront) {
                    auto col = front == Front::Left ? line : cols - 1 - line;
                    auto vOff = static_cast<f32>(rows - entriesInLine) * slotSize * 0.5f;
                    center = ImVec2{
                        origin.x + (static_cast<f32>(col) + 0.5f) * slotSize,
                        origin.y + vOff + (static_cast<f32>(entry) + 0.5f) * slotSize
                    };
                } else {
                    auto row = front == Front::Bottom ? line : rows - 1 - line;
                    auto hOff = static_cast<f32>(cols - entriesInLine) * slotSize * 0.5f;
                    center = {
                        origin.x + hOff + (static_cast<f32>(entry) + 0.5f) * slotSize,
                        origin.y + (static_cast<f32>(row) + 0.5f) * slotSize
                    };
                }

                slots.push_back({.Center = center, .AvailableSize = slotSize});
            }

            line++;
        }

        return slots;
    }
}

namespace Ui::Formation {
    std::vector<Slot> Square(size_t count, UiRect region, Front front) {
        if(count == 0) return {};

        auto lineCapacity = static_cast<size_t>(std::ceil(std::sqrt(static_cast<f32>(count))));
        auto lineCount = (count + lineCapacity - 1) / lineCapacity;
        auto frontCount = count - (lineCount - 1) * lineCapacity;

        std::vector<size_t> lineCounts{};
        lineCounts.reserve(lineCount);
        lineCounts.push_back(frontCount);
        for(size_t line = 1; line < lineCount; line++) {
            lineCounts.push_back(lineCapacity);
        }

        return BuildLines(lineCounts, region, front);
    }

    std::vector<Slot> Triangle(size_t count, UiRect region, Front front) {
        if(count == 0) return {};
        std::vector<size_t> lineCounts{};
        size_t line = 1;
        while(count > 0) {
            auto entriesInLine = std::min(line, count);
            lineCounts.push_back(entriesInLine);
            count -= entriesInLine;
            line++;
        }
        return BuildLines(lineCounts, region, front);
    }

    std::vector<Slot> Lines(std::span<const size_t> lineCounts, UiRect region, Front front) {
        return BuildLines(lineCounts, region, front);
    }
}