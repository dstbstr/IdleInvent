#include "Pets/Ui/Screens/RebirthScreen.h"
#include "Pets/Ui/Ui.h"

#include <Ui/UiUtil.h>

namespace Pets::Ui::Screens::Rebirth {
	bool Initialize() { return true; }

	void ShutDown() {}

	void Render() {
		TextCenteredX("Rebirth");
	}
} // namespace Pets::Ui::Screens::Rebirth
