#include "filehandler.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <random>
#include <sstream>

namespace {

    const char MAGIC[4] = { 'Z', 'B', 'I', 'K' };
    const uint8_t FORMAT_VERSION = 3;

    std::filesystem::path toFsPath(const std::string& utf8) {
        return std::filesystem::u8path(utf8);
    }

    std::string makeTempPath(const std::string& filePath) {
        std::random_device rd;
        std::ostringstream suffix;
        suffix << std::hex << rd() << rd();
        return filePath + ".tmp-" + suffix.str();
    }

    void writeU16(std::ofstream& out, uint16_t value) {
        uint8_t bytes[2] = {
            static_cast<uint8_t>(value & 0xFF),
            static_cast<uint8_t>((value >> 8) & 0xFF)
        };
        out.write(reinterpret_cast<char*>(bytes), 2);
    }

    void writeU32(std::ofstream& out, uint32_t value) {
        uint8_t bytes[4];
        for (int i = 0; i < 4; i++) {
            bytes[i] = static_cast<uint8_t>((value >> (8 * i)) & 0xFF);
        }
        out.write(reinterpret_cast<char*>(bytes), 4);
    }

    void writeU64(std::ofstream& out, uint64_t value) {
        uint8_t bytes[8];
        for (int i = 0; i < 8; i++) {
            bytes[i] = static_cast<uint8_t>((value >> (8 * i)) & 0xFF);
        }
        out.write(reinterpret_cast<char*>(bytes), 8);
    }

    bool readU16(std::ifstream& in, uint16_t& value) {
        uint8_t bytes[2];
        if (!in.read(reinterpret_cast<char*>(bytes), 2)) {
            return false;
        }
        value = static_cast<uint16_t>(bytes[0] | (bytes[1] << 8));
        return true;
    }

    bool readU32(std::ifstream& in, uint32_t& value) {
        uint8_t bytes[4];
        if (!in.read(reinterpret_cast<char*>(bytes), 4)) {
            return false;
        }
        value = 0;
        for (int i = 0; i < 4; i++) {
            value |= static_cast<uint32_t>(bytes[i]) << (8 * i);
        }
        return true;
    }

    bool readU64(std::ifstream& in, uint64_t& value) {
        uint8_t bytes[8];
        if (!in.read(reinterpret_cast<char*>(bytes), 8)) {
            return false;
        }
        value = 0;
        for (int i = 0; i < 8; i++) {
            value |= static_cast<uint64_t>(bytes[i]) << (8 * i);
        }
        return true;
    }
}

filehandler::filehandler() {
}

bool filehandler::save(container& c, std::string filePath) {

    archiveEntry entry;
    entry.path = c.getFileName().empty() ? "data" : c.getFileName();
    entry.isDirectory = false;
    entry.data = c;

    std::vector<archiveEntry> entries;
    entries.push_back(entry);

    return saveArchive(entries, filePath);
}

container filehandler::load(std::string filePath) {

    std::vector<archiveEntry> entries;

    if (!loadArchive(filePath, entries)) {
        return container();
    }

    for (const archiveEntry& entry : entries) {
        if (!entry.isDirectory) {
            container c = entry.data;
            c.setFileName(entry.path);
            return c;
        }
    }

    std::cout << "Error: Archive contains no files: " << filePath << std::endl;
    return container();
}

bool filehandler::saveArchive(const std::vector<archiveEntry>& entries, std::string filePath) {

    filePath = ensureExtension(filePath);

    for (const archiveEntry& entry : entries) {
        if (entry.path.size() > UINT16_MAX) {
            std::cout << "Error: Path too long: " << entry.path << std::endl;
            return false;
        }
    }

    std::string tempPath = makeTempPath(filePath);

    std::ofstream file(toFsPath(tempPath), std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Error: Could not open file for saving: " << tempPath << std::endl;
        return false;
    }

    file.write(MAGIC, 4);
    file.put(static_cast<char>(FORMAT_VERSION));
    writeU32(file, static_cast<uint32_t>(entries.size()));

    for (const archiveEntry& entry : entries) {

        writeU16(file, static_cast<uint16_t>(entry.path.size()));
        file.write(entry.path.data(), static_cast<std::streamsize>(entry.path.size()));
        file.put(entry.isDirectory ? 1 : 0);
        file.put(static_cast<char>(entry.data.getType()));
        writeU64(file, entry.data.getOriginalSize());
        writeU32(file, entry.data.getOriginalCrc());

        const std::vector<uint8_t>& data = entry.data.getData();
        writeU64(file, data.size());
        file.write(reinterpret_cast<const char*>(data.data()),
                   static_cast<std::streamsize>(data.size()));
    }

    file.close();

    std::error_code ec;

    if (!file) {
        std::cout << "Error: Failed while writing: " << tempPath << std::endl;
        std::filesystem::remove(toFsPath(tempPath), ec);
        return false;
    }

    std::filesystem::rename(toFsPath(tempPath), toFsPath(filePath), ec);
    if (ec) {
        std::cout << "Error: Could not replace " << filePath
                  << ": " << ec.message() << std::endl;
        std::filesystem::remove(toFsPath(tempPath), ec);
        return false;
    }

    std::cout << "Saved to: " << filePath << std::endl;
    return true;
}

bool filehandler::loadArchive(std::string filePath, std::vector<archiveEntry>& entries) {

    entries.clear();

    std::ifstream file(toFsPath(filePath), std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Error: Could not open file: " << filePath << std::endl;
        return false;
    }

    file.seekg(0, std::ios::end);
    uint64_t fileSize = static_cast<uint64_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    char magic[4];
    if (!file.read(magic, 4) ||
        magic[0] != MAGIC[0] || magic[1] != MAGIC[1] ||
        magic[2] != MAGIC[2] || magic[3] != MAGIC[3]) {
        std::cout << "Error: Not a ZipBike archive: " << filePath << std::endl;
        return false;
    }

    char version;
    if (!file.get(version) || static_cast<uint8_t>(version) != FORMAT_VERSION) {
        std::cout << "Error: Unsupported archive version in: " << filePath << std::endl;
        return false;
    }

    uint32_t entryCount;
    if (!readU32(file, entryCount)) {
        std::cout << "Error: Corrupt archive header: " << filePath << std::endl;
        return false;
    }

    for (uint32_t i = 0; i < entryCount; i++) {

        archiveEntry entry;

        uint16_t pathLen;
        if (!readU16(file, pathLen)) {
            std::cout << "Error: Corrupt archive entry in: " << filePath << std::endl;
            return false;
        }

        entry.path.resize(pathLen);
        if (pathLen > 0 && !file.read(&entry.path[0], pathLen)) {
            std::cout << "Error: Corrupt archive entry in: " << filePath << std::endl;
            return false;
        }

        char isDir;
        char type;
        uint64_t originalSize;
        uint32_t originalCrc;
        uint64_t compressedSize;

        if (!file.get(isDir) || !file.get(type) ||
            !readU64(file, originalSize) || !readU32(file, originalCrc) ||
            !readU64(file, compressedSize)) {
            std::cout << "Error: Corrupt archive entry in: " << filePath << std::endl;
            return false;
        }

        if (type < NONE || type > HUFFMAN) {
            std::cout << "Error: Unknown compression type in: " << filePath << std::endl;
            return false;
        }

        if (compressedSize > fileSize) {
            std::cout << "Error: Corrupt archive data in: " << filePath << std::endl;
            return false;
        }

        entry.isDirectory = (isDir != 0);
        entry.data.setType(static_cast<CompressionType>(type));
        entry.data.setOriginalSize(originalSize);
        entry.data.setOriginalCrc(originalCrc);

        std::vector<uint8_t> data(compressedSize);
        if (compressedSize > 0 &&
            !file.read(reinterpret_cast<char*>(data.data()),
                       static_cast<std::streamsize>(compressedSize))) {
            std::cout << "Error: Corrupt archive data in: " << filePath << std::endl;
            return false;
        }
        entry.data.setData(std::move(data));

        entries.push_back(std::move(entry));
    }

    uint64_t endPosition = static_cast<uint64_t>(file.tellg());
    if (endPosition != fileSize) {
        entries.clear();
        std::cout << "Error: Corrupt archive (unexpected trailing data): "
                  << filePath << std::endl;
        return false;
    }

    return true;
}

bool filehandler::fileExists(std::string filePath) {
    std::ifstream file(toFsPath(filePath));
    return file.is_open();
}

std::string filehandler::ensureExtension(std::string filePath) {
    std::string ext = ".zbik";

    if (filePath.size() < ext.size() ||
        filePath.substr(filePath.size() - ext.size()) != ext) {
        filePath += ext;
    }

    return filePath;
}
