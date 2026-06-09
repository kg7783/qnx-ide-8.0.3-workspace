#include "SessionAdapter.h"
#include "StatsFilterHelper.h"

std::vector<SessionDisplayData> SessionAdapter::buildDisplayList(
    const std::vector<ProgressSession>& sessions)
{
    std::vector<SessionDisplayData> result;
    for (const auto& s : sessions) {
        SessionDisplayData d;
        d.dateTime = StatsFilterHelper::formatDateTime(s.getStartTimestamp());
        d.correct = s.getCorrectAnswers();
        d.wrong = s.getWrongAnswers();
        d.multTotal = s.getTotalMult();
        d.divTotal = s.getTotalDiv();
        d.accuracy = s.getAccuracy();
        d.sessionId = s.getId();
        result.push_back(d);
    }
    return result;
}
