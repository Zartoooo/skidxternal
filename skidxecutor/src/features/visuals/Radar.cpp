#include "Radar.hpp"
#include <vector>
#include "../../classes/GameContext.hpp"
#include "../../sdk/sdk.hpp"
#include "../../classes/Overlay.hpp"

void Radar::OnInit()
{
}

void Radar::OnShutdown()
{
}

static constexpr const float RADAR_RANGE = 300.f;
static constexpr const int RADAR_SIZE = 70;
static const Vec2 RADAR_POS = { 300.f, 500.f };

void Radar::OnRender()
{
	if (!Game.isInGame)
		return;

	const float reduction_coef = (float)RADAR_SIZE / RADAR_RANGE;

	std::vector<Vec2> relative_pos{};
	relative_pos.reserve(PlrCache.Players.size());

	auto charr = PlrCache.LocalPlayer->Character();

	if (!Utils.IsValidPtr((uintptr_t)charr))
		return;

	auto hrp = charr->FindFirstChild("HumanoidRootPart");

	if (!Utils.IsValidPtr((uintptr_t)hrp))
		return;

	Vec2 locpos = hrp->As<RBX::BasePart>()->Position().Flatten();

	std::unordered_map<uint64_t, Player_t> plrs = PlrCache.Players;

	for (auto plr : plrs)
	{
		auto hrp = plr.second.HRP;

		if (!Utils.IsValidPtr((uintptr_t)hrp))
			continue;

		Vec2 pos = hrp->Position().Flatten();
		Vec2 rel = pos - locpos;

		if (rel.Length() > RADAR_RANGE)
			continue;

		// rotate
		auto camRot = Game.CurrentCamera->Rotation();
		float yaw = atan2f(camRot.x, camRot.z) - M_PI * 0.5f;

		float cos_y = cosf(-yaw);
		float sin_y = sinf(-yaw);

		Vec2 rotated = {
			rel.x * cos_y - rel.y * sin_y,
			rel.x * sin_y + rel.y * cos_y
		};

		relative_pos.push_back(rotated);
	}

	Overlay.BackgroundDrawList->AddCircleFilled(
		ImVec2(RADAR_POS.x, RADAR_POS.y),
		RADAR_SIZE,
		IM_COL32(30, 30, 30, 255)
	);

	for (auto& pos : relative_pos)
	{
		Vec2 pixel_rel_pos = (pos * reduction_coef);

		Overlay.BackgroundDrawList->AddCircleFilled(
			(RADAR_POS + pixel_rel_pos).ToImGui(),
			2.f,
			IM_COL32(255, 0, 0, 255)
		);
	}

	Overlay.BackgroundDrawList->AddCircleFilled(
		ImVec2(RADAR_POS.x, RADAR_POS.y),
		2.f,
		IM_COL32(0, 0, 255, 255)
	);
}
