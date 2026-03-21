//
// Created by IliyaD on 26.02.2026.
//

#ifndef ZIPBIKE_COMPRESSOR_H
#define ZIPBIKE_COMPRESSOR_H

#include <vector>
#include <cstdint>
#include "container.h"

class compressor {
public:

    compressor();

    container compress(std::vector<uint8_t> inputData, CompressionType type);

    std::vector<uint8_t> decompress(container& c);

    void printStats(container& c);

private:

    std::vector<uint8_t> compressRLE(std::vector<uint8_t>& data);
    std::vector<uint8_t> compressLZ77(std::vector<uint8_t>& data);
    std::vector<uint8_t> compressHuffman(std::vector<uint8_t>& data);

    std::vector<uint8_t> decompressRLE(std::vector<uint8_t>& data);
    std::vector<uint8_t> decompressLZ77(std::vector<uint8_t>& data);
    std::vector<uint8_t> decompressHuffman(std::vector<uint8_t>& data);
};

#endif // ZIPBIKE_COMPRESSOR_H