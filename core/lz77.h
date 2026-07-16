#ifndef ZIPBIKE_LZ77_H
#define ZIPBIKE_LZ77_H

#include <vector>
#include <cstdint>
#include <cstddef>

class lz77 {
public:

    std::vector<uint8_t> compress(const std::vector<uint8_t>& input);
    std::vector<uint8_t> decompress(const std::vector<uint8_t>& input);

private:

    static constexpr size_t WINDOW_SIZE = 4096;
    static constexpr size_t MIN_MATCH   = 3;
    static constexpr size_t MAX_MATCH   = 255;
};

#endif
