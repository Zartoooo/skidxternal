#pragma once
#include <cmath>
#include "../../ext/imgui/imgui.h"

// common data types
struct Vec2
{
	float x, y;

	Vec2 operator+(const Vec2& v) const { return { x + v.x, y + v.y }; }
	Vec2 operator-(const Vec2& v) const { return { x - v.x, y - v.y }; }
	Vec2 operator*(const Vec2& v) const { return { x * v.x, y * v.y }; }
	Vec2 operator/(const Vec2& v) const { return { x / v.x, y / v.y }; }

	Vec2 operator+(float v) const { return { x + v, y + v }; }
	Vec2 operator-(float v) const { return { x - v, y - v }; }
	Vec2 operator*(float v) const { return { x * v, y * v }; }
	Vec2 operator/(float v) const { return { x / v, y / v }; }

	Vec2& operator+=(const Vec2& v) { x += v.x; y += v.y; return *this; }
	Vec2& operator-=(const Vec2& v) { x -= v.x; y -= v.y; return *this; }
	Vec2& operator*=(const Vec2& v) { x *= v.x; y *= v.y; return *this; }
	Vec2& operator/=(const Vec2& v) { x /= v.x; y /= v.y; return *this; }

	Vec2& operator+=(float v) { x += v; y += v; return *this; }
	Vec2& operator-=(float v) { x -= v; y -= v; return *this; }
	Vec2& operator*=(float v) { x *= v; y *= v; return *this; }
	Vec2& operator/=(float v) { x /= v; y /= v; return *this; }

	bool operator==(const Vec2& v) const { return x == v.x && y == v.y; }
	bool operator!=(const Vec2& v) const { return !(*this == v); }

	Vec2 operator-() const { return { -x, -y }; }

	float Length() const { return std::sqrt(x * x + y * y); }
	float LengthSqr() const { return x * x + y * y; }
	float DistTo(const Vec2& v) const { return (*this - v).Length(); }
	float DistToSqr(const Vec2& v) const { return (*this - v).LengthSqr(); }

	ImVec2 ToImGui() { return ImVec2(x, y); }
};

struct Vec3
{
	float x, y, z;

	Vec3 operator+(const Vec3& v) const { return { x + v.x, y + v.y, z + v.z }; }
	Vec3 operator-(const Vec3& v) const { return { x - v.x, y - v.y, z - v.z }; }
	Vec3 operator*(const Vec3& v) const { return { x * v.x, y * v.y, z * v.z }; }
	Vec3 operator/(const Vec3& v) const { return { x / v.x, y / v.y, z / v.z }; }

	Vec3 operator+(float v) const { return { x + v, y + v, z + v }; }
	Vec3 operator-(float v) const { return { x - v, y - v, z - v }; }
	Vec3 operator*(float v) const { return { x * v, y * v, z * v }; }
	Vec3 operator/(float v) const { return { x / v, y / v, z / v }; }

	Vec3& operator+=(const Vec3& v) { x += v.x; y += v.y; z += v.z; return *this; }
	Vec3& operator-=(const Vec3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	Vec3& operator*=(const Vec3& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
	Vec3& operator/=(const Vec3& v) { x /= v.x; y /= v.y; z /= v.z; return *this; }

	Vec3& operator+=(float v) { x += v; y += v; z += v; return *this; }
	Vec3& operator-=(float v) { x -= v; y -= v; z -= v; return *this; }
	Vec3& operator*=(float v) { x *= v; y *= v; z *= v; return *this; }
	Vec3& operator/=(float v) { x /= v; y /= v; z /= v; return *this; }

	bool operator==(const Vec3& v) const { return x == v.x && y == v.y && z == v.z; }
	bool operator!=(const Vec3& v) const { return !(*this == v); }

	Vec3 operator-() const { return { -x, -y, -z }; }

	float Length() const { return std::sqrt(x * x + y * y + z * z); }
	float LengthSqr() const { return x * x + y * y + z * z; }
	float DistTo(const Vec3& v) const { return (*this - v).Length(); }
	float DistToSqr(const Vec3& v) const { return (*this - v).LengthSqr(); }
	Vec2 Flatten() const { return Vec2(x, y); }

	float Dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }
	Vec3 Cross(const Vec3& v) const { return { y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x }; }
};

struct Quaternion // no idea wtf is ts
{
	float x, y, z, w;
};

struct Matrix3x4
{
	float m[3][4];

	float* operator[](int idx) { return m[idx]; };
};

struct Matrix4x4
{
	float m[16];

	float operator[](int idx) { return m[idx]; };
};

struct ColorRGB
{
	int r, g, b, a;

	ImU32 ToImU32() const
	{
		return IM_COL32(r, g, b, a);
	}

	ImVec4 ToImVec4() const
	{
		return ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
	}

	void FromArray(float* col)
	{
		r = static_cast<int>(col[0] * 255.f);
		g = static_cast<int>(col[1] * 255.f);
		b = static_cast<int>(col[2] * 255.f);
		a = static_cast<int>(col[3] * 255.f);
	}
};