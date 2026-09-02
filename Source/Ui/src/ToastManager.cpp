#include "Ui/ToastManager.h"

#include <ranges>

namespace Ui {

	void ToastManager::AddToast(const std::string& toast, BaseTime duration) {
		auto pos = GetNextPosition();
        m_Toasts.push_back(Toast{toast, pos, duration});
	}

	void ToastManager::AddToast(ToastImage toast, BaseTime duration) {
        auto pos = GetNextPosition();
        m_Toasts.push_back(Toast{toast, pos, duration});
	}

	void ToastManager::Tick(BaseTime elapsed) {
        auto seconds = static_cast<f32>(elapsed.count()) / 1000.f;
        for(auto& toast: m_Toasts) {
            toast.Lifetime -= elapsed;
			toast.Position += m_Config.ToastVelocity * seconds;
        }

		auto removed = std::ranges::remove_if(m_Toasts, [](const Toast& toast) { 
			return toast.Lifetime <= ZeroTime; 
		});
		m_Toasts.erase(removed.begin(), removed.end());
	}

	void ToastManager::Render() const {
        if(m_Toasts.empty()) return;
		auto* drawList = ImGui::GetForegroundDrawList();

		for(const auto& toast : m_Toasts) {
			auto pos = toast.Position;
            if(auto* image = std::get_if<ToastImage>(&toast.Content)) {
                drawList->AddImage(
					image->Texture, 
					pos, 
					pos + image->Size, 
					image->UvMin, 
					image->UvMax);
			} else if(auto* str = std::get_if<std::string>(&toast.Content)) {
                drawList->AddText(
					m_Config.ToastFont,
					m_Config.ToastFont->FontSize,
					pos, 
					IM_COL32_WHITE, 
					str->c_str());
			}
		}
	}
	
	ImVec2 ToastManager::GetNextPosition() {
        auto pos = m_Config.ToastPositions.at(m_PositionIndex);
        m_PositionIndex = (m_PositionIndex + 1) % m_Config.ToastPositions.size();
        return pos;
	}
	/*
		class ToastManager {
    public:
		ToastManager(ToastManagerConfig config) 
            : m_Config(config) {}

		void AddToast(const std::string& toast, BaseTime duration);
        void AddToast(ImTextureID image, BaseTime duration);

		void Tick(BaseTime elapsed);
		void Render() const;

	private:
        ToastManagerConfig m_Config{};
        std::vector<Toast> m_Toasts{};
		size_t m_PositionIndex{0};
	};
*/
}