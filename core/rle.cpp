#include "rle.h"

std::vector<uint8_t> rle::compress(const std::vector<uint8_t>& input) {

    std::vector<uint8_t> output;
    output.reserve(input.size());

    size_t i = 0;
    while (i < input.size()) {

        uint8_t value = input[i];
        size_t runLength = 1;

        while (i + runLength < input.size() &&
               input[i + runLength] == value &&
               runLength < 255) {
            runLength++;
        }

        output.push_back(static_cast<uint8_t>(runLength));
        output.push_back(value);

        i += runLength;
    }

    return output;
}

std::vector<uint8_t> rle::decompress(const std::vector<uint8_t>& input) {

    std::vector<uint8_t> output;

    for (size_t i = 0; i + 1 < input.size(); i += 2) {
        uint8_t runLength = input[i];
        uint8_t value = input[i + 1];
        output.insert(output.end(), runLength, value);
    }

    return output;
}
