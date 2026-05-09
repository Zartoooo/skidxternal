#pragma once
#include <thread>
#include <atomic>
#include <chrono>

#include "../IFeature.hpp"
#include "../../classes/PlayerCache.hpp"
#include "../../classes/ConfigSystem.hpp"	

class WalkSpeed : public IFeature
{
public:
	void OnInit() override;
	void OnShutdown() override;
	void OnRender() override;

private:
	std::thread m_wsThread;
	std::atomic<bool> m_bRunning;

	void WalkSpeedLoop();
};