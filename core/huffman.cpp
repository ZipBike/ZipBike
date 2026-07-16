#include "huffman.h"
#include "bitstream.h"
#include <queue>
#include <iostream>
#include <fstream>

namespace {

    void writeU16(std::vector<uint8_t>& out, uint16_t value) {
        out.push_back(static_cast<uint8_t>(value & 0xFF));
        out.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    }

    void writeU64(std::vector<uint8_t>& out, uint64_t value) {
        for (int i = 0; i < 8; i++) {
            out.push_back(static_cast<uint8_t>((value >> (8 * i)) & 0xFF));
        }
    }

    uint16_t readU16(const std::vector<uint8_t>& in, size_t pos) {
        return static_cast<uint16_t>(in[pos]) |
               static_cast<uint16_t>(in[pos + 1] << 8);
    }

    uint64_t readU64(const std::vector<uint8_t>& in, size_t pos) {
        uint64_t value = 0;
        for (int i = 0; i < 8; i++) {
            value |= static_cast<uint64_t>(in[pos + i]) << (8 * i);
        }
        return value;
    }
}

huffman::node::node(uint8_t ch, uint64_t freq)
    : ch(ch), freq(freq), left(nullptr), right(nullptr) {}

huffman::node::node(uint8_t ch, uint64_t freq, node* left, node* right)
    : ch(ch), freq(freq), left(left), right(right) {}

bool huffman::compare::operator()(node* a, node* b) {
    if (a->freq != b->freq) {
        return a->freq > b->freq;
    }
    return a->ch > b->ch;
}

huffman::huffman() : root(nullptr) {
    frequencies.fill(0);
}

huffman::~huffman() {
    freeTree(root);
}

void huffman::freeTree(node* n) {
    if (n == nullptr) {
        return;
    }
    freeTree(n->left);
    freeTree(n->right);
    delete n;
}

void huffman::buildHuffmanTree() {

    freeTree(root);
    root = nullptr;
    for (std::string& code : huffmanCode) {
        code.clear();
    }

    std::priority_queue<node*, std::vector<node*>, compare> pq;
    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            pq.push(new node(static_cast<uint8_t>(i), frequencies[i]));
        }
    }

    if (pq.empty()) {
        return;
    }

    while (pq.size() > 1) {
        node* left = pq.top();
        pq.pop();
        node* right = pq.top();
        pq.pop();
        pq.push(new node(0, left->freq + right->freq, left, right));
    }

    root = pq.top();

    if (root->left == nullptr && root->right == nullptr) {
        huffmanCode[root->ch] = "0";
    }
    else {
        buildCodes(root, "");
    }
}

void huffman::buildCodes(node* n, const std::string& code) {
    if (n == nullptr) {
        return;
    }
    if (n->left == nullptr && n->right == nullptr) {
        huffmanCode[n->ch] = code;
        return;
    }
    buildCodes(n->left, code + "0");
    buildCodes(n->right, code + "1");
}

std::vector<uint8_t> huffman::compress(const std::vector<uint8_t>& input) {

    frequencies.fill(0);
    for (uint8_t byte : input) {
        frequencies[byte]++;
    }

    buildHuffmanTree();

    uint16_t symbolCount = 0;
    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            symbolCount++;
        }
    }

    std::vector<uint8_t> output;
    writeU16(output, symbolCount);

    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            output.push_back(static_cast<uint8_t>(i));
            writeU64(output, frequencies[i]);
        }
    }

    bitstream bits;
    for (uint8_t byte : input) {
        const std::string& code = huffmanCode[byte];
        for (char bit : code) {
            bits.writeBit(bit == '1' ? 1 : 0);
        }
    }

    writeU64(output, bits.size());

    const std::vector<uint8_t>& packed = bits.getData();
    output.insert(output.end(), packed.begin(), packed.end());

    return output;
}

std::vector<uint8_t> huffman::decompress(const std::vector<uint8_t>& input) {

    std::vector<uint8_t> output;

    if (input.size() < 2) {
        return output;
    }

    size_t pos = 0;
    uint16_t symbolCount = readU16(input, pos);
    pos += 2;

    if (symbolCount > 256) {
        std::cerr << "Huffman: corrupt data (invalid symbol count)" << std::endl;
        return output;
    }

    frequencies.fill(0);
    uint64_t totalSymbols = 0;

    for (uint16_t i = 0; i < symbolCount; i++) {
        if (pos + 9 > input.size()) {
            std::cerr << "Huffman: corrupt data (truncated frequency table)" << std::endl;
            return output;
        }
        uint8_t symbol = input[pos];
        uint64_t freq = readU64(input, pos + 1);
        pos += 9;
        frequencies[symbol] = freq;
        totalSymbols += freq;
    }

    if (symbolCount == 0) {
        return output;
    }

    if (pos + 8 > input.size()) {
        std::cerr << "Huffman: corrupt data (missing bit count)" << std::endl;
        return output;
    }
    uint64_t bitCount = readU64(input, pos);
    pos += 8;

    if (bitCount > (input.size() - pos) * 8) {
        std::cerr << "Huffman: corrupt data (bit count exceeds payload)" << std::endl;
        return output;
    }

    if (totalSymbols > bitCount) {
        std::cerr << "Huffman: corrupt data (symbol count exceeds bit count)" << std::endl;
        return output;
    }

    buildHuffmanTree();

    std::vector<uint8_t> packed(input.begin() + pos, input.end());
    bitstream bits(packed, bitCount);

    output.reserve(totalSymbols);

    if (root->left == nullptr && root->right == nullptr) {
        for (uint64_t i = 0; i < totalSymbols; i++) {
            output.push_back(root->ch);
        }
        return output;
    }

    node* curr = root;
    while (output.size() < totalSymbols) {
        int bit = bits.readBit();
        if (bit < 0) {
            std::cerr << "Huffman: corrupt data (bit stream ended early)" << std::endl;
            break;
        }
        curr = (bit == 0) ? curr->left : curr->right;
        if (curr == nullptr) {
            std::cerr << "Huffman: corrupt data (invalid code path)" << std::endl;
            break;
        }
        if (curr->left == nullptr && curr->right == nullptr) {
            output.push_back(curr->ch);
            curr = root;
        }
    }

    return output;
}

void huffman::compressFile(const std::string& inputF, const std::string& outputF) {

    std::ifstream in(inputF, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Could not open file " << inputF << std::endl;
        return;
    }
    std::vector<uint8_t> data(
        (std::istreambuf_iterator<char>(in)),
        (std::istreambuf_iterator<char>())
    );
    in.close();

    std::vector<uint8_t> compressed = compress(data);

    std::ofstream out(outputF, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Could not open file " << outputF << std::endl;
        return;
    }
    out.write(reinterpret_cast<const char*>(compressed.data()),
              static_cast<std::streamsize>(compressed.size()));
    out.close();
    std::cout << "The file is successfully compressed" << std::endl;
}

void huffman::decompressFile(const std::string& inputF, const std::string& outputF) {

    std::ifstream in(inputF, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Could not open file " << inputF << std::endl;
        return;
    }
    std::vector<uint8_t> data(
        (std::istreambuf_iterator<char>(in)),
        (std::istreambuf_iterator<char>())
    );
    in.close();

    std::vector<uint8_t> decompressed = decompress(data);

    std::ofstream out(outputF, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Could not open file " << outputF << std::endl;
        return;
    }
    out.write(reinterpret_cast<const char*>(decompressed.data()),
              static_cast<std::streamsize>(decompressed.size()));
    out.close();
    std::cout << "The file is successfully decompressed" << std::endl;
}
