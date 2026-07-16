#include "bitstream.h"

bitstream::bitstream() {
    totalBits = 0;
    readPosition = 0;
}

bitstream::bitstream(const std::vector<uint8_t>& bytes, size_t bitCount) {
    data = bytes;
    totalBits = bitCount;
    readPosition = 0;

    if (totalBits > data.size() * 8) {
        totalBits = data.size() * 8;
    }
}

void bitstream::writeBit(int bit) {

    if (totalBits % 8 == 0) {
        data.push_back(0);
    }

    if (bit == 1) {
        size_t byteIndex = totalBits / 8;
        int bitIndex = 7 - static_cast<int>(totalBits % 8);

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

    size_t byteIndex = readPosition / 8;
    int bitIndex = 7 - static_cast<int>(readPosition % 8);

    readPosition++;

    return (data[byteIndex] >> bitIndex) & 1;
}

bool bitstream::isEmpty() {
    return readPosition >= totalBits;
}

size_t bitstream::size() {
    return totalBits;
}

void bitstream::clear() {
    data.clear();
    totalBits = 0;
    readPosition = 0;
}

const std::vector<uint8_t>& bitstream::getData() const {
    return data;
}
