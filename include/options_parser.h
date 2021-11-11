#ifndef MYCAT_CONFIG_FILE_H
#define MYCAT_CONFIG_FILE_H

#include <boost/program_options.hpp>
#include <string>
#include <iostream>
#include <fstream>

class command_line_options {
public:
    command_line_options();

    ~command_line_options() = default;

    [[nodiscard]] std::vector<std::string> get_filenames() const { return filenames; };

    [[nodiscard]] bool get_A() const { return A; };

    static std::string assert_file_exist(const std::string &f_name);

    void parse(int ac, char **av);

private:
    bool A = false;
    std::vector<std::string> filenames;


    void init_opt_description();

    boost::program_options::variables_map var_map{};
    boost::program_options::options_description opt_conf{
            "Config File Options:\n\tmycat [-h|--help] [-A] <file1> <file2> ... <fileN>\n"};
};

#endif //MYCAT_CONFIG_FILE_H

