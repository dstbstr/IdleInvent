#include "Walker/Ui/Screens/TravelScreen.h"

namespace Walker::WalkerUi::Screens::Travel {
    bool Initialize() { return true; }
    void ShutDown() {}

    void Render() { ImGui::Text("Travel"); }
}
