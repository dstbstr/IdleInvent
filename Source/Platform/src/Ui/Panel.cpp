#include "Ui/Panel.h"

#include <imgui.h>

Panel::Panel(PanelPos pos, const std::pair<f32, f32>& parentDimensions) 
	: m_Dimensions(parentDimensions) {
    if(pos == PanelPos::FullScreen) return;
}