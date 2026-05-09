#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <dwmapi.h>
#include <atomic>
#include <thread>

#include "../features/FeatureManager.hpp"
#include "GameContext.hpp"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class COverlay
{
public:
	void Setup();
	void Shutdown();

	HWND g_hWindow = nullptr;
	HWND g_hTargetWindow = nullptr;

	ImDrawList* BackgroundDrawList = nullptr;
	ImDrawList* ForegroundDrawList = nullptr;

private:
	std::atomic<bool> m_bRunning;
	std::thread m_renderThread;

	void _Draw();
	void _Render();
};

inline COverlay Overlay;
