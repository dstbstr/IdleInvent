#include "Walker/Ui/Screens/SettingsScreen.h"

namespace Walker::WalkerUi::Screens::Settings {
    bool Initialize() { return true; }
    void ShutDown() {}

    void Render() { ImGui::TextUnformatted("Settings"); }
}
