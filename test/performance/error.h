//
// Created by msemc on 20.02.2021.
//

#ifndef LAB_SECOND_INTEGRAL_ERROR_H
#define LAB_SECOND_INTEGRAL_ERROR_H

#include <exception>
#include <iostream>

class ParsingError: public std::exception {
public:
    ParsingError(): std::exception{} {
        std:: cout << "Error occurred, while reading configuration" << std:: endl;
    }
};

#endif //LAB_SECOND_INTEGRAL_ERROR_H
