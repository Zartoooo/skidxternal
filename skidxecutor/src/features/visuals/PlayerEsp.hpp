#pragma once
#include "../../classes/PlayerCache.hpp"
#include "../../../ext/imgui/imgui.h"
#include "../../util/math.hpp"
#include "../../util/util.hpp"
#include "../IFeature.hpp"

class PlayerEsp : public IFeature
{
public:
	void OnInit() override;
	void OnShutdown() override;
	void OnRender() override;
};