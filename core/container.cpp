//
// Created by IliyaD on 26.02.2026.
//

#include "container.h"

container::container() {
    type = NONE;
    originalSize = 0;
    fileName = "";
}

container::container(CompressionType type, std::vector<uint8_t> compressedData, int originalSize) {
    this->type = type;
    this->data = compressedData;
    this->originalSize = originalSize;
    this->fileName = "";
}

// --- Setters ---

void container::setData(std::vector<uint8_t> compressedData) {
    data = compressedData;
}

void container::setType(CompressionType type) {
    this->type = type;
}

void container::setOriginalSize(int size) {
    originalSize = size;
}

void container::setFileName(std::string name) {
    fileName = name;
}

// --- Getters ---

std::vector<uint8_t> container::getData() {
    return data;
}

CompressionType container::getType() {
    return type;
}

int container::getOriginalSize() {
    return originalSize;
}

int container::getCompressedSize() {
    return data.size();
}

std::string container::getFileName() {
    return fileName;
}

bool container::isEmpty() {
    return data.empty();
}

void container::clear() {
    data.clear();
    type = NONE;
    originalSize = 0;
    fileName = "";
}