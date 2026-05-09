#include <Windows.h>
#include "PlayerEsp.hpp"
#include "../../classes/GameContext.hpp"
#include "../../sdk/sdk.hpp"
#include "../../classes/ConfigSystem.hpp"
#include "../../sdk/Types.hpp"
#include "../../util/util.hpp"
#include "../../classes/Overlay.hpp"
void PlayerEsp::OnRender()
{
	if (!Game.isInGame)
		return;

	static bool& playerEspEnabled = ConfigSys.Get<bool>("visuals.playeresp");
	static ColorRGB& playerEspColor = ConfigSys.Get<ColorRGB>("visuals.playeresp.color");

	if (!playerEspEnabled)
		return;

	std::lock_guard<std::mutex> lock(PlrCache.mtx);

	auto local_name_hash = fnv1a64_str(PlrCache.LocalPlayer->Name());

	ImDrawList* dl = Overlay.BackgroundDrawList;

	for (auto& plr : PlrCache.Players)
	{
		Player_t& player = plr.second;

		if (plr.first == local_name_hash)
			continue;

		if (!Utils.IsValidPtr((uintptr_t)player.Character) ||
			!Utils.IsValidPtr((uintptr_t)player.HRP) ||
			!Utils.IsValidPtr((uintptr_t)player.Head))
			continue;

		if (!player.HRP->IsValidPrim() || !player.Head->IsValidPrim())
			continue;

		Vec3 hrppos = player.HRP->Position();
		Vec3 headpos = player.Head->Position() + Vec3{ 0.f,2.f, 0.f };

		bool onScreen;

		Vec2 hrpscreen = Maths.WorldToScreen(hrppos, onScreen);

		if (!onScreen) continue;

		Vec2 headscreen = Maths.WorldToScreen(headpos, onScreen);

		float height = (hrpscreen.y - headscreen.y);
		float width = height * 0.7f;

		dl->AddRect(
			{ hrpscreen.x - width, hrpscreen.y - height },
			{ hrpscreen.x + width, hrpscreen.y + height },
			IM_COL32(0,0,0,255), 0.f, NULL, 3.f
		);

		dl->AddRect(
			{ hrpscreen.x - width, hrpscreen.y - height },
			{ hrpscreen.x + width, hrpscreen.y + height },
			playerEspColor.ToImU32()
		);
		
		Utils.CenterText(
			player.Name,
			{ hrpscreen.x, hrpscreen.y - height },
			IM_COL32(255, 255, 255, 255),
			TEXT_ALIGN::BOTTOM_CENTER
		);
	}
}

void PlayerEsp::OnInit()
{
}

void PlayerEsp::OnShutdown()
{
}
