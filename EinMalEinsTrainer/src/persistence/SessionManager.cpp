#include "SessionManager.h"
#include "../data/json.hpp"
#include <fstream>
#include <cstdlib>

static std::string getSessionsPath() {
    const char* home = getenv("HOME");
    if (!home) home = "/data/home";
    return std::string(home) + "/.einmaleins_sessions.json";
}

SessionManager::SessionManager()
    : sessionsPath(getSessionsPath())
{
}

static nlohmann::json sessionToJson(const ProgressSession& s) {
    nlohmann::json j;
    j["id"] = s.getId();
    j["startTimestamp"] = s.getStartTimestamp();
    j["endTimestamp"] = s.getEndTimestamp();
    j["correctAnswers"] = s.getCorrectAnswers();
    j["wrongAnswers"] = s.getWrongAnswers();
    j["totalAttempts"] = s.getTotalAttempts();

    auto mc = nlohmann::json::object();
    auto mw = nlohmann::json::object();
    auto dc = nlohmann::json::object();
    auto dw = nlohmann::json::object();
    for (int i = 1; i <= 10; i++) {
        mc[std::to_string(i)] = s.getMultCorrectForSeries(i);
        mw[std::to_string(i)] = s.getMultWrongForSeries(i);
        dc[std::to_string(i)] = s.getDivCorrectForSeries(i);
        dw[std::to_string(i)] = s.getDivWrongForSeries(i);
    }
    j["multCorrect"] = mc;
    j["multWrong"] = mw;
    j["divCorrect"] = dc;
    j["divWrong"] = dw;

    return j;
}

static ProgressSession sessionFromJson(const nlohmann::json& j) {
    ProgressSession s;
    // id stays as generated
    if (j.contains("startTimestamp"))
        s.setStartTimestamp(j["startTimestamp"]);
    if (j.contains("endTimestamp"))
        s.setEndTimestamp(j["endTimestamp"]);
    if (j.contains("correctAnswers"))
        s.setCorrectAnswers(j["correctAnswers"]);
    if (j.contains("wrongAnswers"))
        s.setWrongAnswers(j["wrongAnswers"]);
    if (j.contains("totalAttempts"))
        s.setTotalAttempts(j["totalAttempts"]);

    std::unordered_map<int, int> mc, mw, dc, dw;
    auto loadMap = [](const nlohmann::json& j) -> std::unordered_map<int, int> {
        std::unordered_map<int, int> m;
        if (j.is_object()) {
            for (auto it = j.begin(); it != j.end(); ++it) {
                m[std::stoi(it.key())] = it.value();
            }
        }
        return m;
    };

    if (j.contains("multCorrect")) mc = loadMap(j["multCorrect"]);
    if (j.contains("multWrong")) mw = loadMap(j["multWrong"]);
    if (j.contains("divCorrect")) dc = loadMap(j["divCorrect"]);
    if (j.contains("divWrong")) dw = loadMap(j["divWrong"]);

    s.setSeriesStats(mc, mw, dc, dw);
    return s;
}

std::vector<ProgressSession> SessionManager::loadSessions() {
    std::vector<ProgressSession> sessions;
    std::ifstream f(sessionsPath);
    if (f.is_open()) {
        try {
            nlohmann::json j;
            f >> j;
            if (j.is_array()) {
                for (const auto& item : j) {
                    sessions.push_back(sessionFromJson(item));
                }
            }
        } catch (...) {}
    }
    return sessions;
}

void SessionManager::saveSessions(const std::vector<ProgressSession>& sessions) {
    nlohmann::json j = nlohmann::json::array();
    for (const auto& s : sessions) {
        j.push_back(sessionToJson(s));
    }
    std::ofstream f(sessionsPath);
    if (f.is_open()) {
        f << j.dump(2);
    }
}

void SessionManager::startNewSession() {
    auto sessions = loadSessions();
    sessions.insert(sessions.begin(), ProgressSession());
    saveSessions(sessions);
}

void SessionManager::updateCurrentSession(int correct, int wrong, int total) {
    auto sessions = loadSessions();
    if (!sessions.empty()) {
        auto& current = sessions[0];
        current.setCorrectAnswers(correct);
        current.setWrongAnswers(wrong);
        current.setTotalAttempts(total);
        current.setEndTimestamp(::time(nullptr) * 1000);
        saveSessions(sessions);
    }
}

void SessionManager::updateCurrentSession(int correct, int wrong, int total,
                                           const std::unordered_map<int, int>& multC,
                                           const std::unordered_map<int, int>& multW,
                                           const std::unordered_map<int, int>& divC,
                                           const std::unordered_map<int, int>& divW) {
    auto sessions = loadSessions();
    if (!sessions.empty()) {
        auto& current = sessions[0];
        current.setCorrectAnswers(correct);
        current.setWrongAnswers(wrong);
        current.setTotalAttempts(total);
        current.setEndTimestamp(::time(nullptr) * 1000);
        current.setSeriesStats(multC, multW, divC, divW);
        saveSessions(sessions);
    }
}

void SessionManager::clearAllSessions() {
    saveSessions({});
}

void SessionManager::deleteSession(const std::string& id) {
    auto sessions = loadSessions();
    sessions.erase(
        std::remove_if(sessions.begin(), sessions.end(),
            [&id](const ProgressSession& s) { return s.getId() == id; }),
        sessions.end());
    saveSessions(sessions);
}

bool SessionManager::hasActiveSession() {
    return !loadSessions().empty();
}

void SessionManager::addSessions(const std::vector<ProgressSession>& newSessions) {
    auto sessions = loadSessions();
    sessions.insert(sessions.end(), newSessions.begin(), newSessions.end());
    saveSessions(sessions);
}
