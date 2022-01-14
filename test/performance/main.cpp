#include <iostream>
#include "functions.h"
#include "time_measure.h"
#include "error.h"


int main(int argc, char*argv[]) {
    if (argc != 2) {
        std:: cout << "Wrong number of command line arguments" << std:: endl;
        return -1;
    }
    try {
        Configuration conf{argv[1]};
        Integral integral{conf, djongs_func};
        measure_time(integral);
    } catch (ParsingError& error) {
        return -1;
    }

    return 0;
}
