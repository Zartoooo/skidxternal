#include "sdk.hpp"
#include "../util/util.hpp"
#define Self(ts) reinterpret_cast<unsigned long long>(ts)

std::vector<RBX::Instance*> RBX::Instance::GetChildren()
{
	static thread_local std::vector<RBX::Instance*> container;
	container.clear();

	auto start = memory->read<unsigned long long>(Self(this) + offsets::Instance::ChildrenStart);
	auto end = memory->read<unsigned long long>(start + offsets::Instance::ChildrenEnd);

	for (auto instance = memory->read<unsigned long long>(start); instance < end; instance += sizeof(std::shared_ptr<void*>))
	{
		container.emplace_back(memory->read<RBX::Instance*>(instance));
	}

	return container;
}

RBX::Instance* RBX::Instance::FindFirstChild(std::string_view name)
{
	auto childs = GetChildren();

	for (auto& child : childs)
	{
		if (child->Name() == name)
		{
			return child;
		}
	}

	return nullptr;
}

RBX::Instance* RBX::Instance::FindFirstChildOfClass(std::string_view class_name)
{
	auto childs = GetChildren();

	for (auto& child : childs)
	{
		if (child->ClassName() == class_name)
		{
			return child;
		}
	}

	return nullptr;
}

std::string RBX::Instance::Name()
{
	auto name = memory->read<unsigned long long>(Self(this) + offsets::Instance::Name);

	if (name)
	{
		return memory->read_string(name);
	}

	return "str_error";
}

std::string RBX::Instance::ClassName()
{
	auto class_descriptor = memory->read<unsigned long long>(Self(this) + offsets::Instance::ClassDescriptor);
	auto class_name = memory->read<unsigned long long>(class_descriptor + offsets::Instance::ClassName);

	if (class_name)
	{
		return memory->read_string(class_name);
	}

	return "str_error";
}

// broken
uint64_t RBX::Player::UserId()
{
	return memory->read<uint64_t>(Self(this) + offsets::Player::UserId);
}

std::string RBX::Player::DisplayName()
{
	auto name = memory->read_string(Self(this) + offsets::Player::DisplayName);

	return name;
}

RBX::Instance* RBX::Player::Character()
{
	return memory->read<RBX::Instance*>(Self(this) + offsets::Player::Character);
}

RBX::Player* RBX::Players::LocalPlayer()
{
	return memory->read<RBX::Player*>(Self(this) + offsets::Players::LocalPlayer);
}


std::vector<RBX::Player*> RBX::Players::GetPlayers(bool exclude_local)
{
	auto children = GetChildren();

	static thread_local std::vector<RBX::Player*> players;
	players.clear();

	for (auto child : children)
	{

		if (child->ClassName() == "Player")
		{
			players.push_back(reinterpret_cast<RBX::Player*>(child));
		}
	}

	return players;
}

void RBX::Humanoid::SetWalkSpeed(float ws)
{
	memory->write<float>(Self(this) + offsets::Humanoid::WalkSpeed, ws);
	memory->write<float>(Self(this) + offsets::Humanoid::WalkSpeedCheck, ws);
}

float RBX::Humanoid::GetWalkSpeed()
{
	return memory->read<float>(Self(this) + offsets::Humanoid::WalkSpeed);
}

float RBX::Humanoid::GetHealth()
{
	return memory->read<float>(Self(this) + offsets::Humanoid::Health);
}

float RBX::Humanoid::GetMaxHealth()
{
	return memory->read<float>(Self(this) + offsets::Humanoid::MaxHealth);
}

Matrix4x4 RBX::VisualEngine::ViewMatrix()
{
	return memory->read<Matrix4x4>(Self(this) + offsets::VisualEngine::ViewMatrix);
}

Vec2 RBX::VisualEngine::Dimensions()
{
	return memory->read<Vec2>(Self(this) + offsets::VisualEngine::Dimensions);
}

float RBX::Workspace::GetGravity()
{
	return memory->read<float>(Self(this) + offsets::Workspace::ReadOnlyGravity);
}

void RBX::Workspace::SetGravity(float grav)
{
	auto world = memory->read<unsigned long long>(Self(this) + offsets::Workspace::World);
	memory->write<float>(world + offsets::World::Gravity, grav);
}

RBX::Workspace* RBX::DataModel::Workspace()
{
	return memory->read<RBX::Workspace*>(Self(this) + offsets::DataModel::Workspace);
}

Vec3 RBX::BasePart::Position()
{
	auto prim = memory->read<unsigned long long>(Self(this) + offsets::BasePart::Primitive);
	if (!Utils.IsValidPtr(prim))
		return { 0, 0, 0 };

	return memory->read<Vec3>(prim + offsets::Primitive::Position);
}

bool RBX::BasePart::IsValidPrim()
{
	auto prim = memory->read<uintptr_t>(Self(this) + offsets::BasePart::Primitive);
	return Utils.IsValidPtr(prim);
}

Vec2 RBX::MouseService::GetPos()
{
	uintptr_t InputObject = memory->read<uintptr_t>(Self(this) + offsets::MouseService::InputObject);

	if (!Utils.IsValidPtr(InputObject)) return { 0.f, 0.f };

	return memory->read<Vec2>(InputObject + offsets::InputObject::MousePosition);
}

Vec3 RBX::Camera::Rotation()
{
	return memory->read<Vec3>(Self(this) + offsets::Camera::Rotation);
}

Vec3 RBX::Camera::Position()
{
	return memory->read<Vec3>(Self(this) + offsets::Primitive::Position);
}
