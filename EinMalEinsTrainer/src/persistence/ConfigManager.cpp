#include "ConfigManager.h"
#include "../data/json.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>

static std::string getConfigPath() {
    const char* home = getenv("HOME");
    if (!home) home = "/data/home";
    return std::string(home) + "/.einmaleins_config.json";
}

ConfigManager::ConfigManager()
    : configPath(getConfigPath())
{
    load();
}

void ConfigManager::load() {
    std::ifstream f(configPath);
    if (f.is_open()) {
        try {
            f >> data;
        } catch (...) {
            data = nlohmann::json::object();
        }
    } else {
        data = nlohmann::json::object();
    }
}

void ConfigManager::store() {
    std::ofstream f(configPath);
    if (f.is_open()) {
        f << data.dump(2);
    }
}

void ConfigManager::saveConfig(const EinmaleinsConfig& config) {
    std::string baseStr;
    for (int num : config.baseNumbers) {
        if (!baseStr.empty()) baseStr += ",";
        baseStr += std::to_string(num);
    }
    data["baseNumbers"] = baseStr;

    for (int i = 1; i <= 10; i++) {
        std::string multStr;
        for (int mult : config.multipliers[i]) {
            if (!multStr.empty()) multStr += ",";
            multStr += std::to_string(mult);
        }
        data["multipliers" + std::to_string(i)] = multStr;
    }

    data["autoSwitchMode"] = config.autoSwitchMode;
    data["language"] = config.language;
    store();
}

EinmaleinsConfig ConfigManager::loadConfig() {
    EinmaleinsConfig config;

    if (data.contains("baseNumbers")) {
        std::string baseStr = data["baseNumbers"];
        if (!baseStr.empty()) {
            std::istringstream ss(baseStr);
            std::string part;
            while (std::getline(ss, part, ',')) {
                if (!part.empty()) config.baseNumbers.insert(std::stoi(part));
            }
        }
    }

    for (int i = 1; i <= 10; i++) {
        std::string key = "multipliers" + std::to_string(i);
        if (data.contains(key)) {
            std::string multStr = data[key];
            if (!multStr.empty()) {
                std::istringstream ss(multStr);
                std::string part;
                while (std::getline(ss, part, ',')) {
                    if (!part.empty()) config.multipliers[i].insert(std::stoi(part));
                }
            }
        }
    }

    if (data.contains("autoSwitchMode"))
        config.autoSwitchMode = data["autoSwitchMode"];
    if (data.contains("language"))
        config.language = data["language"];

    if (config.baseNumbers.empty()) {
        for (int i = 1; i <= 10; i++) {
            config.baseNumbers.insert(i);
            for (int j = 1; j <= 10; j++) {
                config.multipliers[i].insert(j);
            }
        }
    }

    return config;
}

bool ConfigManager::hasConfig() {
    return data.contains("baseNumbers");
}

void ConfigManager::saveCustomDateRangeStats(int64_t startDate, int64_t endDate) {
    data["customStartDateStats"] = startDate;
    data["customEndDateStats"] = endDate;
    store();
}

int64_t ConfigManager::getCustomStartDateStats() {
    return data.value("customStartDateStats", (int64_t)0);
}

int64_t ConfigManager::getCustomEndDateStats() {
    return data.value("customEndDateStats", (int64_t)0);
}

void ConfigManager::saveCustomDateRangeSeries(int64_t startDate, int64_t endDate) {
    data["customStartDateSeries"] = startDate;
    data["customEndDateSeries"] = endDate;
    store();
}

int64_t ConfigManager::getCustomStartDateSeries() {
    return data.value("customStartDateSeries", (int64_t)0);
}

int64_t ConfigManager::getCustomEndDateSeries() {
    return data.value("customEndDateSeries", (int64_t)0);
}

void ConfigManager::saveLastFilterStats(const std::string& filterName) {
    data["lastFilterStats"] = filterName;
    store();
}

std::string ConfigManager::getLastFilterStats() {
    return data.value("lastFilterStats", std::string("ALL"));
}

void ConfigManager::saveLastFilterSeries(const std::string& filterName) {
    data["lastFilterSeries"] = filterName;
    store();
}

std::string ConfigManager::getLastFilterSeries() {
    return data.value("lastFilterSeries", std::string("ALL"));
}

void ConfigManager::saveLanguage(const std::string& language) {
    data["language"] = language;
    store();
}

std::string ConfigManager::getLanguage() {
    return data.value("language", std::string("en"));
}
