#include "container.h"

container::container() {
    type = NONE;
    originalSize = 0;
    originalCrc = 0;
    fileName = "";
}

container::container(CompressionType type, std::vector<uint8_t> compressedData, uint64_t originalSize) {
    this->type = type;
    this->data = std::move(compressedData);
    this->originalSize = originalSize;
    this->originalCrc = 0;
    this->fileName = "";
}

void container::setData(std::vector<uint8_t> compressedData) {
    data = std::move(compressedData);
}

void container::setType(CompressionType type) {
    this->type = type;
}

void container::setOriginalSize(uint64_t size) {
    originalSize = size;
}

void container::setOriginalCrc(uint32_t crc) {
    originalCrc = crc;
}

void container::setFileName(std::string name) {
    fileName = std::move(name);
}

const std::vector<uint8_t>& container::getData() const {
    return data;
}

CompressionType container::getType() const {
    return type;
}

uint64_t container::getOriginalSize() const {
    return originalSize;
}

uint32_t container::getOriginalCrc() const {
    return originalCrc;
}

uint64_t container::getCompressedSize() const {
    return data.size();
}

std::string container::getFileName() const {
    return fileName;
}

bool container::isEmpty() const {
    return data.empty();
}

void container::clear() {
    data.clear();
    type = NONE;
    originalSize = 0;
    originalCrc = 0;
    fileName = "";
}
