#pragma once

#include <vector>
#include "../core/ProgressSession.h"

class SessionManager {
public:
    SessionManager();

    std::vector<ProgressSession> loadSessions();
    void startNewSession();
    void updateCurrentSession(int correct, int wrong, int total);
    void updateCurrentSession(int correct, int wrong, int total,
                              const std::unordered_map<int, int>& multCorrect,
                              const std::unordered_map<int, int>& multWrong,
                              const std::unordered_map<int, int>& divCorrect,
                              const std::unordered_map<int, int>& divWrong);
    void clearAllSessions();
    void deleteSession(const std::string& id);
    bool hasActiveSession();

    void addSessions(const std::vector<ProgressSession>& newSessions);

private:
    std::string sessionsPath;

    void saveSessions(const std::vector<ProgressSession>& sessions);
};
