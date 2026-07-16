#ifndef ZIPBIKE_BITSTREAM_H
#define ZIPBIKE_BITSTREAM_H

#include <vector>
#include <cstdint>
#include <cstddef>

class bitstream {
public:
    bitstream();
    bitstream(const std::vector<uint8_t>& bytes, size_t bitCount);

    void writeBit(int bit);
    void writeByte(uint8_t byte);
    int readBit();
    bool isEmpty();
    size_t size();
    void clear();

    const std::vector<uint8_t>& getData() const;

private:

    std::vector<uint8_t> data;
    size_t totalBits;
    size_t readPosition;
};

#endif
