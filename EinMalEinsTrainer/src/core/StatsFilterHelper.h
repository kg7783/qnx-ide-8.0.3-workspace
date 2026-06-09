#pragma once

#include <cstdint>
#include <string>
#include <ctime>

struct DateRangeData {
    int64_t startDate;
    int64_t endDate;
    DateRangeData(int64_t s, int64_t e) : startDate(s), endDate(e) {}
};

class StatsFilterHelper {
public:
    static DateRangeData initDateRange(bool forStats,
                                       int64_t savedStart, int64_t savedEnd);
    static std::string formatDate(int64_t timestampMs);
    static std::string formatDateShort(int64_t timestampMs);
    static std::string formatDateTime(int64_t timestampMs);

    static int64_t getStartOfDay(int daysAgo);
    static int64_t getEndOfDay(int daysAgo);
};
