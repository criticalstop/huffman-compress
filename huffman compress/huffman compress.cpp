#include "huffmantree.h"
#include<iostream>
#include<fstream>
#include<bitset>
#include<map>
using namespace std;

class huffmanEncode {
    ifstream file;
    huffmanTree<double>* tree;
    map<char, string> encrypt;
    map<string, char> decrypt;
    unsigned short minlen = -1;//min初始化为最大值
public:
    huffmanEncode() {};
    ~huffmanEncode() { this->file.close(); delete tree; };
    bool startEncryptFrom(string filename);
    bool printEncryptInstruction();
    bool saveKeyTo(string filename);
    bool saveFileTo(string filename);
    bool loadKeyFrom(string filename);
    bool DecryptFile(string encrypted_file, string decrypted_file);
    char decryptChar(string& code);
};

bool huffmanEncode::startEncryptFrom(string filename) {
    file.open(filename);//scanfile
    char c;
    map<char, double> allch;

    file.seekg(0, ios::beg);
    while (file.get(c)) {
        allch[c]++;
    }

    vector<double> vec;//startEncrypt
    double minvalue = 0.00000001;

    for (map<char, double>::iterator iter1 = allch.begin(); iter1 != allch.end(); iter1++) {//防止出现两个优先级相同的字符元素
        for (map<char, double>::iterator iter2 = (++iter1)--; iter2 != allch.end(); iter2++) {
            if (iter1->second == iter2->second) iter2->second += minvalue;
        }
        vec.push_back(iter1->second);
    }

    this->tree = new huffmanTree<double>(vec);

    string temp;
    for (auto iter = allch.begin(); iter != allch.end(); iter++) {
        temp = this->tree->encode(iter->second);
        this->encrypt[iter->first] = temp;
        this->decrypt[temp] = iter->first;
        if (temp.size() < this->minlen) minlen = temp.size();
    }
    //file.close();
    return true;
}

bool huffmanEncode::printEncryptInstruction() {
    if (this->encrypt.empty() || this->decrypt.empty()) return false;
    for (auto iter = encrypt.begin(); iter != encrypt.end(); iter++) {
        if (!isprint(iter->first)) cout << hex << reinterpret_cast<int*>(iter->first) << " : ";
        else if (iter->first == '\n') cout << "\"\\n\" : ";
        else cout << "\"" << iter->first << "\" : ";
        cout << iter->second << endl;
    }
    return true;
}

bool huffmanEncode::saveKeyTo(string filename) {
    ofstream file;
    file.open(filename, ios::out);//append为ios::app
    if (!file.is_open() || this->encrypt.empty()) return false;

    file << this->minlen << endl;
    for (auto iter = encrypt.begin(); iter != encrypt.end(); iter++) {
        if (iter->first == '\n') file << "\\n ";//\n是要分开储存的，不然输出流无法读取\n
        else if (iter->first == ' ') file << "space ";
        else file << iter->first << " ";
        file << iter->second << endl;
    }

    file.close();
    return true;
}

bool huffmanEncode::saveFileTo(string filename) {
    ofstream out_file;
    out_file.open(filename, ios::out | ios::binary);//append为ios::app
    if (!out_file.is_open()) return false;

    if (this->encrypt.empty() || this->decrypt.empty()) return false;
    file.clear();
    file.seekg(0, ios::beg);
    char c;
    unsigned short dummy_bits;
    string buffer;

    out_file << '\0';//保留开始的1字节；
    while (file.get(c)) {
        buffer.append(encrypt[c]);
        while (buffer.size() >= 8) {
            bitset<8> bin_num(buffer.substr(0, 8));
            unsigned short num = bin_num.to_ulong();
            out_file.write(reinterpret_cast<const char*>(&num), 1);//一点一点的写入二进制数至二进制文件中
            buffer.erase(0, 8);
        }
    }

    //开始的一字节表示加密文件的最后一字节的有效位数
    if (!buffer.empty()) {//加密后的文件没有八位对齐
        dummy_bits = 8 - buffer.size();
        bitset<8> bin_num(buffer.insert(buffer.size(), 8 - buffer.size(), '0'));
        unsigned short num = bin_num.to_ulong();
        out_file.write(reinterpret_cast<const char*>(&num), 1);
        out_file.seekp(0, ios::beg);
        out_file.write(reinterpret_cast<const char*>(&dummy_bits), 1);
    }
    out_file.close();
    return true;
}

bool huffmanEncode::loadKeyFrom(string filename) {
    ifstream file;
    file.open(filename, ios::in);
    if (!file.is_open()) return false;
    string c, code;

    this->encrypt.clear();
    this->decrypt.clear();

    file >> this->minlen;
    while (file >> c >> code) {
        if (c == "\\n") {
            encrypt['\n'] = code;
            decrypt[code] = '\n';
        }
        else if (c == "space") {
            encrypt[' '] = code;
            decrypt[code] = ' ';
        }
        else {
            encrypt[*c.c_str()] = code;
            decrypt[code] = *c.c_str();
        }
    }
    file.close();
    return true;
}

bool huffmanEncode::DecryptFile(string encrypted_file, string decrypted_file) {
    ofstream out_file;
    out_file.open(decrypted_file, ios::out);//append为ios::app
    if (!out_file.is_open()) return false;
    ifstream in_file;
    in_file.open(encrypted_file, ios::in | ios::binary);
    if (!in_file.is_open() || in_file.peek() == EOF) return false;

    string buffer;
    char c, result;
    bitset<8> bin;

    in_file.get(c);
    unsigned short dummy_bits = (unsigned short)c;
    while (in_file.get(c)) {
        bitset<8> bin(c);
        buffer.append(bin.to_string());
        if (!buffer.empty() && in_file.peek() == EOF) {//下一个字符为eof
            buffer.erase(buffer.size() - dummy_bits, dummy_bits);
        }
        while (1) {
            result = decryptChar(buffer);
            if (result != '\0') out_file << result;
            else break;
        }
    }
    in_file.close();
    out_file.close();
    return true;
}

char huffmanEncode::decryptChar(string& code) {
    string subcode;
    for (int i = this->minlen; i <= code.size(); i++) {
        subcode = code.substr(0, i);
        auto isfind = decrypt.find(subcode);//isfind是送代器：(
        if (isfind == decrypt.end()) continue;//没找到
        code.erase(0, i);
        return decrypt[subcode];//找到了
    }
    return '\0';
}

int main() {
    huffmanEncode story;
    story.startEncryptFrom("D://test3.txt");
    //story.printEncryptInstruction();
    story.saveFileTo("D://test_encrypted.bin");
    story.saveKeyTo("D://keys.txt");
    story.loadKeyFrom("D://keys.txt");
    story.DecryptFile("D://test_encrypted.bin", "D://test_decrypted.txt");
    return 0;
}