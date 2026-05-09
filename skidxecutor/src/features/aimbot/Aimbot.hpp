#pragma once
#include <thread>
#include <atomic>
#include <chrono>

#include "../../../ext/imgui/imgui.h"
#include "../IFeature.hpp"
#include "../../classes/PlayerCache.hpp"
#include "../../util/math.hpp"
#include "../../classes/Overlay.hpp"

class Aimbot : public IFeature
{
public:
	void OnInit() override;
	void OnShutdown() override;
	void OnRender() override;

private:
	std::atomic<bool> m_bRunning = false;
	std::thread m_aimThread;
	void AimbotLoop();
};