//
// Created by IliyaD on 26.02.2026.
//

#ifndef ZIPBIKE_HUFFMAN_H
#define ZIPBIKE_HUFFMAN_H

#include <string>
#include <map>

class huffman {

    struct node {
        char ch;
        int freq;
        node *left;
        node *right;

        node(char c, int freq);
        node(char ch, int freq, node* left, node* right);
    };

    struct compare {
        bool operator()(node* a, node* b);
    };

    std::map<char, int> frequencies;
    std::map<char, std::string> huffmanCode;
    node* root;

    void printHuffmanCode(node* node, const std::string& code);
    void buildHuffmanTree();
    void createHuffmanTree(const std::string& text);

    public:

    void compressFile(const std::string& inputF, const std::string& outputF);
    void decompressFile(const std::string& inputF, const std::string& outputF);

};

#endif // ZIPBIKE_HUFFMAN_H