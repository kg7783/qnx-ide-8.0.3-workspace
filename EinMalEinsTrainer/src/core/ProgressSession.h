#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

class ProgressSession {
public:
    ProgressSession();
    ProgressSession(int correct, int wrong, int total);

    const std::string& getId() const { return id; }
    int64_t getStartTimestamp() const { return startTimestamp; }
    void setStartTimestamp(int64_t t) { startTimestamp = t; }
    int64_t getEndTimestamp() const { return endTimestamp; }
    void setEndTimestamp(int64_t t) { endTimestamp = t; }

    int getCorrectAnswers() const { return correctAnswers; }
    void setCorrectAnswers(int c) { correctAnswers = c; }
    int getWrongAnswers() const { return wrongAnswers; }
    void setWrongAnswers(int w) { wrongAnswers = w; }
    int getTotalAttempts() const { return totalAttempts; }
    void setTotalAttempts(int t) { totalAttempts = t; }

    double getAccuracy() const;

    std::unordered_map<int, int>& getMultCorrect() { return multCorrect; }
    std::unordered_map<int, int>& getMultWrong() { return multWrong; }
    std::unordered_map<int, int>& getDivCorrect() { return divCorrect; }
    std::unordered_map<int, int>& getDivWrong() { return divWrong; }

    const std::unordered_map<int, int>& getMultCorrect() const { return multCorrect; }
    const std::unordered_map<int, int>& getMultWrong() const { return multWrong; }
    const std::unordered_map<int, int>& getDivCorrect() const { return divCorrect; }
    const std::unordered_map<int, int>& getDivWrong() const { return divWrong; }

    int getMultCorrectForSeries(int series) const;
    int getMultWrongForSeries(int series) const;
    int getDivCorrectForSeries(int series) const;
    int getDivWrongForSeries(int series) const;

    void setSeriesStats(const std::unordered_map<int, int>& multC,
                        const std::unordered_map<int, int>& multW,
                        const std::unordered_map<int, int>& divC,
                        const std::unordered_map<int, int>& divW);

    int getTotalMult() const;
    int getTotalDiv() const;

private:
    std::string id;
    int64_t startTimestamp;
    int64_t endTimestamp;
    int correctAnswers;
    int wrongAnswers;
    int totalAttempts;

    std::unordered_map<int, int> multCorrect;
    std::unordered_map<int, int> multWrong;
    std::unordered_map<int, int> divCorrect;
    std::unordered_map<int, int> divWrong;

    void initMaps();
};
