//
// Created by IliyaD on 26.02.2026.
//

#include "huffman.h"
#include <iostream>
using namespace std;

huffman::node::node(char ch, int freq) : ch(ch), freq(freq), left(nullptr), right(nullptr) {}
huffman::node::node(char ch, int freq, node* left, node* right) : ch(ch), freq(freq), left(left), right(right) {}

bool huffman::compare::operator()(node* left, node* right) {
    return left->freq > right->freq;
}

void huffman::createHuffmanTree(const string& text) {
    for (char c : text) {
        frequancies[c]++;
    }

    priority_queue<node*, vector<node*>, compare> pq;
    for (auto& [ch, freq] : frequancies) {
        pq.push(new node(ch, freq));
    }
    while (pq.size() > 1) {
        node* left = pq.top();
        pq.pop();
        node* right = pq.top();
        pq.pop();
        pq.push(new node('\0', left->freq + right->freq, left, right));
    }
    root = pq.top();

    printHuffmanCode(root, "");

    for (auto pair : huffmanCode) {
        cout << pair.first << " " << pair.second << endl;
    }

}

void huffman::printHuffmanCode(node* node, string code) {
    if (node == nullptr){return;}
    if (node->left == nullptr && node->right == nullptr) {
        huffmanCode[node->ch] = code;
        return;
    }
    printHuffmanCode(node->left, code + "0");
    printHuffmanCode(node->right, code + "1");
}
