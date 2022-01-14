//
// Created by msemc on 20.02.2021.
//

#include "config.h"
#include "Integral.h"
#include <iostream>

Integral:: Integral(const Configuration &configuration, double (*func)(double, double)):
        conf{configuration},
        function{func}
{
}

double Integral:: get_rectangle_value(double x1, double x2, double y1, double y2) {
    double dx = std::abs(x1 - x2);
    double dy = std::abs (y1 - y2);
    return function((x1 + x2) / 2, (y1 + y2) / 2)*dx*dy;
}

double Integral:: calc_integral_sync() {
    double prev = 0, curr = get_rectangle_value(conf.x1, conf.x2, conf.y1, conf.y2);
    double step_x, step_y, temp_x1, temp_y1;
    int iter = 1, partitions = 2;
    while (std::abs(curr - prev) > conf.absolute_error) {
        prev = curr, curr = 0;
        step_x = (conf.x2 - conf.x1) / partitions;
        step_y = (conf.y2 - conf.y1) / partitions;
        temp_x1 = conf.x1, temp_y1 = conf.y1;
        for (int i = 0; i < partitions; i++) {
            for (int j = 0; j < partitions; j++) {
                curr += get_rectangle_value(temp_x1, temp_x1 + step_x, temp_y1, temp_y1 + step_y);
                temp_y1 += step_y;
            }
            temp_x1 += step_x;
            temp_y1 = conf.y1;
        }
        partitions *= 2;
    }
    std::cout << "Absolute error: " << std::abs(curr - prev) << "; " << "Relative error: "
              << std::abs(curr - prev) / curr << std:: endl;
    std:: cout << "Result: " << curr << std::endl;
    return curr;
}

void Integral:: process_unit(double &result, std::mutex &m, double x1, double x2, double y1, int partitions) {
    double temp = 0, step_y = (conf.y2 - conf.y1) / partitions;
    for (int j = 0; j < partitions; j++) {
        temp += get_rectangle_value(x1, x2, y1, y1 + step_y);
        y1 += step_y;
    }
    std::lock_guard<std::mutex> lg{m};
    result += temp;
}

double Integral:: calc_integral_parallel() {
    std::mutex m;
    std::queue<std::thread> q;
    double prev = 0, curr = get_rectangle_value(conf.x1, conf.x2, conf.y1, conf.y2);
    double step_x, temp_x1;
    int iter = 1, partitions = 2;
    while (std::abs(curr - prev) > conf.absolute_error && iter < conf.max_iterations) {
        prev = curr;
        curr = 0;
        step_x = (conf.x2 - conf.x1) / partitions;
        temp_x1 = conf.x1;
        for (int i = 0; i < partitions; i++) {
            std::thread t{&Integral::process_unit, this, std::ref(curr), std::ref(m), temp_x1, temp_x1 + step_x, conf.y1, partitions};
            q.push(std::move(t));
//            Check if thread pool is not full
            if (q.size() == conf.threads) {
                q.front().join();
                q.pop();
            }
            temp_x1 += step_x;
        }
        while (!q.empty()) {
            q.front().join();
            q.pop();
        }
        partitions *= 2;
    }
    std::cout << "Absolute error: " << std::abs(curr - prev) << "; " << "Relative error: "
    << std::abs(curr - prev) / curr << std:: endl;
    std:: cout << "Result: " << curr << std::endl;
    return curr;
}

double Integral:: calculate() {
    if (conf.threads == 1) {
        return calc_integral_sync();
    } else {
        return calc_integral_parallel();
    }
}

void Integral::increment_threads() {
    conf.threads++;
}
