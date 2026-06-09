#include "StatisticsCalculator.h"
#include <algorithm>
#include <ctime>

StatisticsCalculator::Statistics StatisticsCalculator::calculate(
    const std::vector<ProgressSession>& sessions,
    TimeFilter filter,
    int64_t customStartDate,
    int64_t customEndDate)
{
    auto filtered = filterSessions(sessions, filter, customStartDate, customEndDate);

    Statistics stats;
    stats.totalSessions = (int)filtered.size();

    if (filtered.empty()) {
        return stats;
    }

    int totalCorrect = 0;
    int totalWrong = 0;
    double totalAccuracy = 0;
    double bestAcc = -1;
    double worstAcc = 101;

    for (const auto& session : filtered) {
        totalCorrect += session.getCorrectAnswers();
        totalWrong += session.getWrongAnswers();

        double accuracy = session.getAccuracy();
        totalAccuracy += accuracy;

        if (accuracy > bestAcc) {
            bestAcc = accuracy;
            stats.bestSession = &session;
        }
        if (accuracy < worstAcc && session.getTotalAttempts() > 0) {
            worstAcc = accuracy;
            stats.worstSession = &session;
        }
    }

    stats.totalAttempts = totalCorrect + totalWrong;
    stats.correctAnswers = totalCorrect;
    stats.wrongAnswers = totalWrong;
    stats.averageAccuracy = filtered.empty() ? 0.0 : totalAccuracy / filtered.size();
    stats.bestAccuracy = bestAcc;
    stats.worstAccuracy = worstAcc;

    return stats;
}

StatisticsCalculator::SeriesStatistics StatisticsCalculator::calculateSeriesStats(
    const std::vector<ProgressSession>& sessions,
    TimeFilter filter,
    int64_t customStartDate,
    int64_t customEndDate)
{
    auto filtered = filterSessions(sessions, filter, customStartDate, customEndDate);

    SeriesStatistics seriesStats;
    for (int i = 1; i <= 10; i++) {
        seriesStats.multCorrect[i] = 0;
        seriesStats.multWrong[i] = 0;
        seriesStats.divCorrect[i] = 0;
        seriesStats.divWrong[i] = 0;
    }

    for (const auto& session : filtered) {
        for (int series = 1; series <= 10; series++) {
            seriesStats.multCorrect[series] += session.getMultCorrectForSeries(series);
            seriesStats.multWrong[series] += session.getMultWrongForSeries(series);
            seriesStats.divCorrect[series] += session.getDivCorrectForSeries(series);
            seriesStats.divWrong[series] += session.getDivWrongForSeries(series);
        }
    }

    for (int i = 1; i <= 10; i++) {
        int multTotal = seriesStats.multCorrect[i] + seriesStats.multWrong[i];
        seriesStats.multAccuracy[i] = multTotal > 0
            ? (double)seriesStats.multCorrect[i] / multTotal * 100.0 : 0.0;

        int divTotal = seriesStats.divCorrect[i] + seriesStats.divWrong[i];
        seriesStats.divAccuracy[i] = divTotal > 0
            ? (double)seriesStats.divCorrect[i] / divTotal * 100.0 : 0.0;
    }

    return seriesStats;
}

std::vector<ProgressSession> StatisticsCalculator::filterSessions(
    const std::vector<ProgressSession>& sessions,
    TimeFilter filter,
    int64_t customStartDate,
    int64_t customEndDate)
{
    if (filter == TimeFilter::ALL) {
        return sessions;
    }

    time_t now = time(nullptr);
    struct tm* tmNow = localtime(&now);
    struct tm tmStart = *tmNow;
    tmStart.tm_hour = 0;
    tmStart.tm_min = 0;
    tmStart.tm_sec = 0;
    int64_t startOfToday = (int64_t)mktime(&tmStart) * 1000;
    int64_t startOfWeek = startOfToday - (6 * 24 * 60 * 60 * 1000LL);
    int64_t startOfMonth = startOfToday - (29 * 24 * 60 * 60 * 1000LL);

    std::vector<ProgressSession> result;
    for (const auto& session : sessions) {
        int64_t sessionTime = session.getStartTimestamp();
        bool matches = false;

        switch (filter) {
            case TimeFilter::TODAY:
                matches = sessionTime >= startOfToday;
                break;
            case TimeFilter::WEEK:
                matches = sessionTime >= startOfWeek;
                break;
            case TimeFilter::MONTH:
                matches = sessionTime >= startOfMonth;
                break;
            case TimeFilter::CUSTOM:
                if (customStartDate > 0 && customEndDate > 0) {
                    matches = sessionTime >= customStartDate && sessionTime <= customEndDate;
                }
                break;
            default:
                break;
        }

        if (matches) {
            result.push_back(session);
        }
    }

    return result;
}
