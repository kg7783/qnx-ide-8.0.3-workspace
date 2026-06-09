#pragma once

#include <string>
#include "../data/json.hpp"
#include "../core/EinmaleinsConfig.h"

class ConfigManager {
public:
    ConfigManager();

    void saveConfig(const EinmaleinsConfig& config);
    EinmaleinsConfig loadConfig();
    bool hasConfig();

    void saveCustomDateRangeStats(int64_t startDate, int64_t endDate);
    int64_t getCustomStartDateStats();
    int64_t getCustomEndDateStats();

    void saveCustomDateRangeSeries(int64_t startDate, int64_t endDate);
    int64_t getCustomStartDateSeries();
    int64_t getCustomEndDateSeries();

    void saveLastFilterStats(const std::string& filterName);
    std::string getLastFilterStats();
    void saveLastFilterSeries(const std::string& filterName);
    std::string getLastFilterSeries();

    void saveLanguage(const std::string& language);
    std::string getLanguage();

private:
    std::string configPath;
    nlohmann::json data;

    void load();
    void store();
};
