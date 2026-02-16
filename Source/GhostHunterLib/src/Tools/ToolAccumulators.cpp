#include "GhostHunter/GameState/Life.h"
#include "GhostHunter/Tools/ToolAccumulators.h"

#include <unordered_map>

#define ADD(_Tool)                                    \
    namespace _Tool##_Registry {                      \
        auto ToolName = GhostHunter::ToolName::_Tool; \
        void Add##_Tool();                            \
        struct _AutoAdd {                             \
            _AutoAdd() { Add##_Tool(); }              \
        } autoAdd{};                                  \
    }                                                 \
    void _Tool##_Registry::Add##_Tool()

#define Q(_Level, _Time, _Currency, _Amt)                                                             \
    GetToolMap()[ToolName][GhostHunter::QualityType::_Level].push_back(                               \
        {_Time, [](void* room) {                                                                      \
             if(!room) return;                                                                        \
             auto* r = static_cast<GhostHunter::Room*>(room);                                         \
             auto avail = r->AvailableResources[GhostHunter::ResourceName::_Currency].Current;   \
             auto gained = std::min(avail, static_cast<s64>(_Amt));                                   \
             r->AvailableResources[GhostHunter::ResourceName::_Currency].Current -= gained;           \
             auto& inventory = ServiceLocator::Get().GetRequired<GhostHunter::Life>().GetInventory(); \
             inventory.Resources[GhostHunter::ResourceName::_Currency].Current += gained;             \
         }}                                                                                           \
    );

using QualityMap = std::unordered_map<GhostHunter::QualityType, std::vector<Accumulator>>;
using ToolMap = std::unordered_map<GhostHunter::ToolName, QualityMap>;

namespace {
    ToolMap& GetToolMap() {
        static ToolMap result{};
        return result;
    }

    ADD(Flashlight) {
        Q(Awful, OneSecond * 1, Images, 1);
        Q(Awful, OneSecond * 2, Physical, 1);
        Q(Awful, OneSecond * 3, Audio, 1);
        Q(Awful, OneSecond * 4, Video, 1);

        Q(Bad, OneSecond * 1, Images, 2);
        Q(Bad, OneSecond * 2, Physical, 2);
        Q(Bad, OneSecond * 3, Audio, 2);
        Q(Bad, OneSecond * 4, Video, 2);

        Q(Okay, OneSecond * 1, Images, 3);
        Q(Okay, OneSecond * 2, Physical, 3);
        Q(Okay, OneSecond * 3, Audio, 3);
        Q(Okay, OneSecond * 4, Video, 3);

        Q(PrettyGood, OneSecond * 1, Images, 4);
        Q(PrettyGood, OneSecond * 2, Physical, 4);
        Q(PrettyGood, OneSecond * 3, Audio, 4);
        Q(PrettyGood, OneSecond * 4, Video, 4);

        Q(Great, OneSecond * 1, Images, 5);
        Q(Great, OneSecond * 2, Physical, 5);
        Q(Great, OneSecond * 3, Audio, 5);
        Q(Great, OneSecond * 4, Video, 5);

        Q(Excellent, OneSecond * 1, Images, 6);
        Q(Excellent, OneSecond * 2, Physical, 6);
        Q(Excellent, OneSecond * 3, Audio, 6);
        Q(Excellent, OneSecond * 4, Video, 6);

        Q(Wonderful, OneSecond * 1, Images, 7);
        Q(Wonderful, OneSecond * 2, Physical, 7);
        Q(Wonderful, OneSecond * 3, Audio, 7);
        Q(Wonderful, OneSecond * 4, Video, 7);

        Q(Legendary, OneSecond * 1, Images, 8);
        Q(Legendary, OneSecond * 2, Physical, 8);
        Q(Legendary, OneSecond * 3, Audio, 8);
        Q(Legendary, OneSecond * 4, Video, 8);
    }
    ADD(Camera) {
        Q(Awful, OneSecond * 1, Images, 1);
        Q(Awful, OneSecond * 2, Physical, 1);
        Q(Awful, OneSecond * 3, Audio, 1);
        Q(Awful, OneSecond * 4, Video, 1);

        Q(Bad, OneSecond * 1, Images, 2);
        Q(Bad, OneSecond * 2, Physical, 2);
        Q(Bad, OneSecond * 3, Audio, 2);
        Q(Bad, OneSecond * 4, Video, 2);

        Q(Okay, OneSecond * 1, Images, 3);
        Q(Okay, OneSecond * 2, Physical, 3);
        Q(Okay, OneSecond * 3, Audio, 3);
        Q(Okay, OneSecond * 4, Video, 3);

        Q(PrettyGood, OneSecond * 1, Images, 4);
        Q(PrettyGood, OneSecond * 2, Physical, 4);
        Q(PrettyGood, OneSecond * 3, Audio, 4);
        Q(PrettyGood, OneSecond * 4, Video, 4);

        Q(Great, OneSecond * 1, Images, 5);
        Q(Great, OneSecond * 2, Physical, 5);
        Q(Great, OneSecond * 3, Audio, 5);
        Q(Great, OneSecond * 4, Video, 5);

        Q(Excellent, OneSecond * 1, Images, 6);
        Q(Excellent, OneSecond * 2, Physical, 6);
        Q(Excellent, OneSecond * 3, Audio, 6);
        Q(Excellent, OneSecond * 4, Video, 6);

        Q(Wonderful, OneSecond * 1, Images, 7);
        Q(Wonderful, OneSecond * 2, Physical, 7);
        Q(Wonderful, OneSecond * 3, Audio, 7);
        Q(Wonderful, OneSecond * 4, Video, 7);

        Q(Legendary, OneSecond * 1, Images, 8);
        Q(Legendary, OneSecond * 2, Physical, 8);
        Q(Legendary, OneSecond * 3, Audio, 8);
        Q(Legendary, OneSecond * 4, Video, 8);
    }

    ADD(EmfDetector) {
        Q(Awful, OneSecond * 1, Images, 1);
        Q(Awful, OneSecond * 2, Physical, 1);
        Q(Awful, OneSecond * 3, Audio, 1);
        Q(Awful, OneSecond * 4, Video, 1);

        Q(Bad, OneSecond * 1, Images, 2);
        Q(Bad, OneSecond * 2, Physical, 2);
        Q(Bad, OneSecond * 3, Audio, 2);
        Q(Bad, OneSecond * 4, Video, 2);

        Q(Okay, OneSecond * 1, Images, 3);
        Q(Okay, OneSecond * 2, Physical, 3);
        Q(Okay, OneSecond * 3, Audio, 3);
        Q(Okay, OneSecond * 4, Video, 3);

        Q(PrettyGood, OneSecond * 1, Images, 4);
        Q(PrettyGood, OneSecond * 2, Physical, 4);
        Q(PrettyGood, OneSecond * 3, Audio, 4);
        Q(PrettyGood, OneSecond * 4, Video, 4);

        Q(Great, OneSecond * 1, Images, 5);
        Q(Great, OneSecond * 2, Physical, 5);
        Q(Great, OneSecond * 3, Audio, 5);
        Q(Great, OneSecond * 4, Video, 5);

        Q(Excellent, OneSecond * 1, Images, 6);
        Q(Excellent, OneSecond * 2, Physical, 6);
        Q(Excellent, OneSecond * 3, Audio, 6);
        Q(Excellent, OneSecond * 4, Video, 6);

        Q(Wonderful, OneSecond * 1, Images, 7);
        Q(Wonderful, OneSecond * 2, Physical, 7);
        Q(Wonderful, OneSecond * 3, Audio, 7);
        Q(Wonderful, OneSecond * 4, Video, 7);

        Q(Legendary, OneSecond * 1, Images, 8);
        Q(Legendary, OneSecond * 2, Physical, 8);
        Q(Legendary, OneSecond * 3, Audio, 8);
        Q(Legendary, OneSecond * 4, Video, 8);
    }
    ADD(ThermalCamera) {
        Q(Awful, OneSecond * 1, Images, 1);
        Q(Awful, OneSecond * 2, Physical, 1);
        Q(Awful, OneSecond * 3, Audio, 1);
        Q(Awful, OneSecond * 4, Video, 1);

        Q(Bad, OneSecond * 1, Images, 2);
        Q(Bad, OneSecond * 2, Physical, 2);
        Q(Bad, OneSecond * 3, Audio, 2);
        Q(Bad, OneSecond * 4, Video, 2);

        Q(Okay, OneSecond * 1, Images, 3);
        Q(Okay, OneSecond * 2, Physical, 3);
        Q(Okay, OneSecond * 3, Audio, 3);
        Q(Okay, OneSecond * 4, Video, 3);

        Q(PrettyGood, OneSecond * 1, Images, 4);
        Q(PrettyGood, OneSecond * 2, Physical, 4);
        Q(PrettyGood, OneSecond * 3, Audio, 4);
        Q(PrettyGood, OneSecond * 4, Video, 4);

        Q(Great, OneSecond * 1, Images, 5);
        Q(Great, OneSecond * 2, Physical, 5);
        Q(Great, OneSecond * 3, Audio, 5);
        Q(Great, OneSecond * 4, Video, 5);

        Q(Excellent, OneSecond * 1, Images, 6);
        Q(Excellent, OneSecond * 2, Physical, 6);
        Q(Excellent, OneSecond * 3, Audio, 6);
        Q(Excellent, OneSecond * 4, Video, 6);

        Q(Wonderful, OneSecond * 1, Images, 7);
        Q(Wonderful, OneSecond * 2, Physical, 7);
        Q(Wonderful, OneSecond * 3, Audio, 7);
        Q(Wonderful, OneSecond * 4, Video, 7);

        Q(Legendary, OneSecond * 1, Images, 8);
        Q(Legendary, OneSecond * 2, Physical, 8);
        Q(Legendary, OneSecond * 3, Audio, 8);
        Q(Legendary, OneSecond * 4, Video, 8);
    }
    ADD(EvpRecorder) {
        Q(Awful, OneSecond * 1, Images, 1);
        Q(Awful, OneSecond * 2, Physical, 1);
        Q(Awful, OneSecond * 3, Audio, 1);
        Q(Awful, OneSecond * 4, Video, 1);

        Q(Bad, OneSecond * 1, Images, 2);
        Q(Bad, OneSecond * 2, Physical, 2);
        Q(Bad, OneSecond * 3, Audio, 2);
        Q(Bad, OneSecond * 4, Video, 2);

        Q(Okay, OneSecond * 1, Images, 3);
        Q(Okay, OneSecond * 2, Physical, 3);
        Q(Okay, OneSecond * 3, Audio, 3);
        Q(Okay, OneSecond * 4, Video, 3);

        Q(PrettyGood, OneSecond * 1, Images, 4);
        Q(PrettyGood, OneSecond * 2, Physical, 4);
        Q(PrettyGood, OneSecond * 3, Audio, 4);
        Q(PrettyGood, OneSecond * 4, Video, 4);

        Q(Great, OneSecond * 1, Images, 5);
        Q(Great, OneSecond * 2, Physical, 5);
        Q(Great, OneSecond * 3, Audio, 5);
        Q(Great, OneSecond * 4, Video, 5);

        Q(Excellent, OneSecond * 1, Images, 6);
        Q(Excellent, OneSecond * 2, Physical, 6);
        Q(Excellent, OneSecond * 3, Audio, 6);
        Q(Excellent, OneSecond * 4, Video, 6);

        Q(Wonderful, OneSecond * 1, Images, 7);
        Q(Wonderful, OneSecond * 2, Physical, 7);
        Q(Wonderful, OneSecond * 3, Audio, 7);
        Q(Wonderful, OneSecond * 4, Video, 7);

        Q(Legendary, OneSecond * 1, Images, 8);
        Q(Legendary, OneSecond * 2, Physical, 8);
        Q(Legendary, OneSecond * 3, Audio, 8);
        Q(Legendary, OneSecond * 4, Video, 8);
    }
    ADD(LaserGrid) {
        Q(Awful, OneSecond * 1, Images, 1);
        Q(Awful, OneSecond * 2, Physical, 1);
        Q(Awful, OneSecond * 3, Audio, 1);
        Q(Awful, OneSecond * 4, Video, 1);

        Q(Bad, OneSecond * 1, Images, 2);
        Q(Bad, OneSecond * 2, Physical, 2);
        Q(Bad, OneSecond * 3, Audio, 2);
        Q(Bad, OneSecond * 4, Video, 2);

        Q(Okay, OneSecond * 1, Images, 3);
        Q(Okay, OneSecond * 2, Physical, 3);
        Q(Okay, OneSecond * 3, Audio, 3);
        Q(Okay, OneSecond * 4, Video, 3);

        Q(PrettyGood, OneSecond * 1, Images, 4);
        Q(PrettyGood, OneSecond * 2, Physical, 4);
        Q(PrettyGood, OneSecond * 3, Audio, 4);
        Q(PrettyGood, OneSecond * 4, Video, 4);

        Q(Great, OneSecond * 1, Images, 5);
        Q(Great, OneSecond * 2, Physical, 5);
        Q(Great, OneSecond * 3, Audio, 5);
        Q(Great, OneSecond * 4, Video, 5);

        Q(Excellent, OneSecond * 1, Images, 6);
        Q(Excellent, OneSecond * 2, Physical, 6);
        Q(Excellent, OneSecond * 3, Audio, 6);
        Q(Excellent, OneSecond * 4, Video, 6);

        Q(Wonderful, OneSecond * 1, Images, 7);
        Q(Wonderful, OneSecond * 2, Physical, 7);
        Q(Wonderful, OneSecond * 3, Audio, 7);
        Q(Wonderful, OneSecond * 4, Video, 7);

        Q(Legendary, OneSecond * 1, Images, 8);
        Q(Legendary, OneSecond * 2, Physical, 8);
        Q(Legendary, OneSecond * 3, Audio, 8);
        Q(Legendary, OneSecond * 4, Video, 8);
    }
    ADD(SpiritBox) {
        Q(Awful, OneSecond * 1, Images, 1);
        Q(Awful, OneSecond * 2, Physical, 1);
        Q(Awful, OneSecond * 3, Audio, 1);
        Q(Awful, OneSecond * 4, Video, 1);

        Q(Bad, OneSecond * 1, Images, 2);
        Q(Bad, OneSecond * 2, Physical, 2);
        Q(Bad, OneSecond * 3, Audio, 2);
        Q(Bad, OneSecond * 4, Video, 2);

        Q(Okay, OneSecond * 1, Images, 3);
        Q(Okay, OneSecond * 2, Physical, 3);
        Q(Okay, OneSecond * 3, Audio, 3);
        Q(Okay, OneSecond * 4, Video, 3);

        Q(PrettyGood, OneSecond * 1, Images, 4);
        Q(PrettyGood, OneSecond * 2, Physical, 4);
        Q(PrettyGood, OneSecond * 3, Audio, 4);
        Q(PrettyGood, OneSecond * 4, Video, 4);

        Q(Great, OneSecond * 1, Images, 5);
        Q(Great, OneSecond * 2, Physical, 5);
        Q(Great, OneSecond * 3, Audio, 5);
        Q(Great, OneSecond * 4, Video, 5);

        Q(Excellent, OneSecond * 1, Images, 6);
        Q(Excellent, OneSecond * 2, Physical, 6);
        Q(Excellent, OneSecond * 3, Audio, 6);
        Q(Excellent, OneSecond * 4, Video, 6);

        Q(Wonderful, OneSecond * 1, Images, 7);
        Q(Wonderful, OneSecond * 2, Physical, 7);
        Q(Wonderful, OneSecond * 3, Audio, 7);
        Q(Wonderful, OneSecond * 4, Video, 7);

        Q(Legendary, OneSecond * 1, Images, 8);
        Q(Legendary, OneSecond * 2, Physical, 8);
        Q(Legendary, OneSecond * 3, Audio, 8);
        Q(Legendary, OneSecond * 4, Video, 8);
    }
} // namespace

#undef Q
#undef ADD

namespace GhostHunter {
    std::vector<Accumulator> GetAccumulators(ToolName tool, QualityType level) { return GetToolMap()[tool][level]; }
} // namespace GhostHunter
