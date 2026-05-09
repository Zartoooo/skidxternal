#pragma once

#include "../../../ext/imgui/imgui.h"
#include "../IFeature.hpp"
#include "../../classes/Overlay.hpp"
#include "../../classes/ConfigSystem.hpp"
#include <vector>


class Menu : public IFeature
{
public:
	bool IsMouseInRect(ImVec2 min, ImVec2 max);
	bool IsMouseClicked(ImVec2 min, ImVec2 max, ImGuiMouseButton button = ImGuiMouseButton_Left);

	void OnInit() override;
	void OnShutdown() override;
	void OnRender() override;

	static void RegisterFonts();

	struct Fonts
	{
		inline static ImFont* Main;
	};
};