    //
// Created by msemc on 20.02.2021.
//

#include "config.h"
#include "error.h"
#include <fstream>

Configuration:: Configuration(const std::string& file_name) {
    std::ifstream conf_file{file_name};
    if (!conf_file) {
        throw ParsingError();
    }
    std::stringstream conf_stream{};
    conf_stream << conf_file.rdbuf();
    conf_stream >> absolute_error >> relative_error >> x1 >> x2 >> y1 >> y2 >> threads >> max_iterations;
    if (conf_stream.fail()) {
        throw ParsingError();
    }
}
