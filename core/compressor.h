#ifndef ZIPBIKE_COMPRESSOR_H
#define ZIPBIKE_COMPRESSOR_H

#include <vector>
#include <cstdint>
#include "container.h"

class compressor {
public:

    compressor();

    container compress(const std::vector<uint8_t>& inputData, CompressionType type);

    container compressAuto(const std::vector<uint8_t>& inputData);

    std::vector<uint8_t> decompress(const container& c);

    void printStats(const container& c);

    static const char* typeName(CompressionType type);

private:

    std::vector<uint8_t> compressRLE(const std::vector<uint8_t>& data);
    std::vector<uint8_t> compressLZ77(const std::vector<uint8_t>& data);
    std::vector<uint8_t> compressHuffman(const std::vector<uint8_t>& data);

    std::vector<uint8_t> decompressRLE(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decompressLZ77(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decompressHuffman(const std::vector<uint8_t>& data);
};

#endif
