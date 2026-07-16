#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif

#include "cli_parser.h"
#include "compressor.h"
#include "filehandler.h"
#include "crc32.h"

namespace fs = std::filesystem;

namespace {

    std::vector<std::string> collectArgs(int argc, char* argv[]) {
#ifdef _WIN32
        int wargc = 0;
        LPWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), &wargc);
        if (wargv != nullptr) {
            std::vector<std::string> args;
            for (int i = 0; i < wargc; i++) {
                int len = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1,
                                              nullptr, 0, nullptr, nullptr);
                std::string arg;
                if (len > 1) {
                    arg.resize(len - 1);
                    WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1,
                                        &arg[0], len, nullptr, nullptr);
                }
                args.push_back(std::move(arg));
            }
            LocalFree(wargv);
            return args;
        }
#endif
        return std::vector<std::string>(argv, argv + argc);
    }

    fs::path pathFromUtf8(const std::string& utf8) {
        return fs::u8path(utf8);
    }

    std::string pathToUtf8(const fs::path& p) {
        return p.generic_u8string();
    }

    bool readFileBytes(const fs::path& path, std::vector<uint8_t>& out) {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            std::cout << "Error: Could not read file: " << pathToUtf8(path) << std::endl;
            return false;
        }
        file.seekg(0, std::ios::end);
        std::streamoff size = file.tellg();
        file.seekg(0, std::ios::beg);
        if (size < 0) {
            std::cout << "Error: Could not read file: " << pathToUtf8(path) << std::endl;
            return false;
        }
        out.resize(static_cast<size_t>(size));
        if (size > 0 &&
            !file.read(reinterpret_cast<char*>(out.data()), size)) {
            std::cout << "Error: Could not read file: " << pathToUtf8(path) << std::endl;
            return false;
        }
        return true;
    }

    bool writeFileBytes(const fs::path& path, const std::vector<uint8_t>& data) {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            std::cout << "Error: Could not write file: " << pathToUtf8(path) << std::endl;
            return false;
        }
        file.write(reinterpret_cast<const char*>(data.data()),
                   static_cast<std::streamsize>(data.size()));
        file.close();
        if (!file) {
            std::cout << "Error: Failed while writing: " << pathToUtf8(path) << std::endl;
            return false;
        }
        return true;
    }

    container compressBytes(compressor& comp, const std::vector<uint8_t>& data,
                            const std::string& algorithm) {
        if (algorithm == "auto") {
            return comp.compressAuto(data);
        }
        CompressionType type = NONE;
        cli_parser::algorithmFromName(algorithm, type);
        return comp.compress(data, type);
    }

    bool isSafeRelativePath(const std::string& path) {
        if (path.empty()) {
            return false;
        }
#ifdef _WIN32
        if (path.find(':') != std::string::npos) {
            return false;
        }
#endif
        fs::path p = pathFromUtf8(path);
        if (p.is_absolute() || p.has_root_path()) {
            return false;
        }
        for (const fs::path& part : p) {
            if (part == "..") {
                return false;
            }
        }
        return true;
    }

    void printEntryLine(const archiveEntry& entry) {
        if (entry.isDirectory) {
            std::cout << "  [dir]  " << entry.path << std::endl;
            return;
        }
        std::cout << "  [file] " << entry.path
                  << " (" << compressor::typeName(entry.data.getType())
                  << ", " << entry.data.getOriginalSize()
                  << " -> " << entry.data.getCompressedSize() << " bytes)"
                  << std::endl;
    }

    int commandZb(const cli_options& options) {

        fs::path input = fs::absolute(pathFromUtf8(options.input)).lexically_normal();

        if (input.filename().empty() || input.filename() == ".") {
            input = input.parent_path();
        }

        if (!fs::exists(input)) {
            std::cout << "Error: Input does not exist: " << pathToUtf8(input) << std::endl;
            return 1;
        }

        compressor comp;
        filehandler handler;
        std::vector<archiveEntry> entries;

        std::string outputPath = handler.ensureExtension(
            options.output.empty() ? pathToUtf8(input.filename()) : options.output);
        fs::path outputAbs = fs::absolute(pathFromUtf8(outputPath)).lexically_normal();

        uint64_t totalOriginal = 0;
        uint64_t totalCompressed = 0;

        if (fs::is_directory(input)) {

            fs::path base = fs::absolute(input).parent_path();

            archiveEntry rootEntry;
            rootEntry.path = pathToUtf8(input.filename());
            rootEntry.isDirectory = true;
            entries.push_back(rootEntry);

            for (const auto& item : fs::recursive_directory_iterator(
                     input, fs::directory_options::skip_permission_denied)) {

                if (fs::absolute(item.path()).lexically_normal() == outputAbs) {
                    std::cout << "Skipping output archive: "
                              << pathToUtf8(item.path()) << std::endl;
                    continue;
                }

                std::string relative =
                    pathToUtf8(fs::relative(fs::absolute(item.path()), base));

                archiveEntry entry;
                entry.path = relative;

                if (item.is_directory()) {
                    entry.isDirectory = true;
                    entries.push_back(std::move(entry));
                    continue;
                }

                if (!item.is_regular_file()) {
                    std::cout << "Skipping (not a regular file): " << relative << std::endl;
                    continue;
                }

                std::vector<uint8_t> data;
                if (!readFileBytes(item.path(), data)) {
                    return 1;
                }

                entry.data = compressBytes(comp, data, options.algorithm);
                totalOriginal += entry.data.getOriginalSize();
                totalCompressed += entry.data.getCompressedSize();
                printEntryLine(entry);
                entries.push_back(std::move(entry));
            }
        }
        else if (fs::is_regular_file(input)) {

            if (fs::absolute(input).lexically_normal() == outputAbs) {
                std::cout << "Error: Output archive would overwrite the input file: "
                          << pathToUtf8(input) << std::endl;
                std::cout << "Use -o to choose a different output name." << std::endl;
                return 1;
            }

            std::vector<uint8_t> data;
            if (!readFileBytes(input, data)) {
                return 1;
            }

            archiveEntry entry;
            entry.path = pathToUtf8(input.filename());
            entry.data = compressBytes(comp, data, options.algorithm);
            totalOriginal = entry.data.getOriginalSize();
            totalCompressed = entry.data.getCompressedSize();
            printEntryLine(entry);
            entries.push_back(std::move(entry));
        }
        else {
            std::cout << "Error: Input is neither a file nor a folder: "
                      << pathToUtf8(input) << std::endl;
            return 1;
        }

        if (!handler.saveArchive(entries, outputPath)) {
            return 1;
        }

        std::cout << "Total: " << totalOriginal << " -> "
                  << totalCompressed << " bytes";
        if (totalOriginal > 0) {
            double ratio = static_cast<double>(totalCompressed) / totalOriginal * 100.0;
            std::cout << " (" << ratio << "%)";
        }
        std::cout << std::endl;

        return 0;
    }

    int commandExtract(const cli_options& options) {

        filehandler handler;
        std::vector<archiveEntry> entries;

        if (!handler.loadArchive(options.input, entries)) {
            return 1;
        }

        fs::path outputBase = options.output.empty()
            ? fs::path(".")
            : pathFromUtf8(options.output);

        compressor comp;
        int extracted = 0;
        int problems = 0;

        for (const archiveEntry& entry : entries) {

            if (!isSafeRelativePath(entry.path)) {
                std::cout << "Skipping unsafe path in archive: " << entry.path << std::endl;
                problems++;
                continue;
            }

            fs::path target = outputBase / pathFromUtf8(entry.path);

            if (entry.isDirectory) {
                fs::create_directories(target);
                continue;
            }

            if (target.has_parent_path()) {
                fs::create_directories(target.parent_path());
            }

            std::vector<uint8_t> data = comp.decompress(entry.data);

            if (data.size() != entry.data.getOriginalSize()) {
                std::cout << "Warning: Size mismatch for " << entry.path
                          << " (expected " << entry.data.getOriginalSize()
                          << ", got " << data.size() << ")" << std::endl;
                problems++;
            }
            else if (crc32(data) != entry.data.getOriginalCrc()) {
                std::cout << "Warning: Checksum mismatch for " << entry.path
                          << " (data is corrupt)" << std::endl;
                problems++;
            }

            if (!writeFileBytes(target, data)) {
                return 1;
            }

            std::cout << "  extracted: " << pathToUtf8(target) << std::endl;
            extracted++;
        }

        std::cout << "Done. " << extracted << " file(s) extracted." << std::endl;

        if (problems > 0) {
            std::cout << problems << " entry(ies) had problems." << std::endl;
            return 1;
        }

        return 0;
    }

    int commandList(const cli_options& options) {

        filehandler handler;
        std::vector<archiveEntry> entries;

        if (!handler.loadArchive(options.input, entries)) {
            return 1;
        }

        uint64_t totalOriginal = 0;
        uint64_t totalCompressed = 0;
        int fileCount = 0;

        std::cout << "Archive: " << options.input << std::endl;
        for (const archiveEntry& entry : entries) {
            printEntryLine(entry);
            if (!entry.isDirectory) {
                totalOriginal += entry.data.getOriginalSize();
                totalCompressed += entry.data.getCompressedSize();
                fileCount++;
            }
        }

        std::cout << fileCount << " file(s), " << totalOriginal << " -> "
                  << totalCompressed << " bytes";
        if (totalOriginal > 0) {
            double ratio = static_cast<double>(totalCompressed) / totalOriginal * 100.0;
            std::cout << " (" << ratio << "%)";
        }
        std::cout << std::endl;

        return 0;
    }

    int commandVerify(const cli_options& options) {

        filehandler handler;
        std::vector<archiveEntry> entries;

        if (!handler.loadArchive(options.input, entries)) {
            return 1;
        }

        compressor comp;
        int failures = 0;

        for (const archiveEntry& entry : entries) {
            if (entry.isDirectory) {
                continue;
            }

            std::vector<uint8_t> data = comp.decompress(entry.data);

            if (data.size() != entry.data.getOriginalSize()) {
                std::cout << "  FAIL: " << entry.path
                          << " (expected " << entry.data.getOriginalSize()
                          << " bytes, got " << data.size() << ")" << std::endl;
                failures++;
            }
            else if (crc32(data) != entry.data.getOriginalCrc()) {
                std::cout << "  FAIL: " << entry.path << " (checksum mismatch)" << std::endl;
                failures++;
            }
            else {
                std::cout << "  OK:   " << entry.path << std::endl;
            }
        }

        if (failures > 0) {
            std::cout << failures << " entry(ies) failed verification." << std::endl;
            return 1;
        }

        std::cout << "Archive verified successfully." << std::endl;
        return 0;
    }
}

int main(int argc, char* argv[]) {

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::vector<std::string> args = collectArgs(argc, argv);

    cli_parser parser;
    cli_options options = parser.parse(args);

    if (!options.valid) {
        std::cout << "Error: " << options.error << std::endl << std::endl;
        parser.printUsage();
        return 1;
    }

    if (options.command == "help") {
        parser.printUsage();
        return 0;
    }

    try {
        if (options.command == "zb") {
            return commandZb(options);
        }
        if (options.command == "extract") {
            return commandExtract(options);
        }
        if (options.command == "list") {
            return commandList(options);
        }
        if (options.command == "verify") {
            return commandVerify(options);
        }
    }
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }

    parser.printUsage();
    return 1;
}
