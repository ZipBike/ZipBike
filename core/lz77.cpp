#include "lz77.h"

namespace {

    constexpr size_t HASH_BITS = 13;
    constexpr size_t HASH_SIZE = static_cast<size_t>(1) << HASH_BITS;
    constexpr size_t MAX_CHAIN = 64;

    inline uint32_t hash3(const uint8_t* p) {
        uint32_t x = static_cast<uint32_t>(p[0]) |
                     (static_cast<uint32_t>(p[1]) << 8) |
                     (static_cast<uint32_t>(p[2]) << 16);
        return (x * 2654435761u) >> (32 - HASH_BITS);
    }
}

std::vector<uint8_t> lz77::compress(const std::vector<uint8_t>& input) {

    std::vector<uint8_t> output;
    output.reserve(input.size());

    const size_t windowMask = WINDOW_SIZE - 1;

    std::vector<long long> head(HASH_SIZE, -1);
    std::vector<long long> prevLink(WINDOW_SIZE, -1);

    size_t pos = 0;
    while (pos < input.size()) {

        size_t bestOffset = 0;
        size_t bestLength = 0;

        size_t remaining = input.size() - pos;
        size_t maxLength = remaining > 1 ? remaining - 1 : 0;
        if (maxLength > MAX_MATCH) {
            maxLength = MAX_MATCH;
        }

        size_t windowStart = pos > WINDOW_SIZE ? pos - WINDOW_SIZE : 0;

        if (maxLength >= MIN_MATCH && pos + 2 < input.size()) {

            long long candidate = head[hash3(&input[pos])];
            size_t chainLength = 0;

            while (candidate >= static_cast<long long>(windowStart) &&
                   chainLength < MAX_CHAIN) {

                size_t c = static_cast<size_t>(candidate);
                size_t length = 0;
                while (length < maxLength &&
                       input[c + length] == input[pos + length]) {
                    length++;
                }

                if (length > bestLength) {
                    bestLength = length;
                    bestOffset = pos - c;
                    if (bestLength == maxLength) {
                        break;
                    }
                }

                long long next = prevLink[c & windowMask];
                if (next >= candidate) {
                    break;
                }
                candidate = next;
                chainLength++;
            }
        }

        size_t advance;

        if (bestLength >= MIN_MATCH) {
            uint8_t literal = input[pos + bestLength];
            output.push_back(static_cast<uint8_t>(bestOffset & 0xFF));
            output.push_back(static_cast<uint8_t>((bestOffset >> 8) & 0xFF));
            output.push_back(static_cast<uint8_t>(bestLength));
            output.push_back(literal);
            advance = bestLength + 1;
        }
        else {
            output.push_back(0);
            output.push_back(0);
            output.push_back(0);
            output.push_back(input[pos]);
            advance = 1;
        }

        for (size_t k = 0; k < advance && pos + k + 2 < input.size(); k++) {
            uint32_t h = hash3(&input[pos + k]);
            prevLink[(pos + k) & windowMask] = head[h];
            head[h] = static_cast<long long>(pos + k);
        }

        pos += advance;
    }

    return output;
}

std::vector<uint8_t> lz77::decompress(const std::vector<uint8_t>& input) {

    std::vector<uint8_t> output;

    for (size_t i = 0; i + 3 < input.size(); i += 4) {

        size_t offset = static_cast<size_t>(input[i]) |
                        (static_cast<size_t>(input[i + 1]) << 8);
        size_t length = input[i + 2];
        uint8_t literal = input[i + 3];

        if (offset > 0 && offset <= output.size()) {
            size_t start = output.size() - offset;
            for (size_t j = 0; j < length; j++) {
                output.push_back(output[start + j]);
            }
        }

        output.push_back(literal);
    }

    return output;
}
