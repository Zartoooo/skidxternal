#pragma once
#include <thread>
#include <chrono>
#include <atomic>
#include "../sdk/Types.hpp"

namespace RBX
{
	class DataModel;
	class VisualEngine;
	class Players;
	class Workspace;
	class MouseService;
	class Camera;
}

struct VisengineCache_t
{
	Matrix4x4 ViewMatrix;
	Vec2 Dimensions;
};

class CGameContext final
{
public:
	void Setup();
	void Shutdown();

	RBX::DataModel* DataModel = nullptr;
	RBX::VisualEngine* VisualEngine = nullptr;
	VisengineCache_t VisengineCache{};


	std::atomic<bool> isInGame = false;

	RBX::Players* Players = nullptr;
	RBX::Workspace* Workspace = nullptr;
	RBX::MouseService* Mouse = nullptr;
	RBX::Camera* CurrentCamera = nullptr;

private:
	std::thread m_rescanThread;

	void _Setup();
	void _RescanLoop();
};

inline CGameContext Game = CGameContext();