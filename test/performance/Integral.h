//
// Created by msemc on 20.02.2021.
//

#ifndef LAB_SECOND_INTEGRAL_INTEGRAL_H
#define LAB_SECOND_INTEGRAL_INTEGRAL_H

#include <mutex>
#include <thread>
#include <queue>
#include "config.h"

class Integral {
    Configuration conf;
    double (*function)(double x, double y);
private:
    double get_rectangle_value(double x1, double x2, double y1, double y2);
    void process_unit(double& result, std::mutex& m, double x1, double x2, double y1, int partitions);
    double calc_integral_parallel();
    double calc_integral_sync();
public:
    explicit Integral(const Configuration& configuration, double (*func)(double x, double y));
    double calculate();
    void increment_threads();
};

#endif //LAB_SECOND_INTEGRAL_INTEGRAL_H
