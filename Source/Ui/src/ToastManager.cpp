#include "Ui/ToastManager.h"

#include <ranges>

namespace Ui {

	ToastManager::ToastManager(ToastManagerConfig config) : m_Config(config) {
        DR_ASSERT_MSG(!m_Config.ToastPositions.empty(), "Must have at least 1 toast position");
        if(m_Config.ToastPositions.empty()) {
            m_Config.ToastPositions.push_back({0, 0});
        }

		for(auto position : m_Config.ToastPositions) {
			m_Slots.push_back(ToastSlot{position});
		}
    }

	void ToastManager::AddToast(Toast toast) {
		m_PendingToasts.push(std::move(toast));
	}

	void ToastManager::AddToast(const std::string& toast, BaseTime duration) {
        m_PendingToasts.push(Toast{toast, {}, duration, duration});
	}

	void ToastManager::AddToast(ToastImage toast, BaseTime duration) {
        m_PendingToasts.push(Toast{toast, {}, duration, duration});
	}

	void ToastManager::Tick(BaseTime elapsed) {
        auto seconds = static_cast<f32>(elapsed.count()) / 1000.f;
        for(auto& slot: m_Slots) {
			if(!slot.Active) continue;
            auto& toast = *slot.Active;
            toast.Lifetime -= elapsed;
            toast.Position += m_Config.ToastVelocity * seconds;

			if(toast.Fade && toast.Duration > ZeroTime) {
				auto alpha = static_cast<f32>(toast.Lifetime.count()) / static_cast<f32>(toast.Duration.count());
				alpha = std::clamp(alpha, 0.f, 1.f);
                auto color = ImGui::ColorConvertU32ToFloat4(toast.Color);
                color.w = alpha;
                toast.Color = ImGui::ColorConvertFloat4ToU32(color);
			}

			if(toast.Lifetime <= ZeroTime) {
				slot.Active.reset();
			}
        }

		StartPendingToasts();
	}

	void ToastManager::Render() const {
		auto* drawList = ImGui::GetForegroundDrawList();

		for(const auto& slot: m_Slots) {
			if(!slot.Active) continue;
			auto& toast = *slot.Active;
            if(auto* image = std::get_if<ToastImage>(&toast.Content)) {
                drawList->AddImage(
					image->Texture, 
					toast.Position, 
					toast.Position + image->Size, 
					image->UvMin, 
					image->UvMax,
					toast.Color);
			} else if(auto* str = std::get_if<std::string>(&toast.Content)) {
                drawList->AddText(
					m_Config.ToastFont,
					m_Config.ToastFont->FontSize,
					toast.Position, 
					toast.Color, 
					str->c_str());
			}
		}
	}
	
	void ToastManager::StartPendingToasts() {
        for(auto& slot: m_Slots) {
            if(m_PendingToasts.empty()) return;
            if(slot.Active.has_value()) continue;
            slot.Active = std::move(m_PendingToasts.front());
            m_PendingToasts.pop();

			auto& toast = *slot.Active;
			if(toast.Lifetime == ZeroTime && toast.Duration == ZeroTime) {
				slot.Active.reset();
				return;
			}
			if(toast.Lifetime != toast.Duration) {
				toast.Lifetime = std::max(toast.Lifetime, toast.Duration);
				toast.Duration = toast.Lifetime;
			}
			slot.Active->Position = slot.Position;
		}
	}
}