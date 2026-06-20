#include "Pets/Ui/Screens/CombatScreen.h"
#include "Pets/Ui/Ui.h"

#include <Ui/UiUtil.h>

namespace Pets::Ui::Screens::Combat {
	bool Initialize() { return true; }

	void ShutDown() {}

	void Render() {
		TextCenteredX("Combat");
	}
} // namespace Pets::Ui::Screens::Combat
