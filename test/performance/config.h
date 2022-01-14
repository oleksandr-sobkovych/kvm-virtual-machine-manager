//
// Created by msemc on 20.02.2021.
//

#ifndef LAB_SECOND_INTEGRAL_CONFIG_H
#define LAB_SECOND_INTEGRAL_CONFIG_H
#include <cstdio>
#include <sstream>
#include <string>

struct Configuration {
    double absolute_error;
    double relative_error;
    double x1, x2, y1, y2;
    std::size_t max_iterations;
    int threads;
public:
    Configuration(const std::string& file_name);
};
#endif //LAB_SECOND_INTEGRAL_CONFIG_H
