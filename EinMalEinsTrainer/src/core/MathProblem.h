#pragma once

struct MathProblem {
    int num1;
    int num2;
    int result;
    char operation;

    MathProblem() : num1(0), num2(0), result(0), operation('*') {}
    MathProblem(int n1, int n2, int r, char op)
        : num1(n1), num2(n2), result(r), operation(op) {}
};
