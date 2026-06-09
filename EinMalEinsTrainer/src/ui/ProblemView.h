#pragma once

#include "Widget.h"
#include "../core/MathProblem.h"
#include <functional>
#include <string>
#include <chrono>

class ProblemView : public Widget {
public:
    ProblemView() = default;
    ProblemView(Rect bounds);

    void setProblem(const MathProblem& problem);
    MathProblem getProblem() const { return currentProblem; }

    void appendDigit(int digit);
    void deleteDigit();
    void clearAnswer();
    std::string getAnswerText() const { return answerText; }

    bool checkAnswer();
    bool wasLastCorrect() const { return wasCorrect; }
    bool isAnswerChecked() const { return answerChecked; }

    void setActive(bool active);
    bool isActive() const { return active; }
    bool isAnimating() const { return animating; }

    void setOnClick(std::function<void()> cb) { onClick = cb; }

    void draw(screen_context_t ctx, screen_buffer_t buf) override;
    bool handleEvent(int type, int x, int y, int key) override;

    void startTimer(int timeoutMs = 2000);
    void cancelTimer();
    void updateTimer();

    bool isTimerExpired() const { return timerExpired; }

    void setAlignedLayout(int gx, int maxN1W, int maxN2W, int iw);

private:
    MathProblem currentProblem;
    std::string answerText;
    bool active = false;
    bool animating = false;
    bool wasCorrect = false;
    bool answerChecked = false;

    int bgColor = 0xFFFFFFFF;
    int correctColor = 0xFF228B22;
    int wrongColor = 0xFFDC143C;
    int activeBgColor = 0xFFF0F8FF;

    float scale = 1.0f;
    float animProgress = 0.0f;
    bool showMark = false;
    bool showCheckmark = false;
    int markType = 0;

    std::function<void()> onClick;

    int alignGroupStartX = -1;
    int alignMaxNum1W = 0;
    int alignMaxNum2W = 0;
    int alignInputW = 0;

    std::chrono::steady_clock::time_point timerStart;
    int timerTimeoutMs = 2000;
    bool timerRunning = false;
    bool timerExpired = false;

    void startAnimation(bool correct);
    void updateAnimation();
    void drawProblemText(screen_context_t ctx, screen_buffer_t buf, int x, int y);
    void drawInputBox(screen_context_t ctx, screen_buffer_t buf, int x, int y, int w, int h);
};
