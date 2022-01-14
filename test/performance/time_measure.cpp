//
// Created by msemc on 21.02.2021.
//

#include "time_measure.h"
#include <iostream>

long long measure_time(Integral& integral) {
    auto start = get_current_time_fenced();
    integral.calculate();
    auto finish = get_current_time_fenced();
    long long res = to_us(finish - start);
    std::cout << "Measured time (microseconds): " << res << std::endl;
    return res;
}