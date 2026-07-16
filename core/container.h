#ifndef ZIPBIKE_CONTAINER_H
#define ZIPBIKE_CONTAINER_H

#include <vector>
#include <cstdint>
#include <string>

enum CompressionType {
    NONE,
    RLE,
    LZ77,
    HUFFMAN
};

class container {
public:

    container();

    container(CompressionType type, std::vector<uint8_t> compressedData, uint64_t originalSize);

    void setData(std::vector<uint8_t> compressedData);
    void setType(CompressionType type);
    void setOriginalSize(uint64_t size);
    void setOriginalCrc(uint32_t crc);
    void setFileName(std::string name);

    const std::vector<uint8_t>& getData() const;
    CompressionType getType() const;
    uint64_t getOriginalSize() const;
    uint32_t getOriginalCrc() const;
    uint64_t getCompressedSize() const;
    std::string getFileName() const;

    bool isEmpty() const;

    void clear();

private:

    std::vector<uint8_t> data;
    CompressionType type;
    uint64_t originalSize;
    uint32_t originalCrc;
    std::string fileName;
};

#endif
