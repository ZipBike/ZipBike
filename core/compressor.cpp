//
// Created by IliyaD on 26.02.2026.
//

#include "compressor.h"
#include "rle.h"
#include "lz77.h"
#include "huffman.h"
#include <iostream>

compressor::compressor() {
    // Nothing to set up for now
}


container compressor::compress(std::vector<uint8_t> inputData, CompressionType type) {

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

  
    return container(type, result, inputData.size());
}


std::vector<uint8_t> compressor::decompress(container& c) {

    std::vector<uint8_t> data = c.getData();

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

void compressor::printStats(container& c) {
    std::cout << "Original size:   " << c.getOriginalSize() << " bytes" << std::endl;
    std::cout << "Compressed size: " << c.getCompressedSize() << " bytes" << std::endl;

    float ratio = (float)c.getCompressedSize() / c.getOriginalSize() * 100;
    std::cout << "Compression ratio: " << ratio << "%" << std::endl;
}

// --- Private helpers (they call your actual algorithm classes) ---

std::vector<uint8_t> compressor::compressRLE(std::vector<uint8_t>& data) {
    rle r;
    return r.compress(data);
}

std::vector<uint8_t> compressor::compressLZ77(std::vector<uint8_t>& data) {
    lz77 l;
    return l.compress(data);
}

std::vector<uint8_t> compressor::compressHuffman(std::vector<uint8_t>& data) {
    huffman h;
    return h.compress(data);
}

std::vector<uint8_t> compressor::decompressRLE(std::vector<uint8_t>& data) {
    rle r;
    return r.decompress(data);
}

std::vector<uint8_t> compressor::decompressLZ77(std::vector<uint8_t>& data) {
    lz77 l;
    return l.decompress(data);
}

std::vector<uint8_t> compressor::decompressHuffman(std::vector<uint8_t>& data) {
    huffman h;
    return h.decompress(data);
}