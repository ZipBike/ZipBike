//
// Created by IliyaD on 26.02.2026.
//

#include "filehandler.h"
#include <fstream>
#include <iostream>

filehandler::filehandler() {
}


bool filehandler::save(container& c, std::string filePath) {

    filePath = ensureExtension(filePath);

    std::ofstream file(filePath, std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Error: Could not open file for saving: " << filePath << std::endl;
        return false;
    }

    uint8_t type = static_cast<uint8_t>(c.getType());
    file.write(reinterpret_cast<char*>(&type), sizeof(uint8_t));

    int originalSize = c.getOriginalSize();
    file.write(reinterpret_cast<char*>(&originalSize), sizeof(int));

    std::vector<uint8_t> data = c.getData();
    file.write(reinterpret_cast<char*>(data.data()), data.size());

    file.close();

    std::cout << "Saved to: " << filePath << std::endl;
    return true;
}


container filehandler::load(std::string filePath) {

    container c;

    std::ifstream file(filePath, std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Error: Could not open file: " << filePath << std::endl;
        return c; // return empty container
    }

    uint8_t type;
    file.read(reinterpret_cast<char*>(&type), sizeof(uint8_t));
    c.setType(static_cast<CompressionType>(type));

    int originalSize;
    file.read(reinterpret_cast<char*>(&originalSize), sizeof(int));
    c.setOriginalSize(originalSize);

    std::vector<uint8_t> data(
        (std::istreambuf_iterator<char>(file)),
        (std::istreambuf_iterator<char>())
    );
    c.setData(data);
    c.setFileName(filePath);

    file.close();

    std::cout << "Loaded from: " << filePath << std::endl;
    return c;
}

bool filehandler::fileExists(std::string filePath) {
    std::ifstream file(filePath);
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