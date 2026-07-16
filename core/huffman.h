#ifndef ZIPBIKE_HUFFMAN_H
#define ZIPBIKE_HUFFMAN_H

#include <string>
#include <array>
#include <vector>
#include <cstdint>

class huffman {

    struct node {
        uint8_t ch;
        uint64_t freq;
        node *left;
        node *right;

        node(uint8_t ch, uint64_t freq);
        node(uint8_t ch, uint64_t freq, node* left, node* right);
    };

    struct compare {
        bool operator()(node* a, node* b);
    };

    std::array<uint64_t, 256> frequencies;
    std::array<std::string, 256> huffmanCode;
    node* root;

    void buildHuffmanTree();
    void buildCodes(node* n, const std::string& code);
    void freeTree(node* n);

public:

    huffman();
    ~huffman();

    huffman(const huffman&) = delete;
    huffman& operator=(const huffman&) = delete;

    std::vector<uint8_t> compress(const std::vector<uint8_t>& input);
    std::vector<uint8_t> decompress(const std::vector<uint8_t>& input);

    void compressFile(const std::string& inputF, const std::string& outputF);
    void decompressFile(const std::string& inputF, const std::string& outputF);
};

#endif
