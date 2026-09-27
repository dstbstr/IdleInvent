#include "Walker/Home/HomeBase.h"

namespace Walker {
	void HomeBase::Tick(BaseTime elapsed) {
		Crew.Tick(elapsed);
	}
}