#include "Pets/Ui/Screens/BestiaryScreen.h"
#include "Pets/Ui/Ui.h"

#include <Ui/UiUtil.h>

namespace Pets::Ui::Screens::Bestiary {
	bool Initialize() { return true; }

	void ShutDown() {}

	void Render() {
		TextCenteredX("Bestiary");
	}
} // namespace Pets::Ui::Screens::Bestiary
