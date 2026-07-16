#include "cli_parser.h"
#include <iostream>

cli_options cli_parser::parse(int argc, char* argv[]) {
    return parse(std::vector<std::string>(argv, argv + argc));
}

cli_options cli_parser::parse(const std::vector<std::string>& args) {

    cli_options options;

    if (args.size() < 2) {
        options.error = "No input or command given.";
        return options;
    }

    const std::string& first = args[1];

    if (first == "help" || first == "--help" || first == "-h") {
        options.command = "help";
        options.valid = true;
        return options;
    }

    if (first == "extract" || first == "list" || first == "verify") {
        options.command = first;
    }
    else if (!first.empty() && first[0] == '-') {
        options.error = "Unknown option: " + first;
        return options;
    }
    else {
        options.command = "zb";
        options.input = first;
    }

    for (size_t i = 2; i < args.size(); i++) {
        const std::string& arg = args[i];

        if (arg == "-o" || arg == "--output") {
            if (i + 1 >= args.size()) {
                options.error = "Missing value after " + arg;
                return options;
            }
            options.output = args[++i];
        }
        else if (arg == "-a" || arg == "--algorithm") {
            if (i + 1 >= args.size()) {
                options.error = "Missing value after " + arg;
                return options;
            }
            options.algorithm = args[++i];
        }
        else if (!arg.empty() && arg[0] == '-') {
            options.error = "Unknown option: " + arg;
            return options;
        }
        else if (options.input.empty()) {
            options.input = arg;
        }
        else {
            options.error = "Unexpected argument: " + arg;
            return options;
        }
    }

    if (options.input.empty()) {
        options.error = "Missing input path for command: " + options.command;
        return options;
    }

    if (options.algorithm != "auto") {
        CompressionType ignored;
        if (!algorithmFromName(options.algorithm, ignored)) {
            options.error = "Unknown algorithm: " + options.algorithm +
                            " (expected auto, rle, lz77, huffman or none)";
            return options;
        }
    }

    options.valid = true;
    return options;
}

void cli_parser::printUsage() {
    std::cout <<
        "zb - ZipBike compression tool\n"
        "\n"
        "Usage:\n"
        "  zb <file|folder> [-o <output.zbik>] [-a <algorithm>]\n"
        "  zb extract <archive.zbik> [-o <output-dir>]\n"
        "  zb list <archive.zbik>\n"
        "  zb verify <archive.zbik>\n"
        "  zb help\n"
        "\n"
        "Commands:\n"
        "  <file|folder>  Compress a file or a whole folder into a .zbik archive\n"
        "  extract        Restore the contents of a .zbik archive\n"
        "  list           Show what is inside a .zbik archive\n"
        "  verify         Check that every entry in an archive decompresses cleanly\n"
        "\n"
        "Options:\n"
        "  -o, --output     Output path (archive when compressing, folder for extract)\n"
        "  -a, --algorithm  auto (default), rle, lz77, huffman, none\n";
}

bool cli_parser::algorithmFromName(const std::string& name, CompressionType& type) {
    if (name == "rle") {
        type = RLE;
    }
    else if (name == "lz77") {
        type = LZ77;
    }
    else if (name == "huffman") {
        type = HUFFMAN;
    }
    else if (name == "none") {
        type = NONE;
    }
    else {
        return false;
    }
    return true;
}
