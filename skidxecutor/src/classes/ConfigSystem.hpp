#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <any>
#include <stdexcept>
#include "../util/fnv1a.h"

class IConfigVar
{
public:
    virtual ~IConfigVar() = default;
    IConfigVar(std::string name) : m_name(std::move(name)) {}

    virtual std::unique_ptr<IConfigVar> Clone() const = 0;
    virtual std::string GetName() const = 0;
    virtual std::any GetValue() const = 0;
    virtual void SetValue(const std::any& val) = 0;

protected:
    std::string m_name;
};

template <typename T>
class CConfigVar : public IConfigVar
{
public:
    ~CConfigVar() = default;
    CConfigVar(std::string name, T def)
        : IConfigVar(std::move(name)), m_value(std::move(def)) {
    }

    std::string GetName() const override { return m_name; }
    std::unique_ptr<IConfigVar> Clone() const override {
        return std::make_unique<CConfigVar<T>>(m_name, m_value);
	}

    T& Get() { return m_value; }
    const T& Get() const { return m_value; }

    std::any GetValue() const override { return m_value; }
    void SetValue(const std::any& val) override {
        if (val.type() == typeid(T))
            m_value = std::any_cast<T>(val);
    }

private:
    T m_value;
};

class CConfig
{
public:
    std::string name;

    std::unordered_map<std::string, std::unique_ptr<IConfigVar>> m_configVars;
};

class ConfigSystem final
{
public:
    void Init();
	void LoadConfig(const CConfig& config);
    void SaveConfig(CConfig& config);

    template<typename T>
    inline T& Get(const std::string& cvarname)
    {
        auto it = m_currentVars.find(cvarname);
        if (it == m_currentVars.end() || !it->second) {
            throw std::out_of_range("Config var not found: " + cvarname);
        }

        auto* derived = dynamic_cast<CConfigVar<T>*>(it->second.get());
        if (!derived) {
            throw std::runtime_error("Config var type mismatch for: " + cvarname);
        }

        return derived->Get();
    }

    std::unordered_map<std::string, std::unique_ptr<IConfigVar>> m_currentVars;
	std::unordered_map<std::string, std::shared_ptr<CConfig>> m_configs;

    std::shared_ptr<CConfig> LoadFromFile(const std::string& filename);
	void SaveToFile(const std::string& filename, std::shared_ptr<CConfig> config);
};

inline ConfigSystem ConfigSys = ConfigSystem();