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

	void ToastManager::AddToast(const std::string& toast, BaseTime duration) {
        m_PendingToasts.push(Toast{toast, {}, duration});
	}

	void ToastManager::AddToast(ToastImage toast, BaseTime duration) {
        m_PendingToasts.push(Toast{toast, {}, duration});
	}

	void ToastManager::Tick(BaseTime elapsed) {
        auto seconds = static_cast<f32>(elapsed.count()) / 1000.f;
        for(auto& slot: m_Slots) {
            if(slot.Active) {
                slot.Active->Lifetime -= elapsed;
                slot.Active->Position += m_Config.ToastVelocity * seconds;
				if(slot.Active->Lifetime <= ZeroTime) {
					slot.Active.reset();
				}
            }
        }
		StartPendingToasts();
	}

	void ToastManager::Render() const {
		auto* drawList = ImGui::GetForegroundDrawList();

		for(const auto& slot: m_Slots) {
			if(!slot.Active) continue;
			auto pos = slot.Active->Position;
            if(auto* image = std::get_if<ToastImage>(&slot.Active->Content)) {
                drawList->AddImage(
					image->Texture, 
					pos, 
					pos + image->Size, 
					image->UvMin, 
					image->UvMax);
			} else if(auto* str = std::get_if<std::string>(&slot.Active->Content)) {
                drawList->AddText(
					m_Config.ToastFont,
					m_Config.ToastFont->FontSize,
					pos, 
					IM_COL32_WHITE, 
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

			slot.Active->Position = slot.Position;
		}
	}
}