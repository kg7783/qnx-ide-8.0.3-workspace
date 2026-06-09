#pragma once

#include <vector>
#include <unordered_map>
#include "ProgressSession.h"

class StatisticsCalculator {
public:
    enum class TimeFilter {
        ALL, TODAY, WEEK, MONTH, CUSTOM
    };

    struct Statistics {
        int totalSessions = 0;
        int totalAttempts = 0;
        int correctAnswers = 0;
        int wrongAnswers = 0;
        double averageAccuracy = 0.0;
        double bestAccuracy = 0.0;
        double worstAccuracy = 0.0;
        const ProgressSession* bestSession = nullptr;
        const ProgressSession* worstSession = nullptr;
    };

    struct SeriesStatistics {
        std::unordered_map<int, double> multAccuracy;
        std::unordered_map<int, double> divAccuracy;
        std::unordered_map<int, int> multCorrect;
        std::unordered_map<int, int> multWrong;
        std::unordered_map<int, int> divCorrect;
        std::unordered_map<int, int> divWrong;
    };

    static Statistics calculate(const std::vector<ProgressSession>& sessions,
                                 TimeFilter filter,
                                 int64_t customStartDate = 0,
                                 int64_t customEndDate = 0);

    static SeriesStatistics calculateSeriesStats(const std::vector<ProgressSession>& sessions,
                                                  TimeFilter filter,
                                                  int64_t customStartDate = 0,
                                                  int64_t customEndDate = 0);

private:
    static std::vector<ProgressSession> filterSessions(
        const std::vector<ProgressSession>& sessions,
        TimeFilter filter,
        int64_t customStartDate,
        int64_t customEndDate);
};
