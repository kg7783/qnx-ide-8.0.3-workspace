#include "ui/ScreenManager.h"
#include "ui/MainScreen.h"
#include "ui/SetupScreen.h"
#include "ui/HistoryScreen.h"
#include "persistence/ConfigManager.h"
#include "data/strings_en.h"
#include "data/strings_de.h"
#include <cstdio>
#include <memory>

static std::string getString(const std::string& key, bool useGerman) {
    auto& map = useGerman ? strings_de : strings_en;
    auto it = map.find(key);
    if (it != map.end()) return it->second;
    return key;
}

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;

    ConfigManager cfgMgr;
    std::string lang = cfgMgr.getLanguage();
    bool useGerman = (lang == "de");

    ScreenManager mgr;
    if (!mgr.init(800, 480)) {
        fprintf(stderr, "Failed to initialize ScreenManager\n");
        return 1;
    }

    mgr.setStringProvider([useGerman](const std::string& key) -> std::string {
        return getString(key, useGerman);
    });

    mgr.addScreen("main", std::make_unique<MainScreen>(&mgr));
    mgr.addScreen("setup", std::make_unique<SetupScreen>(&mgr));
    mgr.addScreen("history", std::make_unique<HistoryScreen>(&mgr));

    mgr.pushScreen("main");
    mgr.run();

    return 0;
}
