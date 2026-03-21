//
// Created by IliyaD on 26.02.2026.
//

#ifndef ZIPBIKE_BITSTREAM_H
#define ZIPBIKE_BITSTREAM_H

#include <vector>
#include <cstdint>  // for uint8_t

class bitstream {
public:
    bitstream();
    void writeBit(int bit);
    void writeByte(uint8_t byte);
    int readBit();
    bool isEmpty();
    int size();
    void clear();

private:

    std::vector<uint8_t> data;
    int totalBits;
    int readPosition;
};

#endif // ZIPBIKE_BITSTREAM_H