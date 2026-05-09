#include "Overlay.hpp"
#include "../memory/memory.h"
#include "../features/FeatureManager.hpp"
#include "../../ext/imgui/imgui.h"
#include "../../ext/imgui/imgui_impl_win32.h"
#include "../../ext/imgui/imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <format>
#include <windows.h>
#include <psapi.h>
#include "../features/ui/Menu.hpp"
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwmapi.lib")
#include <dwmapi.h>
#include "../sdk/sdk.hpp"
#include "GameContext.hpp"
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void UpdateOverlayToTarget(HWND overlayHwnd);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static bool                     g_SwapChainOccluded = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

void COverlay::Setup()
{
	m_renderThread = std::thread(&COverlay::_Render, this);
}

void COverlay::Shutdown()
{
	m_renderThread.join();
}

void COverlay::_Draw()
{
}

void COverlay::_Render()
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	g_hTargetWindow = FindWindow("WINDOWSCLIENT", "Roblox");
	static bool& m_bMenuOpen = ConfigSys.Get<bool>("menu.open");
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
	::RegisterClassExW(&wc);

	RECT clientRect;
	GetClientRect(g_hTargetWindow, &clientRect);
	POINT origin = { 0, 0 };
	ClientToScreen(g_hTargetWindow, &origin);
	int x = origin.x, y = origin.y;
	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;

	HWND hwnd = CreateWindowExW(
		WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
		wc.lpszClassName, L"", WS_POPUP,
		x, y, width, height,
		nullptr, nullptr, wc.hInstance, nullptr
	);
	this->g_hWindow = hwnd;

	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
	}

	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = "";

	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
	Menu::RegisterFonts();
	ImGui::PushFont(Menu::Fonts::Main);
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
	bool done = false;

	int lastOverlayWidth = 0;
	int lastOverlayHeight = 0;

	while (!done)
	{
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done)
			break;

		bool isGameFocused = (GetForegroundWindow() == g_hTargetWindow);
		bool isMenuFocused = (GetForegroundWindow() == hwnd); 

		bool shouldShow = isGameFocused || m_bMenuOpen || isMenuFocused;

		static bool wasShown = false;
		if (shouldShow != wasShown) {
			ShowWindow(hwnd, shouldShow ? SW_SHOW : SW_HIDE);
			wasShown = shouldShow;
		}

		RECT r;
		GetClientRect(g_hTargetWindow, &r);

		POINT topLeft = { r.left, r.top };
		POINT bottomRight = { r.right, r.bottom };

		ClientToScreen(g_hTargetWindow, &topLeft);
		ClientToScreen(g_hTargetWindow, &bottomRight);

		int width = bottomRight.x - topLeft.x;
		int height = bottomRight.y - topLeft.y;

		static int lastX = -1, lastY = -1, lastW = -1, lastH = -1;

		if (topLeft.x != lastX || topLeft.y != lastY || width != lastW || height != lastH) {
			SetWindowPos(hwnd, NULL, topLeft.x, topLeft.y, width, height, SWP_SHOWWINDOW);
			lastX = topLeft.x; lastY = topLeft.y; lastW = width; lastH = height;
		}
		

		RECT overlayClient;
		if (GetClientRect(hwnd, &overlayClient))
		{
			int overlayW = overlayClient.right - overlayClient.left;
			int overlayH = overlayClient.bottom - overlayClient.top;

			if (overlayW > 0 && overlayH > 0)
			{
				if (overlayW != lastOverlayWidth || overlayH != lastOverlayHeight)
				{
					lastOverlayWidth = overlayW;
					lastOverlayHeight = overlayH;

					g_ResizeWidth = (UINT)overlayW;
					g_ResizeHeight = (UINT)overlayH;
				}
			}
		}


		if (g_SwapChainOccluded && g_pSwapChain && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
		{
			::Sleep(10);
			continue;
		}
		g_SwapChainOccluded = false;

		if (g_ResizeWidth && g_ResizeHeight)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
			g_ResizeWidth = g_ResizeHeight = 0;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		BackgroundDrawList = ImGui::GetBackgroundDrawList();
		ForegroundDrawList = ImGui::GetForegroundDrawList();

		Game.VisengineCache.ViewMatrix = Game.VisualEngine->ViewMatrix();
		Game.VisengineCache.Dimensions = Game.VisualEngine->Dimensions();

		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("debug :3");                          

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			PROCESS_MEMORY_COUNTERS_EX pmc;
			GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
			SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;

			ImGui::Text("Virtual Memory Usage: %s", std::format("{:.2f} MB", virtualMemUsedByMe / 1000000.f).c_str());


			const Vec3 crot = Game.CurrentCamera->Rotation();

			ImGui::Text("Cam rot: x=%.1f,y=%.1f,y=%.1f", crot.x, crot.y, crot.z);

			ImGui::Text("InGame = %s", (Game.isInGame ? "true" : "false"));

			ImGui::End();
		}


		Features.OnRender();

		ImGui::Render();
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(0, 0);
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

bool CreateDeviceD3D(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;


	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED)
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;
		g_ResizeWidth = (UINT)LOWORD(lParam); 
		g_ResizeHeight = (UINT)HIWORD(lParam);
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}