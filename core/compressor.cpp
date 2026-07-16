#include "compressor.h"
#include "rle.h"
#include "lz77.h"
#include "huffman.h"
#include "crc32.h"
#include <iostream>

compressor::compressor() {
}

container compressor::compress(const std::vector<uint8_t>& inputData, CompressionType type) {

    std::vector<uint8_t> result;

    if (type == RLE) {
        result = compressRLE(inputData);
    }
    else if (type == LZ77) {
        result = compressLZ77(inputData);
    }
    else if (type == HUFFMAN) {
        result = compressHuffman(inputData);
    }
    else {
        result = inputData;
    }

    container c(type, std::move(result), inputData.size());
    c.setOriginalCrc(crc32(inputData));
    return c;
}

container compressor::compressAuto(const std::vector<uint8_t>& inputData) {

    CompressionType bestType = NONE;
    std::vector<uint8_t> bestData;
    size_t bestSize = inputData.size();
    bool beatStored = false;

    CompressionType candidates[] = { RLE, LZ77, HUFFMAN };

    for (CompressionType type : candidates) {

        std::vector<uint8_t> attempt;
        if (type == RLE) {
            attempt = compressRLE(inputData);
        }
        else if (type == LZ77) {
            attempt = compressLZ77(inputData);
        }
        else {
            attempt = compressHuffman(inputData);
        }

        if (attempt.size() < bestSize) {
            bestType = type;
            bestSize = attempt.size();
            bestData = std::move(attempt);
            beatStored = true;
        }
    }

    if (!beatStored) {
        bestData = inputData;
    }

    container c(bestType, std::move(bestData), inputData.size());
    c.setOriginalCrc(crc32(inputData));
    return c;
}

std::vector<uint8_t> compressor::decompress(const container& c) {

    const std::vector<uint8_t>& data = c.getData();

    if (c.getType() == RLE) {
        return decompressRLE(data);
    }
    else if (c.getType() == LZ77) {
        return decompressLZ77(data);
    }
    else if (c.getType() == HUFFMAN) {
        return decompressHuffman(data);
    }

    return data;
}

void compressor::printStats(const container& c) {
    std::cout << "Algorithm:       " << typeName(c.getType()) << std::endl;
    std::cout << "Original size:   " << c.getOriginalSize() << " bytes" << std::endl;
    std::cout << "Compressed size: " << c.getCompressedSize() << " bytes" << std::endl;

    if (c.getOriginalSize() > 0) {
        double ratio = static_cast<double>(c.getCompressedSize()) / c.getOriginalSize() * 100.0;
        std::cout << "Compression ratio: " << ratio << "%" << std::endl;
    }
}

const char* compressor::typeName(CompressionType type) {
    switch (type) {
        case RLE:     return "rle";
        case LZ77:    return "lz77";
        case HUFFMAN: return "huffman";
        default:      return "none";
    }
}

std::vector<uint8_t> compressor::compressRLE(const std::vector<uint8_t>& data) {
    rle r;
    return r.compress(data);
}

std::vector<uint8_t> compressor::compressLZ77(const std::vector<uint8_t>& data) {
    lz77 l;
    return l.compress(data);
}

std::vector<uint8_t> compressor::compressHuffman(const std::vector<uint8_t>& data) {
    huffman h;
    return h.compress(data);
}

std::vector<uint8_t> compressor::decompressRLE(const std::vector<uint8_t>& data) {
    rle r;
    return r.decompress(data);
}

std::vector<uint8_t> compressor::decompressLZ77(const std::vector<uint8_t>& data) {
    lz77 l;
    return l.decompress(data);
}

std::vector<uint8_t> compressor::decompressHuffman(const std::vector<uint8_t>& data) {
    huffman h;
    return h.decompress(data);
}
