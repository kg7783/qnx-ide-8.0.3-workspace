#pragma once

#include <string>
#include <vector>
#include "ProgressSession.h"

struct SessionDisplayData {
    std::string dateTime;
    int correct;
    int wrong;
    int multTotal;
    int divTotal;
    double accuracy;
    std::string sessionId;
};

class SessionAdapter {
public:
    static std::vector<SessionDisplayData> buildDisplayList(
        const std::vector<ProgressSession>& sessions);
};
