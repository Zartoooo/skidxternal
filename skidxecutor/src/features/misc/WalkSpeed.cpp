#include <Windows.h>
#include "WalkSpeed.hpp"
#include "../../classes/GameContext.hpp"
#include "../../sdk/sdk.hpp"

void WalkSpeed::WalkSpeedLoop()
{
	static int& walkSpeed = ConfigSys.Get<int>("misc.walkspeed");
	static int oldWalkSpeed = 16;
	while (m_bRunning)
	{
		

		if (oldWalkSpeed != walkSpeed && walkSpeed != 0)
		{
			PlrCache.LocalPlayer->Character()->FindFirstChildOfClass("Humanoid")->As<RBX::Humanoid>()->SetWalkSpeed(static_cast<float>(walkSpeed));	
		}

		oldWalkSpeed = walkSpeed;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

void WalkSpeed::OnInit()
{
	m_bRunning = true;	
	m_wsThread = std::thread(&WalkSpeed::WalkSpeedLoop, this);
}

void WalkSpeed::OnShutdown()
{
	m_bRunning = false;
	m_wsThread.join();
}

void WalkSpeed::OnRender()
{
}
