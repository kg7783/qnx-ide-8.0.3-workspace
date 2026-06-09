#include "StatsFilterHelper.h"
#include <ctime>
#include <cstring>
#include <sstream>
#include <iomanip>

DateRangeData StatsFilterHelper::initDateRange(bool /*forStats*/,
                                                int64_t savedStart, int64_t savedEnd) {
    int64_t start = savedStart;
    int64_t end = savedEnd;
    if (start == 0) start = getStartOfDay(0);
    if (end == 0) end = getEndOfDay(0);
    return DateRangeData(start, end);
}

std::string StatsFilterHelper::formatDate(int64_t timestampMs) {
    if (timestampMs <= 0) return "--";
    time_t sec = timestampMs / 1000;
    struct tm* tm = localtime(&sec);
    char buf[64];
    strftime(buf, sizeof(buf), "%d.%m.%Y", tm);
    return std::string(buf);
}

std::string StatsFilterHelper::formatDateShort(int64_t timestampMs) {
    if (timestampMs <= 0) return "--";
    time_t sec = timestampMs / 1000;
    struct tm* tm = localtime(&sec);
    char buf[64];
    strftime(buf, sizeof(buf), "%d.%m.", tm);
    return std::string(buf);
}

std::string StatsFilterHelper::formatDateTime(int64_t timestampMs) {
    if (timestampMs <= 0) return "...";
    time_t sec = timestampMs / 1000;
    struct tm* tm = localtime(&sec);
    char buf[64];
    strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M", tm);
    return std::string(buf);
}

int64_t StatsFilterHelper::getStartOfDay(int daysAgo) {
    time_t now = time(nullptr);
    struct tm* tmNow = localtime(&now);
    struct tm tm = *tmNow;
    tm.tm_mday -= daysAgo;
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    return (int64_t)mktime(&tm) * 1000;
}

int64_t StatsFilterHelper::getEndOfDay(int daysAgo) {
    time_t now = time(nullptr);
    struct tm* tmNow = localtime(&now);
    struct tm tm = *tmNow;
    tm.tm_mday -= daysAgo;
    tm.tm_hour = 23;
    tm.tm_min = 59;
    tm.tm_sec = 59;
    return (int64_t)mktime(&tm) * 1000;
}
