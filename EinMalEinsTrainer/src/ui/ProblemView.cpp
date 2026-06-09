#include "ProblemView.h"
#include "Font.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

ProblemView::ProblemView(Rect bounds)
    : Widget(bounds)
{
}

void ProblemView::setProblem(const MathProblem& problem) {
    cancelTimer();
    currentProblem = problem;
    answerText.clear();
    scale = 1.0f;
    bgColor = 0xFFFFFFFF;
    animating = false;
    showMark = false;
    timerExpired = false;
    answerChecked = false;
}

void ProblemView::appendDigit(int digit) {
    if (answerText.size() < 4) {
        answerText += (char)('0' + digit);
        startTimer();
    }
}

void ProblemView::deleteDigit() {
    if (!answerText.empty()) {
        answerText.pop_back();
        startTimer();
    }
}

void ProblemView::setAlignedLayout(int gx, int maxN1W, int maxN2W, int iw) {
    alignGroupStartX = gx;
    alignMaxNum1W = maxN1W;
    alignMaxNum2W = maxN2W;
    alignInputW = iw;
}

void ProblemView::clearAnswer() {
    answerText.clear();
    wasCorrect = false;
    answerChecked = false;
    showMark = false;
    animating = false;
    scale = 1.0f;
    timerExpired = false;
}

bool ProblemView::checkAnswer() {
    if (currentProblem.num2 == 0 && answerText.empty()) return false;
    if (answerText.empty()) return false;

    int answer = std::stoi(answerText);
    wasCorrect = (answer == currentProblem.result);
    answerChecked = true;
    startAnimation(wasCorrect);
    return wasCorrect;
}

void ProblemView::setActive(bool a) {
    active = a;
}

void ProblemView::startAnimation(bool correct) {
    animating = true;
    showMark = true;
    animProgress = 0.0f;
    startTimer(1200);
}

void ProblemView::updateAnimation() {
    if (!animating) return;
    animProgress += 0.05f;
    if (animProgress >= 1.0f) {
        animating = false;
    }
}

void ProblemView::startTimer(int timeoutMs) {
    timerTimeoutMs = timeoutMs;
    timerStart = std::chrono::steady_clock::now();
    timerRunning = true;
    timerExpired = false;
}

void ProblemView::cancelTimer() {
    timerRunning = false;
    timerExpired = false;
}

void ProblemView::updateTimer() {
    if (!timerRunning) return;
    if (answerText.empty()) return;

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - timerStart).count();
    if (elapsed >= timerTimeoutMs) {
        timerExpired = true;
        timerRunning = false;
    }
}

void ProblemView::drawProblemText(screen_context_t ctx, screen_buffer_t buf, int x, int y) {
    if (currentProblem.num2 == 0 && currentProblem.num1 == 0) return;

    char num1[8], num2[8], op[2] = {currentProblem.operation, 0};
    snprintf(num1, sizeof(num1), "%d", currentProblem.num1);
    snprintf(num2, sizeof(num2), "%d", currentProblem.num2);

    int textColor = 0xFF000000;
    int opColor = 0xFF323232;

    int cx = bounds.x + bounds.w / 2;
    int cy = bounds.y + 10;
    int totalW = Font::stringWidth(num1) + Font::stringWidth(" ") +
                 Font::CHAR_WIDTH + Font::stringWidth(" ") +
                 Font::stringWidth(num2) + Font::stringWidth(" ") +
                 Font::stringWidth("=") + Font::stringWidth(" ");

    int sx = cx - totalW / 2;
    Font::drawString(ctx, buf, sx, cy, num1, textColor);
    sx += Font::stringWidth(num1) + Font::stringWidth(" ");
    Font::drawString(ctx, buf, sx, cy, op, opColor);
    sx += Font::CHAR_WIDTH + Font::stringWidth(" ");
    Font::drawString(ctx, buf, sx, cy, num2, textColor);
    sx += Font::stringWidth(num2) + Font::stringWidth(" ");
    Font::drawString(ctx, buf, sx, cy, "=", opColor);
}

void ProblemView::drawInputBox(screen_context_t ctx, screen_buffer_t buf, int x, int y, int w, int h) {
    int inputBg = 0xFFE6E6FA;
    int inputBorderActive = 0xFF0066CC;
    int inputBorder = 0xFF6464C8;

    int fill[] = {
        SCREEN_BLIT_DESTINATION_X, x,
        SCREEN_BLIT_DESTINATION_Y, y,
        SCREEN_BLIT_DESTINATION_WIDTH, w,
        SCREEN_BLIT_DESTINATION_HEIGHT, h,
        SCREEN_BLIT_COLOR, inputBg,
        SCREEN_BLIT_END
    };
    screen_fill(ctx, buf, fill);

    int border = active ? inputBorderActive : inputBorder;
    int borderW = active ? 3 : 2;
    for (int i = 0; i < borderW; i++) {
        int bdr[] = {
            SCREEN_BLIT_DESTINATION_X, x + i,
            SCREEN_BLIT_DESTINATION_Y, y + i,
            SCREEN_BLIT_DESTINATION_WIDTH, w - i * 2,
            SCREEN_BLIT_DESTINATION_HEIGHT, 1,
            SCREEN_BLIT_COLOR, border,
            SCREEN_BLIT_END
        };
        screen_fill(ctx, buf, bdr);
        bdr[3] = y + h - 1 - i;
        screen_fill(ctx, buf, bdr);
        int bdr2[] = {
            SCREEN_BLIT_DESTINATION_X, x + i,
            SCREEN_BLIT_DESTINATION_Y, y + i,
            SCREEN_BLIT_DESTINATION_WIDTH, 1,
            SCREEN_BLIT_DESTINATION_HEIGHT, h - i * 2,
            SCREEN_BLIT_COLOR, border,
            SCREEN_BLIT_END
        };
        screen_fill(ctx, buf, bdr2);
        bdr2[1] = x + w - 1 - i;
        screen_fill(ctx, buf, bdr2);
    }

    std::string display = answerText.empty() ? "_" : answerText;
    int tx = x + (w - Font::stringWidth(display)) / 2;
    int ty = y + (h - Font::CHAR_HEIGHT) / 2;
    Font::drawString(ctx, buf, tx, ty, display, 0xFF000000);
}

void ProblemView::draw(screen_context_t ctx, screen_buffer_t buf) {
    if (!visible) return;

    updateAnimation();
    updateTimer();

    int w = bounds.w;
    int h = bounds.h;

    int bgRect[] = {
        SCREEN_BLIT_DESTINATION_X, bounds.x,
        SCREEN_BLIT_DESTINATION_Y, bounds.y,
        SCREEN_BLIT_DESTINATION_WIDTH, w,
        SCREEN_BLIT_DESTINATION_HEIGHT, h,
        SCREEN_BLIT_COLOR, static_cast<int>(0xFFFFFFFF),
        SCREEN_BLIT_END
    };
    screen_fill(ctx, buf, bgRect);

    if (active) {
        int ab[] = {
            SCREEN_BLIT_DESTINATION_X, bounds.x,
            SCREEN_BLIT_DESTINATION_Y, bounds.y,
            SCREEN_BLIT_DESTINATION_WIDTH, w,
            SCREEN_BLIT_DESTINATION_HEIGHT, 4,
            SCREEN_BLIT_COLOR, static_cast<int>(0xFF0066CC),
            SCREEN_BLIT_END
        };
        screen_fill(ctx, buf, ab);
        ab[3] = bounds.y + h - 4;
        screen_fill(ctx, buf, ab);
        ab[5] = 4;
        ab[7] = h;
        ab[1] = bounds.x;
        ab[3] = bounds.y;
        screen_fill(ctx, buf, ab);
        ab[1] = bounds.x + w - 4;
        screen_fill(ctx, buf, ab);
    }

    if (currentProblem.num2 == 0 && currentProblem.num1 == 0) return;

    int savedScale = Font::renderScale;
    int savedSpacing = Font::letterSpacing;
    Font::renderScale = 2;
    Font::letterSpacing = 3;
    int s = Font::renderScale;
    int inputW = alignGroupStartX >= 0 ? alignInputW : Font::stringWidth("888") + 12;
    int inputH = Font::CHAR_HEIGHT * s + 4;
    int inputX;
    int textY = bounds.y + (h - Font::CHAR_HEIGHT * s) / 2;
    int inputY = textY - 2;

    char num1[8], num2[8], op[2] = {currentProblem.operation, 0};
    snprintf(num1, sizeof(num1), "%d", currentProblem.num1);
    snprintf(num2, sizeof(num2), "%d", currentProblem.num2);

    int textColor = 0xFF000000;
    int opColor = 0xFF323232;

    if (alignGroupStartX >= 0) {
        int charW = Font::CHAR_WIDTH * s;
        int spaceW = Font::stringWidth(" ");
        int e0 = 0;
        int e1 = e0 + alignMaxNum1W;
        int e2 = e1 + spaceW;
        int e3 = e2 + charW;
        int e4 = e3 + spaceW;
        int e5 = e4 + alignMaxNum2W;
        int e6 = e5 + spaceW;
        int e7 = e6 + charW;
        int e8 = e7 + spaceW;
        int e9 = e8 + alignInputW;

        int n1x = alignGroupStartX + e1 - Font::stringWidth(num1);
        Font::drawString(ctx, buf, n1x, textY, num1, textColor);
        Font::drawString(ctx, buf, alignGroupStartX + e2, textY, op, opColor);
        int n2x = alignGroupStartX + e5 - Font::stringWidth(num2);
        Font::drawString(ctx, buf, n2x, textY, num2, textColor);
        Font::drawString(ctx, buf, alignGroupStartX + e6, textY, "=", opColor);
        inputX = alignGroupStartX + e8;
    } else {
        int cx = bounds.x + 10;
        Font::drawString(ctx, buf, cx, textY, num1, textColor);
        cx += Font::stringWidth(num1) + Font::stringWidth(" ");
        Font::drawString(ctx, buf, cx, textY, op, opColor);
        cx += Font::CHAR_WIDTH + Font::stringWidth(" ");
        Font::drawString(ctx, buf, cx, textY, num2, textColor);
        cx += Font::stringWidth(num2) + Font::stringWidth(" ");
        Font::drawString(ctx, buf, cx, textY, "=", opColor);
        inputX = bounds.x + bounds.w - inputW - 12;
    }

    int boxBg = 0xFFE6E6FA;
    int boxBorder = active ? 0xFF0066CC : 0xFF6464C8;
    if (showMark) {
        boxBg = wasCorrect ? 0xFF90EE90 : 0xFFFFA07A;
        boxBorder = wasCorrect ? 0xFF228B22 : 0xFFDC143C;
    }

    int fill[] = {
        SCREEN_BLIT_DESTINATION_X, inputX,
        SCREEN_BLIT_DESTINATION_Y, inputY,
        SCREEN_BLIT_DESTINATION_WIDTH, inputW,
        SCREEN_BLIT_DESTINATION_HEIGHT, inputH,
        SCREEN_BLIT_COLOR, boxBg,
        SCREEN_BLIT_END
    };
    screen_fill(ctx, buf, fill);

    int borderW = active ? 2 : 1;
    for (int i = 0; i < borderW; i++) {
        int bdr[] = {
            SCREEN_BLIT_DESTINATION_X, inputX + i,
            SCREEN_BLIT_DESTINATION_Y, inputY + i,
            SCREEN_BLIT_DESTINATION_WIDTH, inputW - i * 2,
            SCREEN_BLIT_DESTINATION_HEIGHT, 1,
            SCREEN_BLIT_COLOR, boxBorder,
            SCREEN_BLIT_END
        };
        screen_fill(ctx, buf, bdr);
        bdr[3] = inputY + inputH - 1 - i;
        screen_fill(ctx, buf, bdr);
        int bdr2[] = {
            SCREEN_BLIT_DESTINATION_X, inputX + i,
            SCREEN_BLIT_DESTINATION_Y, inputY + i,
            SCREEN_BLIT_DESTINATION_WIDTH, 1,
            SCREEN_BLIT_DESTINATION_HEIGHT, inputH - i * 2,
            SCREEN_BLIT_COLOR, boxBorder,
            SCREEN_BLIT_END
        };
        screen_fill(ctx, buf, bdr2);
        bdr2[1] = inputX + inputW - 1 - i;
        screen_fill(ctx, buf, bdr2);
    }

    std::string display = answerText.empty() ? "_" : answerText;
    int tx = inputX + (inputW - Font::stringWidth(display)) / 2;
    int ty = inputY + (inputH - Font::stringHeight()) / 2;
    Font::drawString(ctx, buf, tx, ty, display, 0xFF000000);

    Font::renderScale = savedScale;
    Font::letterSpacing = savedSpacing;
}

bool ProblemView::handleEvent(int type, int x, int y, int key) {
    if (!visible || !enabled) return false;
    if (type == 0 && bounds.contains(x, y)) {
        if (onClick) onClick();
        return true;
    }
    return false;
}
