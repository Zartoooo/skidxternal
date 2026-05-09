#include "PlayerCache.hpp"
#include "GameContext.hpp"
#include "../sdk/sdk.hpp"
#include "../util/util.hpp"

void CPlayerCache::Setup()
{
	m_bRunning = true;
	m_Thread = std::thread(&CPlayerCache::_Update, this);
}

void CPlayerCache::Shutdown()
{
	m_bRunning = false;
	Players.clear();
	if (m_Thread.joinable())
		m_Thread.join();
}

void CPlayerCache::_Update()
{
	while (m_bRunning)
	{
		auto players = Game.Players->GetPlayers();

		auto locplrnamehash = fnv1a64_str(Game.Players->LocalPlayer()->Name());

		std::unordered_map<uint64_t, Player_t> newPlayers;
		newPlayers.reserve(players.size());

		RBX::Player* foundLocPlr = nullptr;
		for (auto* plr : players)
		{
			const auto& n = plr->Name();
			uint64_t hash = fnv1a64_str(n);

			if (hash == locplrnamehash)
				foundLocPlr = plr;

			auto* humanoid = (RBX::Humanoid*)plr->FindFirstChildOfClass("Humanoid");

			auto* character = plr->Character();
			RBX::BasePart* hrp = nullptr;
			RBX::BasePart* head = nullptr;

			if (Utils.IsValidPtr((uintptr_t)character) && character->ClassName() == "Model")
			{
				hrp = (RBX::BasePart*)character->FindFirstChild("HumanoidRootPart");
				head = (RBX::BasePart*)character->FindFirstChild("Head");
			}

			newPlayers.emplace(hash, Player_t{ n, plr, character, humanoid, hrp, head });
		}

		{
			std::lock_guard<std::mutex> lock(mtx);
			this->LocalPlayer = foundLocPlr;
			Players = std::move(newPlayers);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}
