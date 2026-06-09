#include "TestDataGenerator.h"
#include <random>
#include <algorithm>
#include <ctime>
#include <unordered_map>

static ProgressSession createSession(std::mt19937& rng, int daysAgo,
                                     int minCorrect, int maxCorrect,
                                     int minWrong, int maxWrong)
{
    ProgressSession session;

    time_t now = time(nullptr);
    struct tm* tmNow = localtime(&now);
    struct tm tm = *tmNow;
    tm.tm_mday -= daysAgo;
    tm.tm_hour = std::uniform_int_distribution<int>(8, 19)(rng);
    tm.tm_min = std::uniform_int_distribution<int>(0, 59)(rng);
    tm.tm_sec = 0;

    int64_t startTime = (int64_t)mktime(&tm) * 1000;
    int64_t duration = std::uniform_int_distribution<int>(10, 30)(rng) * 60 * 1000;

    int correct = std::uniform_int_distribution<int>(minCorrect, maxCorrect)(rng);
    int wrong = std::uniform_int_distribution<int>(minWrong, maxWrong)(rng);
    int total = correct + wrong;

    session.setStartTimestamp(startTime);
    session.setEndTimestamp(startTime + duration);
    session.setCorrectAnswers(correct);
    session.setWrongAnswers(wrong);
    session.setTotalAttempts(total);

    return session;
}

std::vector<ProgressSession> TestDataGenerator::generateTestSessions() {
    std::mt19937 rng(std::random_device{}());

    std::vector<ProgressSession> sessions;
    sessions.push_back(createSession(rng, 0, 8, 10, 0, 2));
    sessions.push_back(createSession(rng, 1, 6, 9, 1, 4));
    sessions.push_back(createSession(rng, 7, 7, 10, 0, 3));
    sessions.push_back(createSession(rng, 14, 5, 8, 2, 5));
    sessions.push_back(createSession(rng, 30, 4, 7, 3, 6));
    sessions.push_back(createSession(rng, 60, 3, 6, 4, 7));

    for (auto& session : sessions) {
        std::unordered_map<int, int> multCorrect, multWrong, divCorrect, divWrong;
        for (int s = 1; s <= 10; s++) {
            multCorrect[s] = 0;
            multWrong[s] = 0;
            divCorrect[s] = 0;
            divWrong[s] = 0;
        }

        int remainingCorrect = session.getCorrectAnswers();
        int remainingWrong = session.getWrongAnswers();

        std::vector<int> seriesIndices;
        for (int s = 1; s <= 10; s++) seriesIndices.push_back(s);
        std::shuffle(seriesIndices.begin(), seriesIndices.end(), rng);

        int maxSeries = std::min(remainingCorrect, 10);
        for (int idx = 0; idx < maxSeries && remainingCorrect > 0; idx++) {
            int series = seriesIndices[idx];
            bool isMult = rng() % 2 == 0;
            int val = std::min((int)(rng() % 2 + 1), remainingCorrect);
            if (isMult) multCorrect[series] += val;
            else divCorrect[series] += val;
            remainingCorrect -= val;
        }
        if (remainingCorrect > 0) {
            bool isMult = rng() % 2 == 0;
            if (isMult) multCorrect[seriesIndices[0]] += remainingCorrect;
            else divCorrect[seriesIndices[0]] += remainingCorrect;
        }

        maxSeries = std::min(remainingWrong, 10);
        for (int idx = 0; idx < maxSeries && remainingWrong > 0; idx++) {
            int series = seriesIndices[idx];
            bool isMult = rng() % 2 == 0;
            int val = std::min(1, remainingWrong);
            if (isMult) multWrong[series] += val;
            else divWrong[series] += val;
            remainingWrong -= val;
        }
        if (remainingWrong > 0) {
            bool isMult = rng() % 2 == 0;
            if (isMult) multWrong[seriesIndices[0]] += remainingWrong;
            else divWrong[seriesIndices[0]] += remainingWrong;
        }

        session.setSeriesStats(multCorrect, multWrong, divCorrect, divWrong);
    }

    return sessions;
}
