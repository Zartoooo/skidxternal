#pragma once
#include <cstdint>
#include <Windows.h>
#include "../../ext/imgui/imgui.h"
#include <string>

class COverlay;
extern COverlay Overlay;

enum class TEXT_ALIGN
{
	TOP_LEFT,
	TOP_CENTER,
	TOP_RIGHT,
	CENTER_LEFT,
	CENTER_CENTER,
	CENTER_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_CENTER,
	BOTTOM_RIGHT
};

class CUtils final
{
public:
	void CenterText(const std::string& str, ImVec2 pos, ImU32 color, TEXT_ALIGN align);

	inline bool IsValidPtr(uintptr_t addr)
	{
		if (!addr || addr < 0x10000 || addr > 0x7FFFFFFFFFFF)
			return false;
		return true;
	}

	inline void MoveMouseRel(int x, int y)
	{
		INPUT inp;
		inp.type = INPUT_MOUSE;
		inp.mi.dx = x;
		inp.mi.dy = y;
		inp.mi.dwFlags = MOUSEEVENTF_MOVE;
		inp.mi.time = 0;
		inp.mi.mouseData = 0;

		SendInput(1, &inp, sizeof(INPUT));
	}

	inline void MoveMouseAbs(int x, int y)
	{
		INPUT inp;
		inp.type = INPUT_MOUSE;
		inp.mi.dx = x;
		inp.mi.dy = y;
		inp.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
		inp.mi.time = 0;
		inp.mi.mouseData = 0;

		SendInput(1, &inp, sizeof(INPUT));
	}
};

inline CUtils Utils = CUtils();