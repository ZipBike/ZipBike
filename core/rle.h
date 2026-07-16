#ifndef ZIPBIKE_RLE_H
#define ZIPBIKE_RLE_H

#include <vector>
#include <cstdint>

class rle {
public:

    std::vector<uint8_t> compress(const std::vector<uint8_t>& input);
    std::vector<uint8_t> decompress(const std::vector<uint8_t>& input);
};

#endif
