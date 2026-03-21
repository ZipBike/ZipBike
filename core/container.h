//
// Created by IliyaD on 26.02.2026.
//

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

    container(CompressionType type, std::vector<uint8_t> compressedData, int originalSize);

    void setData(std::vector<uint8_t> compressedData);
    void setType(CompressionType type);
    void setOriginalSize(int size);

    std::vector<uint8_t> getData();
    CompressionType getType();
    int getOriginalSize();
    int getCompressedSize();

    bool isEmpty();

    void clear();

private:

    std::vector<uint8_t> data;
    CompressionType type;
    int originalSize;
};

#endif // ZIPBIKE_CONTAINER_H