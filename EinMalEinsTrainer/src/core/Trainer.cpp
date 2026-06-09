#include "Trainer.h"
#include <algorithm>

Trainer::Trainer()
    : rng(std::random_device{}())
    , multFactor1(DEFAULT_VALUE)
    , multFactor2(DEFAULT_VALUE)
    , divProduct(DEFAULT_VALUE)
    , divDivisor(DEFAULT_VALUE)
    , correctAnswers(0)
    , wrongAnswers(0)
    , totalAttempts(0)
{
    for (int i = MIN_MULTIPLIER; i <= MAX_MULTIPLIER; i++) {
        multCorrect[i] = 0;
        multWrong[i] = 0;
        divCorrect[i] = 0;
        divWrong[i] = 0;
    }

    for (int i = MIN_MULTIPLIER; i <= MAX_MULTIPLIER; i++) {
        config.baseNumbers.insert(i);
        for (int j = MIN_MULTIPLIER; j <= MAX_MULTIPLIER; j++) {
            config.multipliers[i].insert(j);
        }
    }

    generateNewProblems();
}

void Trainer::setConfig(const EinmaleinsConfig& cfg) {
    config = cfg;
}

std::set<int> Trainer::getValidMultipliers() const {
    std::set<int> valid;
    for (int i = MIN_MULTIPLIER; i <= MAX_MULTIPLIER; i++) {
        if (!config.multipliers[i].empty()) {
            valid.insert(i);
        }
    }
    return valid;
}

int Trainer::selectRandomFromSet(const std::set<int>& set) {
    if (set.empty()) return DEFAULT_VALUE;
    std::uniform_int_distribution<int> dist(0, (int)set.size() - 1);
    int index = dist(rng);
    auto it = set.begin();
    std::advance(it, index);
    return *it;
}

void Trainer::generateNewProblems() {
    auto validMult = getValidMultipliers();
    if (validMult.empty() || config.baseNumbers.empty()) return;

    int m1 = selectRandomFromSet(validMult);
    int b1 = selectRandomFromSet(config.multipliers[m1]);
    int m2 = selectRandomFromSet(validMult);
    int b2 = selectRandomFromSet(config.multipliers[m2]);

    multFactor1 = b1;
    multFactor2 = m1;
    divProduct = b2 * m2;
    divDivisor = m2;
}

MathProblem Trainer::generateMultiplicationProblem() {
    auto validMult = getValidMultipliers();
    if (validMult.empty() || config.baseNumbers.empty()) return MathProblem();

    int mult = selectRandomFromSet(validMult);
    int base = selectRandomFromSet(config.multipliers[mult]);
    multFactor1 = base;
    multFactor2 = mult;
    return MathProblem(multFactor1, multFactor2, multFactor1 * multFactor2, '*');
}

MathProblem Trainer::generateDivisionProblem() {
    auto validMult = getValidMultipliers();
    if (validMult.empty() || config.baseNumbers.empty()) return MathProblem();

    int mult = selectRandomFromSet(validMult);
    int base = selectRandomFromSet(config.multipliers[mult]);
    divProduct = base * mult;
    divDivisor = mult;
    return MathProblem(divProduct, divDivisor, divProduct / divDivisor, '/');
}

MathProblem Trainer::getMultiplicationProblem() const {
    int f1 = multFactor1 == 0 ? DEFAULT_VALUE : multFactor1;
    int f2 = multFactor2 == 0 ? DEFAULT_VALUE : multFactor2;
    return MathProblem(f1, f2, f1 * f2, '*');
}

MathProblem Trainer::getDivisionProblem() const {
    int d = divDivisor == 0 ? DEFAULT_VALUE : divDivisor;
    int p = d == DEFAULT_VALUE ? d : divProduct;
    return MathProblem(p, d, p / d, '/');
}

bool Trainer::hasValidMultiplicationProblem() const {
    return !getValidMultipliers().empty() && !config.baseNumbers.empty();
}

bool Trainer::hasValidDivisionProblem() const {
    return hasValidMultiplicationProblem();
}

void Trainer::recordCorrectAnswer() {
    correctAnswers++;
    totalAttempts++;
}

void Trainer::recordCorrectAnswer(int series, bool isDivision) {
    recordCorrectAnswer();
    if (isDivision) {
        divCorrect[series] = divCorrect[series] + 1;
    } else {
        multCorrect[series] = multCorrect[series] + 1;
    }
}

void Trainer::recordWrongAnswer() {
    wrongAnswers++;
    totalAttempts++;
}

void Trainer::recordWrongAnswer(int series, bool isDivision) {
    recordWrongAnswer();
    if (isDivision) {
        divWrong[series] = divWrong[series] + 1;
    } else {
        multWrong[series] = multWrong[series] + 1;
    }
}

void Trainer::resetStats() {
    correctAnswers = 0;
    wrongAnswers = 0;
    totalAttempts = 0;
    for (int i = MIN_MULTIPLIER; i <= MAX_MULTIPLIER; i++) {
        multCorrect[i] = 0;
        multWrong[i] = 0;
        divCorrect[i] = 0;
        divWrong[i] = 0;
    }
}

void Trainer::setStats(int correct, int wrong, int total) {
    correctAnswers = correct;
    wrongAnswers = wrong;
    totalAttempts = total;
}

void Trainer::setSeriesStats(const std::unordered_map<int, int>& multC,
                             const std::unordered_map<int, int>& multW,
                             const std::unordered_map<int, int>& divC,
                             const std::unordered_map<int, int>& divW) {
    multCorrect = multC;
    multWrong = multW;
    divCorrect = divC;
    divWrong = divW;
}
