#include "Walker/Ui/WalkerContent.h"
#include "Walker/Ui/Screens/WalkerScreens.h"

namespace Walker::WalkerUi::Content {
    bool Initialize() { return Screens::Initialize(); }
    void Render() { Screens::Render(); }
    void ShutDown() { Screens::ShutDown(); }
} // namespace Walker::WalkerUi::Content