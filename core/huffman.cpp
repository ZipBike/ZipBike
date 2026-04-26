//
// Created by IliyaD on 26.02.2026.
//

#include "huffman.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

huffman::node::node(char ch, int freq) : ch(ch), freq(freq), left(nullptr), right(nullptr) {}
huffman::node::node(char ch, int freq, node* left, node* right) : ch(ch), freq(freq), left(left), right(right) {}

bool huffman::compare::operator()(node* left, node* right) {
    return left->freq > right->freq;
}

void huffman::createHuffmanTree(const string& text) {
   frequencies.clear();
    for (char ch : text) {
        frequencies[ch]++;
    }
    buildHuffmanTree();
}

void huffman::buildHuffmanTree() {
    priority_queue<node*, vector<node*>, compare> pq;
    for (auto& [ch, freq] : frequencies) {
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
    huffmanCode.clear();
    printHuffmanCode(root, "");
}

void huffman::printHuffmanCode(node* node, const string& code) {
    if (node == nullptr){return;}
    if (node->left == nullptr && node->right == nullptr) {
        huffmanCode[node->ch] = code;
        return;
    }
    printHuffmanCode(node->left, code + "0");
    printHuffmanCode(node->right, code + "1");
}

void huffman::compressFile(const std::string& inputF, const std::string& outputF) {
    ifstream in(inputF.c_str(), ios::binary);
    if (!in.is_open()) {
        cerr << "Could not open file " << inputF << endl;
        return;
    }
    stringstream buffer;
    buffer << in.rdbuf();
    string text = buffer.str();
    in.close();
    createHuffmanTree(text);
    string output;
    for (char c : text) {
        output += huffmanCode[c];
    }
    ofstream out(outputF.c_str(), ios::binary);
    if (!out.is_open()) {
        cerr << "Could not open file " << outputF << endl;
        return;
    }
    size_t padding = (8 - (output.length() % 8)) % 8;
    auto paddingByte = static_cast<unsigned char>(padding);
    out.write(reinterpret_cast<char*>(&paddingByte), 1);
    size_t mapSize = frequencies.size();
    out.write(reinterpret_cast<char*>(&mapSize), sizeof(size_t));
    for (auto& pair : frequencies) {
        out.write(reinterpret_cast<const char*>(&pair.first), sizeof(char));
        out.write(reinterpret_cast<const char*>(&pair.second), sizeof(int));
    }
    for (size_t i = 0; i < output.length(); i+=8) {
        string byteText = output.substr(i, 8);
        while (byteText.length() < 8) {
            byteText.append("0");
        }
        auto byte = static_cast<unsigned char>(stoul(byteText, nullptr, 2));
        out.write(reinterpret_cast<char*>(&byte), 1);
    }
    out.close();
    cout << "The file is successfully compressed" << endl;
}

void huffman::decompressFile(const std::string& inputF, const std::string& outputF) {
    ifstream in(inputF.c_str(), ios::binary);
    if (!in.is_open()) {
        cerr << "Could not open file " << inputF << endl;
        return;
    }
    unsigned char paddingByte;
    in.read(reinterpret_cast<char*>(&paddingByte), 1);
    int padding = static_cast<unsigned char>(paddingByte);
    size_t mapSize;
    in.read(reinterpret_cast<char*>(&mapSize), sizeof(size_t));
    frequencies.clear();
    for (size_t i = 0; i < mapSize; i++) {
        char ch;
        int freq;
        in.read(reinterpret_cast<char*>(&ch), sizeof(char));
        in.read(reinterpret_cast<char*>(&freq), sizeof(int));
        frequencies[ch] = freq;
    }
    buildHuffmanTree();
    string text;
    char byte;
    while (in.read(&byte, 1)) {
        for (int i = 7; i >= 0; i--) {
            if (byte >> i & 1) {
                text += '1';
            }else {
                text += '0';
            }
        }
    }
    in.close();
    if (padding > 0 && text.length() >= padding) {
        text.erase(text.length() - padding, padding);
    }
    string finalText;
    node* curr = root;
    for (char bit : text) {
        if (bit == '0') {
            curr = curr->left;
        }else {
            curr = curr->right;
        }
        if (curr->left == nullptr && curr->right == nullptr) {
            finalText += curr->ch;
            curr = root;
        }
    }
    ofstream out(outputF.c_str());
    if (!out.is_open()) {
        cerr << "Could not open file " << outputF << endl;
        return;
    }
    out << finalText;
    out.close();
    cout << "The file is successfully decompressed" << endl;
}