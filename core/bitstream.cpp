//
// Created by IliyaD on 26.02.2026.
//

#include "bitstream.h"

bitstream::bitstream() {
    totalBits = 0;
    readPosition = 0;
}

void bitstream::writeBit(int bit) {

    if (totalBits % 8 == 0) {
        data.push_back(0);
    }

    if (bit == 1) {
        int byteIndex = totalBits / 8;
        int bitIndex  = 7 - (totalBits % 8);

        data[byteIndex] |= (1 << bitIndex);
    }

    totalBits++;
}

void bitstream::writeByte(uint8_t byte) {
    for (int i = 7; i >= 0; i--) {
        int bit = (byte >> i) & 1;
        writeBit(bit);
    }
}

int bitstream::readBit() {

    if (readPosition >= totalBits) {
        return -1;
    }

    int byteIndex = readPosition / 8;
    int bitIndex  = 7 - (readPosition % 8);

    readPosition++;

    return (data[byteIndex] >> bitIndex) & 1;
}

bool bitstream::isEmpty() {
    return readPosition >= totalBits;
}

int bitstream::size() {
    return totalBits;
}

void bitstream::clear() {
    data.clear();
    totalBits = 0;
    readPosition = 0;
}