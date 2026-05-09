#pragma once
#include <unordered_map>
#include <mutex>
#include <thread>
#include <chrono>
#include <atomic>

#include "../sdk/Types.hpp"
#include "../util/fnv1a.h"

namespace RBX
{
	class Player;
	class Instance;
	class Humanoid;
	class BasePart;
}

struct Player_t
{
	std::string Name;
	RBX::Player* Instance;

	RBX::Instance* Character;
	RBX::Humanoid* Humanoid;
	RBX::BasePart* HRP;
	RBX::BasePart* Head;
};

class CPlayerCache final
{
public:
	void Setup();
	void Shutdown();

	RBX::Player* LocalPlayer;
	std::mutex mtx;
	std::unordered_map<uint64_t, Player_t> Players;
private:
	std::thread m_Thread;
	std::atomic<bool> m_bRunning{ false };

	void _Update();
};

inline CPlayerCache PlrCache = CPlayerCache();