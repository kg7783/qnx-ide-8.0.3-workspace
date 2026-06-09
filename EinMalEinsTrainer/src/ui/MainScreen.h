#pragma once

#include "Screen.h"
#include "ProblemView.h"
#include "NumberPadView.h"
#include "StatsView.h"
#include "../core/Trainer.h"
#include "../core/ProgressSession.h"
#include <memory>

class MainScreen : public Screen {
public:
    MainScreen(ScreenManager* mgr);
    ~MainScreen() override = default;

    void onEnter() override;
    void onLeave() override;
    void onDraw(screen_context_t ctx, screen_buffer_t buf) override;
    void onEvent(int type, int x, int y, int key) override;

private:
    ProblemView problemViewMult;
    ProblemView problemViewDiv;
    NumberPadView numberPad;
    StatsView statsView;

    Trainer trainer;
    ProgressSession session;

    ProblemView* activeProblem = nullptr;
    bool autoSwitch = false;

    void onNumberPressed(int digit);
    void onDelete();
    void onOk();
    void nextProblemMult();
    void nextProblemDiv();
    void setActiveProblem(ProblemView* pv);
    void switchToOtherProblem();
    void finishSession();
};
