#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace my {

vector<string> read(const string& filename) {
    ifstream file(filename, ios::binary);
    vector<string> codepoints;

    if (!file) {
        throw runtime_error("Cannot open file");
    }

    string buffer;
    char c;
    while (file.get(c)) {
        unsigned char byte = c;

        size_t bytes_in_char = 0;
        if (byte <= 0x7F) bytes_in_char = 1;
        else if ((byte & 0xE0) == 0xC0) bytes_in_char = 2;
        else if ((byte & 0xF0) == 0xE0) bytes_in_char = 3;
        else if ((byte & 0xF8) == 0xF0) bytes_in_char = 4;
        else throw runtime_error("Invalid UTF-8");

        string utf8_char(1, c);
        for (size_t i = 1; i < bytes_in_char; ++i) {
            if (!file.get(c)) throw runtime_error("Truncated UTF-8");
            utf8_char += c;
        }
        codepoints.push_back(utf8_char);
    }

    return codepoints;
}

}