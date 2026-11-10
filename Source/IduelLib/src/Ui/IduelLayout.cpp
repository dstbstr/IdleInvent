#include "Iduel/Ui/IduelLayout.h"
#include "Iduel/Ui/Screens/IduelScreens.h"

#include <DesignPatterns/ServiceLocator.h>
#include <Instrumentation/Logging.h>
#include <Platform/Graphics.h>
#include <Utilities/Handle.h>
#include <Ui/ToastManager.h>
#include <Ui/UiBuilder.h>
#include <Ui/UiUtil.h>
#include <Ui/ScreenManager.h>

namespace {
    std::optional<Ui::ToastManager> Toasts;
    std::vector<ScopedHandle> ToastSubs;
    Ui::ScreenManager<Iduel::IduelUi::Screen>* Manager = nullptr;
}

namespace Iduel::IduelUi::Layout {
    bool Initialize() {
        InitializeFonts("DroidSans.ttf");

		Manager = &GetScreenManager();
		DR_ASSERT(Manager);
        bool success = Manager->Initialize();
        DR_ASSERT(success);

        Toasts.emplace(Ui::ToastManagerConfig{
            .ToastPositions = { { 20.f, 100.f } },
            .ToastVelocity = {},
            .ToastFont = GetFont(FontSizes::H2)
            });
        return success;
    }

    void Render() {
        auto width = Graphics::ScreenWidth;
        UiBuilder()
            .AddPart(Graphics::ScreenHeight, [] { Manager->Render(); })
            .Build();

        if (Toasts) {
            ImGui::PushFont(GetFont(FontSizes::H2));
            auto toastHeight = ImGui::GetTextLineHeight();
            ImGui::PopFont();

            Toasts->SetSlotPosition(0, ImVec2{ 20.f, toastHeight - 20.f });
            Toasts->Render();
        }
    }

    void Tick(BaseTime elapsed) {
        if (Toasts && elapsed > ZeroTime) {
            Toasts->Tick(elapsed);
        }
    }

    void ShutDown() {
        ToastSubs.clear();
        Toasts.reset();
        Manager->ShutDown();
    }
} // namespace Iduel::IduelUi::Layout