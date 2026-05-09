#include <Windows.h>
#include "Aimbot.hpp"
#include "../../classes/ConfigSystem.hpp"
#include "../../classes/GameContext.hpp"
#include "../../sdk/sdk.hpp"
#include "../../util/util.hpp"

void Aimbot::OnInit()
{
	m_bRunning = true;
	this->m_aimThread = std::thread(&Aimbot::AimbotLoop, this);
}

void Aimbot::OnShutdown()
{
	m_bRunning = false;
	if (this->m_aimThread.joinable())
		this->m_aimThread.join();
}

void Aimbot::OnRender()
{
	if (!Game.isInGame)
		return;
	 
	static int& aimbotFov = ConfigSys.Get<int>("aimbot.fov");
	static bool& aimbotDrawFov = ConfigSys.Get<bool>("aimbot.drawfov");
	
	if (!aimbotDrawFov)
		return;

	Vec2 mouse_pos = Game.Mouse->GetPos();
	Overlay.BackgroundDrawList->AddCircle({ mouse_pos.x, mouse_pos.y }, (float)aimbotFov, IM_COL32(0, 0, 0, 255), 64, 3.f);
	Overlay.BackgroundDrawList->AddCircle({ mouse_pos.x, mouse_pos.y }, (float)aimbotFov, IM_COL32(255, 255, 255, 255), 64, 1.f);
}

Player_t* GetClosestPlayer()
{
	static int& aimbotFov = ConfigSys.Get<int>("aimbot.fov");
	Player_t* closest = nullptr;
	float min_dist = (float)aimbotFov;

	auto local_name_hash = fnv1a64_str(PlrCache.LocalPlayer->Name());
	Vec2 mouse_pos = Game.Mouse->GetPos();

	std::lock_guard<std::mutex> lock(PlrCache.mtx);

	for (auto& pair : PlrCache.Players)
	{
		auto& player = pair.second;

		if (pair.first == local_name_hash)
			continue;

		if (!Utils.IsValidPtr((uintptr_t)player.Character) || !Utils.IsValidPtr((uintptr_t)player.Head))
			continue;

		if (!player.Head->IsValidPrim())
			continue;

		bool onscreen = false;
		Vec2 screen_pos = Maths.WorldToScreen(player.Head->Position(), onscreen);

		if (onscreen)
		{
			float dist = (screen_pos - mouse_pos).Length();
			if (dist < min_dist)
			{
				min_dist = dist;
				closest = &player;
			}
		}
	}

	return closest;
}

void Aimbot::AimbotLoop()
{
	static bool& aimbotEnabled = ConfigSys.Get<bool>("aimbot.enabled");
	static int& aimbotFov = ConfigSys.Get<int>("aimbot.fov");
	static float& aimbotSmoothingx = ConfigSys.Get<float>("aimbot.smoothing.x");
	static float& aimbotSmoothingy = ConfigSys.Get<float>("aimbot.smoothing.y");

	auto last_time = std::chrono::high_resolution_clock::now();

	while (m_bRunning)
	{
		if (!Game.isInGame)
			continue;

		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (!aimbotEnabled)
			continue;
		
		auto current_time = std::chrono::high_resolution_clock::now(); 
		float deltaTime = std::chrono::duration<float>(current_time - last_time).count();
		last_time = current_time;

		if (GetForegroundWindow() != Overlay.g_hTargetWindow)
			continue;

		if (!(GetAsyncKeyState(VK_RBUTTON) & 0x8000))
			continue;

		Player_t* target = GetClosestPlayer();
		if (!target)
			continue;

		bool onscreen = false;
		Vec2 target_screen = Maths.WorldToScreen(target->Head->Position(), onscreen);
		if (!onscreen)
			continue;

		Vec2 mouse_pos = Game.Mouse->GetPos();

		float dx = target_screen.x - mouse_pos.x;
		float dy = target_screen.y - mouse_pos.y;

		float mx = (dx * 1.f * 100.0f / aimbotSmoothingx) * deltaTime;
		float my = (dy * 1.f * 100.0f / aimbotSmoothingy) * deltaTime;

		if (std::abs(mx) > 100.f || std::abs(my) > 100.f)
			continue;


		if (std::abs(mx) >= 1.f || std::abs(my) >= 1.f) {
			Utils.MoveMouseRel((int)mx, (int)my);
		}
	}
}

