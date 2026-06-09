#include "EinmaleinsConfig.h"

EinmaleinsConfig::EinmaleinsConfig() {
    for (int i = 1; i <= 10; i++) {
        multipliers[i] = std::set<int>();
    }
}

EinmaleinsConfig EinmaleinsConfig::copy() const {
    EinmaleinsConfig c;
    c.baseNumbers = this->baseNumbers;
    for (int i = 1; i <= 10; i++) {
        c.multipliers[i] = this->multipliers[i];
    }
    c.autoSwitchMode = this->autoSwitchMode;
    c.language = this->language;
    return c;
}
