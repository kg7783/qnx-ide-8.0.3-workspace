#define _POSIX_C_SOURCE 199309L
#include "ProgressSession.h"
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <time.h>

static std::string generateUUID() {
    std::srand(std::time(nullptr) ^ (std::rand() << 16));
    std::ostringstream oss;
    oss << std::hex;
    for (int i = 0; i < 8; i++) oss << (std::rand() % 16);
    oss << "-";
    for (int i = 0; i < 4; i++) oss << (std::rand() % 16);
    oss << "-4";
    for (int i = 0; i < 3; i++) oss << (std::rand() % 16);
    oss << "-";
    oss << (8 + std::rand() % 4);
    for (int i = 0; i < 3; i++) oss << (std::rand() % 16);
    oss << "-";
    for (int i = 0; i < 12; i++) oss << (std::rand() % 16);
    return oss.str();
}

static int64_t nowMs() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (int64_t)ts.tv_sec * 1000 + (int64_t)ts.tv_nsec / 1000000;
}

static void initMapsImpl(std::unordered_map<int, int>& multC,
                         std::unordered_map<int, int>& multW,
                         std::unordered_map<int, int>& divC,
                         std::unordered_map<int, int>& divW) {
    for (int i = 1; i <= 10; i++) {
        multC[i] = 0;
        multW[i] = 0;
        divC[i] = 0;
        divW[i] = 0;
    }
}

void ProgressSession::initMaps() {
    ::initMapsImpl(multCorrect, multWrong, divCorrect, divWrong);
}

ProgressSession::ProgressSession()
    : id(generateUUID())
    , startTimestamp(nowMs())
    , endTimestamp(0)
    , correctAnswers(0)
    , wrongAnswers(0)
    , totalAttempts(0)
{
    initMaps();
}

ProgressSession::ProgressSession(int correct, int wrong, int total)
    : id(generateUUID())
    , startTimestamp(nowMs())
    , endTimestamp(nowMs())
    , correctAnswers(correct)
    , wrongAnswers(wrong)
    , totalAttempts(total)
{
    initMaps();
}

double ProgressSession::getAccuracy() const {
    return totalAttempts > 0 ? (double)correctAnswers / totalAttempts * 100.0 : 0.0;
}

int ProgressSession::getMultCorrectForSeries(int series) const {
    auto it = multCorrect.find(series);
    return it != multCorrect.end() ? it->second : 0;
}

int ProgressSession::getMultWrongForSeries(int series) const {
    auto it = multWrong.find(series);
    return it != multWrong.end() ? it->second : 0;
}

int ProgressSession::getDivCorrectForSeries(int series) const {
    auto it = divCorrect.find(series);
    return it != divCorrect.end() ? it->second : 0;
}

int ProgressSession::getDivWrongForSeries(int series) const {
    auto it = divWrong.find(series);
    return it != divWrong.end() ? it->second : 0;
}

void ProgressSession::setSeriesStats(const std::unordered_map<int, int>& multC,
                                     const std::unordered_map<int, int>& multW,
                                     const std::unordered_map<int, int>& divC,
                                     const std::unordered_map<int, int>& divW) {
    multCorrect = multC;
    multWrong = multW;
    divCorrect = divC;
    divWrong = divW;
}

int ProgressSession::getTotalMult() const {
    int total = 0;
    for (int i = 1; i <= 10; i++) {
        total += getMultCorrectForSeries(i) + getMultWrongForSeries(i);
    }
    return total;
}

int ProgressSession::getTotalDiv() const {
    int total = 0;
    for (int i = 1; i <= 10; i++) {
        total += getDivCorrectForSeries(i) + getDivWrongForSeries(i);
    }
    return total;
}
