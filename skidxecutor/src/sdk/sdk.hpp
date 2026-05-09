#pragma once
#include <vector>
#include <string>
#include <functional>

#include "memory/memory.h"
#include "offset.hpp"
#include "Types.hpp"

namespace RBX
{
	class Instance
	{
	public:
		std::vector<Instance*> GetChildren();

		Instance* FindFirstChild(std::string_view name);

		Instance* FindFirstChildOfClass(std::string_view class_name);

		std::string Name();
		std::string ClassName();

		template <typename T>
		inline T* As()
		{
			return reinterpret_cast<T*>(this);
		}
	};

	class BasePart : public Instance
	{
	public:
		Vec3 Position();
		bool IsValidPrim();
	};

	class Workspace final : public Instance
	{
	public:
		float GetGravity();
		void SetGravity(float grav);
	};

	class DataModel final : public Instance
	{
	public:
		Workspace* Workspace();
	};

	class VisualEngine final
	{
	public:
		Matrix4x4 ViewMatrix();
		Vec2 Dimensions();
	};

	class Humanoid final : public Instance
	{
	public:
		void SetWalkSpeed(float ws);
		float GetWalkSpeed();

		float GetHealth();
		float GetMaxHealth();
	};

	class Player final : public Instance
	{
	public:
		uint64_t UserId();
		std::string DisplayName();
		Instance* Character();
	};

	class Players final : public Instance
	{
	public:
		Player* LocalPlayer();
		std::vector<Player*> GetPlayers(bool exclude_local = false);
	};

	class MouseService : public Instance
	{
	public:
		Vec2 GetPos();
	};

	class Camera : public Instance
	{
	public:
		Vec3 Rotation();
		Vec3 Position();
	};
}