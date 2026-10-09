#include "Walker/Ui/WalkerLayout.h"
#include "Walker/Ui/WalkerContent.h"
#include "Walker/Ui/WalkerHeader.h"
#include "Walker/Ui/WalkerNav.h"
#include "Walker/Home/HomeBase.h"

#include <DesignPatterns/ServiceLocator.h>
#include <Instrumentation/Logging.h>
#include <Platform/Graphics.h>
#include <Utilities/Handle.h>
#include <Ui/ToastManager.h>
#include <Ui/UiBuilder.h>
#include <Ui/UiUtil.h>

namespace {
    std::optional<Ui::ToastManager> Toasts;
    std::vector<ScopedHandle> ToastSubs;
}

namespace Walker::WalkerUi::Layout {
    bool Initialize() { 
        InitializeFonts("DroidSans.ttf");

        bool success = true;
        success &= Content::Initialize();
        success &= Header::Initialize();
        success &= Nav::Initialize();
        DR_ASSERT(success);

        Toasts.emplace(Ui::ToastManagerConfig{
            .ToastPositions = {{20.f, 100.f}},
            .ToastVelocity = {},
            .ToastFont = GetFont(FontSizes::H2)
        });
		auto& home = ServiceLocator::Get().GetRequired<HomeBase>();
        home.Milestones.Subscribe(ToastSubs, [](const MilestoneUnlocked& unlocked) {
			const auto& presentation = GetMilestonePresentation(unlocked.Kind);
			auto text = std::format("Milestone Unlocked: {}", presentation.Name);

            Toasts->AddToast(text, OneSecond * 4);
        });
        return success;
    }

    void Render() {
        auto width = Graphics::ScreenWidth;
        auto topHeight = Header::GetRequestedHeight(width);
        auto navHeight = Nav::GetRequestedHeight(width);
        auto mainHeight = std::max(0.f, Graphics::ScreenHeight - topHeight - navHeight);
        UiBuilder()
            .AddPart(topHeight, Header::Render)
            .AddPart(mainHeight, Content::Render)
            .AddPart(navHeight, Nav::Render)
            .Build();

        if(Toasts) {
            ImGui::PushFont(GetFont(FontSizes::H2));
            auto toastHeight = ImGui::GetTextLineHeight();
            ImGui::PopFont();

			auto navTop = Graphics::ScreenHeight - navHeight;
			Toasts->SetSlotPosition(0, ImVec2{ 20.f, navTop - toastHeight - 20.f });
            Toasts->Render();
        }
    }
    
    void Tick(BaseTime elapsed) {
        if(Toasts && elapsed > ZeroTime) {
            Toasts->Tick(elapsed);
        }
    }

    void ShutDown() {
        ToastSubs.clear();
        Toasts.reset();

        Nav::ShutDown();
        Header::ShutDown();
        Content::ShutDown();
    }
} // namespace Walker::WalkerUi::Layout