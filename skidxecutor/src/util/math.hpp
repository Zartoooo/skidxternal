#pragma once
#include "../sdk/Types.hpp"

#define M_PI 	3.14159265358979323846

class CMaths final
{
public:
	Vec2 WorldToScreen(const Vec3& world, bool& onScreen);
};

inline CMaths Maths = CMaths();