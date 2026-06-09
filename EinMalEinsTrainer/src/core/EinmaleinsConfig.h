#pragma once

#include <set>
#include <string>
#include <array>

class EinmaleinsConfig {
public:
    std::set<int> baseNumbers;
    std::array<std::set<int>, 11> multipliers;
    bool autoSwitchMode = true;
    std::string language = "en";

    EinmaleinsConfig();

    EinmaleinsConfig copy() const;
};
