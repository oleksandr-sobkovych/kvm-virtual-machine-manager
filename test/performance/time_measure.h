//
// Created by msemc on 19.02.2021.
//

#ifndef LAB_SECOND_INTEGRAL_TIME_MEASURE_H
#define LAB_SECOND_INTEGRAL_TIME_MEASURE_H

#include <chrono>
#include <atomic>

#include <cstdio>
#include <sstream>

#include "Integral.h"

template<class D>
inline long long to_us(const D& d) {
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}

inline std::chrono::high_resolution_clock::time_point get_current_time_fenced()
{
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

long long measure_time(Integral& integral);
#endif //LAB_SECOND_INTEGRAL_TIME_MEASURE_H
