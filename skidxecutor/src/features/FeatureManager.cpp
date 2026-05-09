#include <Windows.h>
#include "FeatureManager.hpp"

#include "visuals/PlayerEsp.hpp"
#include "ui/Menu.hpp"
#include "aimbot/Aimbot.hpp"
#include "misc/WalkSpeed.hpp"
#include "visuals/Radar.hpp"

#define REG_FEATURE(_class) Register(std::make_unique<_class>())

void CFeatureManager::RegisterFeatures()
{
	REG_FEATURE(PlayerEsp);
	REG_FEATURE(Menu);
	REG_FEATURE(Aimbot);
	REG_FEATURE(WalkSpeed);
	REG_FEATURE(Radar);
}

void CFeatureManager::Register(std::unique_ptr<IFeature> feature)
{
	m_Features.emplace_back(std::move(feature));
}

void CFeatureManager::Setup()
{
	RegisterFeatures();

	for (auto& f : m_Features)
		f->OnInit();
}

void CFeatureManager::Shutdown()
{
	for (auto& f : m_Features)
		f->OnShutdown();

	m_Features.clear();
}

void CFeatureManager::OnRender()
{
	for (auto& f : m_Features)
		f->OnRender();
}
