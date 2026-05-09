#pragma once

class IFeature
{
public:
	virtual ~IFeature() = default;

public:
	virtual void OnInit() = 0;
	virtual void OnShutdown() = 0;
	virtual void OnRender() = 0;
};