#include "ConfigSystem.hpp"
#include "../../ext/imgui/imgui.h"
#include "../sdk/Types.hpp"
#include "../../ext/json/json.hpp"

#include <fstream>
#include <cstdio>
#include <filesystem>
#include <iostream>

#define CREATE_CVAR(type, name, def) \
	m_currentVars[std::string(name)] = std::make_unique<CConfigVar<type>>(name, def);

using json = nlohmann::json;

void ConfigSystem::Init()
{

	CREATE_CVAR(bool, "menu.open", false);
	CREATE_CVAR(bool, "aimbot.enabled", false);
	CREATE_CVAR(int, "aimbot.fov", 100);       
	CREATE_CVAR(bool, "aimbot.drawfov", true);
	CREATE_CVAR(float, "aimbot.smoothing.x", 1.1f);
	CREATE_CVAR(float, "aimbot.smoothing.y", 1.1f);

	CREATE_CVAR(bool, "visuals.playeresp", true);
	CREATE_CVAR(ColorRGB, "visuals.playeresp.color", ColorRGB(255,255,255,255));

    CREATE_CVAR(int, "misc.walkspeed", 0);

	m_configs["default"] = std::make_shared<CConfig>();
    m_configs["default"]->name = "default";
	SaveConfig( * m_configs["default"].get()); 

    std::filesystem::create_directories("configs");
    for (const auto& entry : std::filesystem::directory_iterator("configs"))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".json")
        {
            std::string filename = entry.path().filename().string();
            auto cfg = LoadFromFile(filename);
            if (cfg)
            {
                m_configs[cfg->name] = cfg;
            }
        }
    }
}

void ConfigSystem::LoadConfig(const CConfig& config)
{
    for (const auto& [key, value] : config.m_configVars)
    {
        auto it = m_currentVars.find(key);
        if (it != m_currentVars.end())
        {
            it->second->SetValue(value->GetValue());
        }
    }

    printf("Loading config %s\n", config.name.c_str());
}

void ConfigSystem::SaveConfig(CConfig& config)
{
    config.m_configVars.clear();

    for (const auto& [key, value] : m_currentVars)
    {
        config.m_configVars[key] = value->Clone();
    }

    printf("Saving config %s\n", config.name.c_str());
}


std::shared_ptr<CConfig> ConfigSystem::LoadFromFile(const std::string& filename)
{
    std::ifstream file("configs/" + filename);
    if (!file.is_open())
        return nullptr;

    json j;
    try {
        file >> j;
    } catch (...) {
        return nullptr;
    }

    CConfig cfg = CConfig();

    if (j.contains("config-name"))
        cfg.name = j["config-name"];
    else
        cfg.name = filename.substr(0, filename.find_last_of("."));

    if (j.contains("vars"))
    {
        std::unordered_map<std::string, nlohmann::json> varMap;
        varMap = j["vars"].get<std::unordered_map<std::string, nlohmann::json>>();

        for (auto& [key, value] : varMap)
        {
            if (value.is_boolean())
            {
                cfg.m_configVars[key] = std::make_unique<CConfigVar<bool>>(key, value.get<bool>());

            }
            else if (value.is_number_integer())
            {
                cfg.m_configVars[key] = std::make_unique<CConfigVar<int>>(key, value.get<int>());
            }
            else if (value.is_number_float())
            {
                cfg.m_configVars[key] = std::make_unique<CConfigVar<float>>(key, value.get<float>());
            }
            else if (value.is_object() && value.contains("r") && value.contains("g") && value.contains("b") && value.contains("a"))
            {
                ColorRGB color;
                color.r = value["r"].get<int>();
                color.g = value["g"].get<int>();
                color.b = value["b"].get<int>();
                color.a = value["a"].get<int>();
                cfg.m_configVars[key] = std::make_unique<CConfigVar<ColorRGB>>(key, color);
            }
        }
    }

    printf("Loading config from file %s\n", cfg.name.c_str());

	return std::make_shared<CConfig>(std::move(cfg));
}

void ConfigSystem::SaveToFile(const std::string& filename, std::shared_ptr<CConfig> config)
{
    json j;
	j["config-name"] = config->name;

	json vars = json::object();

	for (const auto& [key, value] : config->m_configVars)
    {
        std::any val = value->GetValue();
        if (val.type() == typeid(bool))
        {
            vars[key] = std::any_cast<bool>(val);
        }
        else if (val.type() == typeid(int))
        {
            vars[key] = std::any_cast<int>(val);
        }
        else if (val.type() == typeid(float))
        {
            vars[key] = std::any_cast<float>(val);
        }
        else if (val.type() == typeid(ColorRGB))
        {
            ColorRGB color = std::any_cast<ColorRGB>(val);
            vars[key] = { {"r", color.r}, {"g", color.g}, {"b", color.b}, {"a", color.a} };
        }
    }

    j["vars"] = vars;
    std::filesystem::create_directories("configs");
    std::ofstream file("configs/" + filename);
    if (!file.is_open())
    {
        return;
    }

    printf("Saving config to file %s\n", config->name.c_str());

    file << j.dump(4);
    file.close();
}
