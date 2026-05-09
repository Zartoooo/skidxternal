#pragma once
#include <vector>
#include <memory>

#include "IFeature.hpp"


// funny system, totally useless
class CFeatureManager
{
public:
	void RegisterFeatures();

	void Register(std::unique_ptr<IFeature> feature);

	void Setup();
	void Shutdown();
	void OnRender();

private:
	std::vector<std::unique_ptr<IFeature>> m_Features;
};

inline CFeatureManager Features = CFeatureManager();