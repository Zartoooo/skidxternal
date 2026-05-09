#include <Windows.h>
#include "sdk/sdk.hpp"
#include "classes/GameContext.hpp"
#include "classes/PlayerCache.hpp"
#include "classes/Overlay.hpp"
#include "classes/ConfigSystem.hpp"
#include "features/FeatureManager.hpp"
#include <iostream>
#include <thread>

int main()
{
	SetConsoleTitle("Fully pasted external using um memory so its fully detected :)");

	if (!memory->attach_to_process("RobloxPlayerBeta.exe"))
	{
		std::cout << "waiting for roblox...\n";

		return 1;
	}

	if (!memory->find_module_address("RobloxPlayerBeta.exe"))
	{
		std::cout << "failed to get base :(\n";
		return 1;
	}

	printf_s("base -> 0x%llx\n", memory->get_module_address());	

	Game.Setup();
	ConfigSys.Init();
	Overlay.Setup();
	PlrCache.Setup();
	Features.Setup();

	system("pause");

	Features.Shutdown();
	PlrCache.Shutdown();
	Overlay.Shutdown();
	Game.Shutdown();

	return 0;
}