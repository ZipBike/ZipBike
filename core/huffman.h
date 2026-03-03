//
// Created by IliyaD on 26.02.2026.
//

#ifndef ZIPBIKE_HUFFMAN_H
#define ZIPBIKE_HUFFMAN_H

#include <string>
#include <map>
#include <queue>

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

    std::map<char, int> frequancies;
    std::map<char, std::string> huffmanCode;
    node* root;

    void printHuffmanCode(node* node, std::string code);
    void createHuffmanFreq(std::string& text);

    public:

    void createHuffmanTree(const std::string& text);
};

#endif // ZIPBIKE_HUFFMAN_H