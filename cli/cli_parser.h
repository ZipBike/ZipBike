#ifndef ZIPBIKE_CLI_PARSER_H
#define ZIPBIKE_CLI_PARSER_H

#include <string>
#include <vector>
#include "container.h"

struct cli_options {
    std::string command;
    std::string input;
    std::string output;
    std::string algorithm;
    bool valid;
    std::string error;

    cli_options() : algorithm("auto"), valid(false) {}
};

class cli_parser {
public:

    cli_options parse(int argc, char* argv[]);
    cli_options parse(const std::vector<std::string>& args);

    void printUsage();

    static bool algorithmFromName(const std::string& name, CompressionType& type);
};

#endif
