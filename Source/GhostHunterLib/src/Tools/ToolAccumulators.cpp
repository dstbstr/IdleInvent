#include "GhostHunter/Tools/ToolAccumulators.h"

#include "GhostHunter/GameState/Life.h"
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

#define Q(_Level, _Time, _Currency, _Amt)                               \
    GetToolMap()[ToolName][GhostHunter::QualityType::_Level].push_back( \
        {_Time, [] {                                                    \
             ServiceLocator::Get()                                      \
                 .GetRequired<GhostHunter::Life>()                      \
                 .GetInventory()                                        \
                 .Resources[GhostHunter::ResourceName::_Currency]       \
                 .Current += _Amt;                                      \
         }}                                                             \
    );

using QualityMap = std::unordered_map<GhostHunter::QualityType, std::vector<Accumulator>>;
using ToolMap = std::unordered_map<GhostHunter::ToolName, QualityMap>;

namespace {
	ToolMap& GetToolMap() {
        static ToolMap result{};
        return result;
	}

	ADD(Flashlight) {
		Q(Awful, OneSecond * 3, Images, 1);
        Q(Awful, OneSecond * 2, Physical, 1);

		Q(Bad, OneSecond * 2, Images, 2);
        Q(Bad, OneSecond * 3, Physical, 3);
	}
	ADD(Camera) {
		Q(Bad, OneMinute, Images, 1)
	}

}

#undef Q
#undef ADD

namespace GhostHunter {
	std::vector<Accumulator> GetAccumulators(ToolName tool, QualityType level) {
        return GetToolMap()[tool][level];
	}
}

