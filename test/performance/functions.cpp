//
// Created by msemc on 19.02.2021.
//
#include <cmath>

double djongs_func(double x1, double x2) {
    double result = 0.002;
    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            result += 1 / (5*(i + 2) + j + 3 + pow(x1 - 16*j, 6) + pow(x2 - 16*i, 6));
        }
    }
    return 1 / result;
}

double shuberts_func(double x1, double x2) {
    double temp1 = 0, temp2 = 0;
    for (int i = 1; i <=5; i++) {
        temp1 += i*cos((i + 1)*x1 + 1);
        temp2 += i*cos((i + 1)*x2 + 1);
    }
    return -temp1*temp2;
}