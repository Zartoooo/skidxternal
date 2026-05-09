#include "util.hpp"
#include "../classes/Overlay.hpp"

void CUtils::CenterText(const std::string& str, ImVec2 pos, ImU32 color, TEXT_ALIGN align)
{
	ImVec2 size = ImGui::CalcTextSize(str.c_str());
	const float halfW = size.x * 0.5f;
	const float halfH = size.y * 0.5f;
	ImVec2 textPos = pos;

	switch (align)
	{
	case TEXT_ALIGN::TOP_LEFT:                                                   break;
	case TEXT_ALIGN::TOP_CENTER:    textPos.x -= halfW;                          break;
	case TEXT_ALIGN::TOP_RIGHT:     textPos.x -= size.x;                         break;
	case TEXT_ALIGN::CENTER_LEFT:   textPos.y -= halfH;                          break;
	case TEXT_ALIGN::CENTER_CENTER: textPos.x -= halfW;  textPos.y -= halfH;     break;
	case TEXT_ALIGN::CENTER_RIGHT:  textPos.x -= size.x; textPos.y -= halfH;     break;
	case TEXT_ALIGN::BOTTOM_LEFT:   textPos.y -= size.y;                         break;
	case TEXT_ALIGN::BOTTOM_CENTER: textPos.x -= halfW;  textPos.y -= size.y;    break;
	case TEXT_ALIGN::BOTTOM_RIGHT:  textPos.x -= size.x; textPos.y -= size.y;    break;
	}

	if (Overlay.BackgroundDrawList)
		Overlay.BackgroundDrawList->AddText(textPos, color, str.c_str());
}
