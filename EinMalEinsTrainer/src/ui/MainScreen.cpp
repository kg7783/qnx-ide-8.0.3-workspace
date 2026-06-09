#define _POSIX_C_SOURCE 199309L
#include "MainScreen.h"
#include "Font.h"
#include "../persistence/ConfigManager.h"
#include "../persistence/SessionManager.h"
#include <time.h>
#include <cstdio>

MainScreen::MainScreen(ScreenManager* mgr)
    : Screen(mgr), trainer(), session()
{
    int w = mgr->getWidth();
    int h = mgr->getHeight();

    int margin = 4;
    int statH = 45;
    int titleH = 38;
    int probH = 62;
    int gap = 4;
    int numpH = h - statH - titleH - probH * 2 - gap * 5 - margin * 4 - 46;

    int statY = margin;
    int titleY = statY + statH + gap;
    int probY1 = titleY + titleH + gap;
    int probY2 = probY1 + probH + gap;
    int numpY = probY2 + probH + gap;
    int btnY = numpY + numpH + gap;

    problemViewMult = ProblemView(Rect{margin, probY1, w - margin * 2, probH});
    problemViewDiv = ProblemView(Rect{margin, probY2, w - margin * 2, probH});
    numberPad = NumberPadView(Rect{margin, numpY, w - margin * 2, numpH});
    statsView = StatsView(Rect{margin, statY, w - margin * 2, statH});

    problemViewMult.setOnClick([this]() { setActiveProblem(&problemViewMult); });
    problemViewDiv.setOnClick([this]() { setActiveProblem(&problemViewDiv); });
    numberPad.setOnNumberPressed([this](int val) { onNumberPressed(val); });
    numberPad.setOnDelete([this]() { onDelete(); });
    numberPad.setOnOk([this]() { onOk(); });
}

void MainScreen::onEnter() {
    ConfigManager cm;
    auto cfg = cm.loadConfig();
    autoSwitch = cfg.autoSwitchMode;

    MathProblem p1 = trainer.generateMultiplicationProblem();
    problemViewMult.setProblem(p1);
    MathProblem p2 = trainer.generateDivisionProblem();
    problemViewDiv.setProblem(p2);
    setActiveProblem(&problemViewMult);
}

void MainScreen::onLeave() {
    finishSession();
}

void MainScreen::setActiveProblem(ProblemView* pv) {
    if (activeProblem && activeProblem != pv) {
        activeProblem->setActive(false);
    }
    activeProblem = pv;
    if (activeProblem) {
        activeProblem->setActive(true);
    }
}

void MainScreen::nextProblemMult() {
    MathProblem problem = trainer.generateMultiplicationProblem();
    problemViewMult.setProblem(problem);
}

void MainScreen::nextProblemDiv() {
    MathProblem problem = trainer.generateDivisionProblem();
    problemViewDiv.setProblem(problem);
}

void MainScreen::onNumberPressed(int digit) {
    if (activeProblem) activeProblem->appendDigit(digit);
}

void MainScreen::onDelete() {
    if (activeProblem) activeProblem->deleteDigit();
}

void MainScreen::onOk() {
    if (!activeProblem) return;
    if (activeProblem->getAnswerText().empty()) return;

    bool correct = activeProblem->checkAnswer();
    int series = activeProblem->getProblem().num2;
    bool isDiv = (activeProblem->getProblem().operation == '/');

    if (correct) {
        trainer.recordCorrectAnswer(series, isDiv);
        session.setCorrectAnswers(session.getCorrectAnswers() + 1);
    } else {
        trainer.recordWrongAnswer(series, isDiv);
        session.setWrongAnswers(session.getWrongAnswers() + 1);
    }
    session.setTotalAttempts(session.getTotalAttempts() + 1);
}

void MainScreen::onEvent(int type, int x, int y, int key) {
    if (key > 0) {
        if (key >= '0' && key <= '9') {
            onNumberPressed(key - '0');
        } else if (key == KEY_DEL || key == KEY_BSP) {
            onDelete();
        } else if (key == '\n' || key == '\r') {
            onOk();
        } else if (key == 's' || key == 'S') {
            mgr->pushScreen("setup");
        } else if (key == 'h' || key == 'H') {
            mgr->pushScreen("history");
        } else if (key == 'n' || key == 'N') {
            trainer.resetStats();
            session = ProgressSession();
            onEnter();
        }
        return;
    }

    int w = mgr->getWidth();
    int h = mgr->getHeight();
    int margin = 4;
    int btnH = 46;
    int btnY = h - margin - btnH;
    int btnW = (w - margin * 4) / 3;

    if (y >= btnY && y < btnY + btnH) {
        if (type == 0 || type == 1) {
            int x1 = margin;
            int x2 = x1 + btnW + margin;
            int x3 = x2 + btnW + margin;
            if (x >= x1 && x < x1 + btnW) {
                mgr->pushScreen("setup");
            } else if (x >= x2 && x < x2 + btnW) {
                trainer.resetStats();
                session = ProgressSession();
                onEnter();
            } else if (x >= x3 && x < x3 + btnW) {
                mgr->pushScreen("history");
            }
        }
        return;
    }

    if (numberPad.handleEvent(type, x, y, 0)) return;
    if (problemViewMult.handleEvent(type, x, y, 0)) return;
    if (problemViewDiv.handleEvent(type, x, y, 0)) return;
}

void MainScreen::switchToOtherProblem() {
    if (activeProblem == &problemViewMult) {
        setActiveProblem(&problemViewDiv);
    } else {
        setActiveProblem(&problemViewMult);
    }
}

void MainScreen::finishSession() {
    session.setSeriesStats(trainer.getMultCorrect(), trainer.getMultWrong(),
                           trainer.getDivCorrect(), trainer.getDivWrong());
    int64_t now;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    now = (int64_t)ts.tv_sec * 1000 + (int64_t)ts.tv_nsec / 1000000;
    session.setEndTimestamp(now);
    SessionManager sm;
    sm.addSessions({session});
}

void MainScreen::onDraw(screen_context_t ctx, screen_buffer_t buf) {
    int w = mgr->getWidth();
    int h = mgr->getHeight();

    int bg[] = {
        SCREEN_BLIT_DESTINATION_X, 0,
        SCREEN_BLIT_DESTINATION_Y, 0,
        SCREEN_BLIT_DESTINATION_WIDTH, w,
        SCREEN_BLIT_DESTINATION_HEIGHT, h,
        SCREEN_BLIT_COLOR, static_cast<int>(0xFF87CEEB),
        SCREEN_BLIT_END
    };
    screen_fill(ctx, buf, bg);

    int margin = 4;
    int statH = 45;

    int statY = margin;
    int titleY = statY + statH + 4;

    statsView.draw(ctx, buf);

    std::string title1 = mgr->tr("title_ein_mal_eins");
    std::string title2 = mgr->tr("title_trainer");
    int cx = w / 2;
    int t1x = cx - Font::stringWidth(title1) / 2;
    int t2x = cx - Font::stringWidth(title2) / 2;
    int t1y = titleY + 4;
    Font::drawString(ctx, buf, t1x, t1y, title1, 0xFF000000);
    Font::drawString(ctx, buf, t2x, t1y + 18, title2, 0xFF333333);

    {
        int savedScale = Font::renderScale;
        int savedSpacing = Font::letterSpacing;
        Font::renderScale = 2;
        Font::letterSpacing = 3;

        MathProblem mp = problemViewMult.getProblem();
        MathProblem dp = problemViewDiv.getProblem();
        char m1[8], m2[8], d1[8], d2[8];
        snprintf(m1, sizeof(m1), "%d", mp.num1);
        snprintf(m2, sizeof(m2), "%d", mp.num2);
        snprintf(d1, sizeof(d1), "%d", dp.num1);
        snprintf(d2, sizeof(d2), "%d", dp.num2);

        int maxN1W = std::max(Font::stringWidth(m1), Font::stringWidth(d1));
        int maxN2W = std::max(Font::stringWidth(m2), Font::stringWidth(d2));
        int charW = Font::CHAR_WIDTH * Font::renderScale;
        int spaceW = Font::stringWidth(" ");
        int inputW = Font::stringWidth("888") + 12;
        int totalW = maxN1W + spaceW + charW + spaceW + maxN2W + spaceW + charW + spaceW + inputW;

        auto pv = &problemViewMult;
        int gx = pv->getBounds().x + (pv->getBounds().w - totalW) / 2;
        pv->setAlignedLayout(gx, maxN1W, maxN2W, inputW);

        pv = &problemViewDiv;
        gx = pv->getBounds().x + (pv->getBounds().w - totalW) / 2;
        pv->setAlignedLayout(gx, maxN1W, maxN2W, inputW);

        problemViewMult.draw(ctx, buf);
        problemViewDiv.draw(ctx, buf);

        Font::renderScale = savedScale;
        Font::letterSpacing = savedSpacing;
    }

    numberPad.draw(ctx, buf);

    if (activeProblem && activeProblem->isTimerExpired()) {
        if (!activeProblem->isAnswerChecked() && !activeProblem->getAnswerText().empty()) {
            onOk();
        } else if (activeProblem->isAnswerChecked()) {
            if (activeProblem->wasLastCorrect()) {
                if (activeProblem == &problemViewMult) {
                    nextProblemMult();
                } else {
                    nextProblemDiv();
                }
                if (autoSwitch) {
                    switchToOtherProblem();
                }
            } else {
                activeProblem->clearAnswer();
            }
        }
    }

    statsView.setStats(trainer.getCorrectAnswers(), trainer.getWrongAnswers());

    int btnH = 46;
    int btnY = h - margin - btnH;
    int btnW = (w - margin * 4) / 3;
    int btnColors[3] = {static_cast<int>(0xFF4682B4), static_cast<int>(0xFFFFA500), static_cast<int>(0xFF228B22)};
    int btnXs[3] = {margin, margin + btnW + margin, margin + (btnW + margin) * 2};
    int btnWs[3] = {btnW, btnW, w - margin * 3 - btnW * 2};
    const char* btnLabels[3] = {"btn_settings", "btn_new_session", "btn_history"};

    for (int i = 0; i < 3; i++) {
        int rect[] = {
            SCREEN_BLIT_DESTINATION_X, btnXs[i],
            SCREEN_BLIT_DESTINATION_Y, btnY,
            SCREEN_BLIT_DESTINATION_WIDTH, btnWs[i],
            SCREEN_BLIT_DESTINATION_HEIGHT, btnH,
            SCREEN_BLIT_COLOR, btnColors[i],
            SCREEN_BLIT_END
        };
        screen_fill(ctx, buf, rect);
        std::string label = mgr->tr(btnLabels[i]);
        int lx = btnXs[i] + (btnWs[i] - Font::stringWidth(label)) / 2;
        int ly = btnY + (btnH - Font::stringHeight()) / 2;
        Font::drawString(ctx, buf, lx, ly, label, 0xFFFFFFFF);
    }

}
