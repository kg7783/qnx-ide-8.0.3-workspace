#pragma once

#include <unordered_map>
#include <set>
#include <random>
#include "MathProblem.h"
#include "EinmaleinsConfig.h"

class Trainer {
public:
    Trainer();

    void setConfig(const EinmaleinsConfig& config);
    const EinmaleinsConfig& getConfig() const { return config; }

    void generateNewProblems();
    MathProblem generateMultiplicationProblem();
    MathProblem generateDivisionProblem();

    MathProblem getMultiplicationProblem() const;
    MathProblem getDivisionProblem() const;

    bool hasValidMultiplicationProblem() const;
    bool hasValidDivisionProblem() const;

    void recordCorrectAnswer();
    void recordCorrectAnswer(int series, bool isDivision);
    void recordWrongAnswer();
    void recordWrongAnswer(int series, bool isDivision);

    int getCorrectAnswers() const { return correctAnswers; }
    int getWrongAnswers() const { return wrongAnswers; }
    int getTotalAttempts() const { return totalAttempts; }

    void resetStats();
    void setStats(int correct, int wrong, int total);

    void setSeriesStats(const std::unordered_map<int, int>& multC,
                        const std::unordered_map<int, int>& multW,
                        const std::unordered_map<int, int>& divC,
                        const std::unordered_map<int, int>& divW);

    const std::unordered_map<int, int>& getMultCorrect() const { return multCorrect; }
    const std::unordered_map<int, int>& getMultWrong() const { return multWrong; }
    const std::unordered_map<int, int>& getDivCorrect() const { return divCorrect; }
    const std::unordered_map<int, int>& getDivWrong() const { return divWrong; }

private:
    static const int MIN_MULTIPLIER = 1;
    static const int MAX_MULTIPLIER = 10;
    static const int DEFAULT_VALUE = 1;

    std::mt19937 rng;
    EinmaleinsConfig config;

    int multFactor1;
    int multFactor2;
    int divProduct;
    int divDivisor;

    int correctAnswers;
    int wrongAnswers;
    int totalAttempts;

    std::unordered_map<int, int> multCorrect;
    std::unordered_map<int, int> multWrong;
    std::unordered_map<int, int> divCorrect;
    std::unordered_map<int, int> divWrong;

    std::set<int> getValidMultipliers() const;
    int selectRandomFromSet(const std::set<int>& set);
};
