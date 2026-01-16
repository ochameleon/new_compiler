#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace my {

vector<string> read(const string& filename) {
    ifstream file(filename, ios::binary);
    vector<string> atoms;

    if (!file)
        throw runtime_error("Cannot open file");

    char c;
    while (file.get(c)) {
        unsigned char b0 = static_cast<unsigned char>(c);
        size_t n = 0;

        if (b0 <= 0x7F)                    n = 1;
        else if (b0 >= 0xC2 && b0 <= 0xDF) n = 2;
        else if (b0 >= 0xE0 && b0 <= 0xEF) n = 3;
        else if (b0 >= 0xF0 && b0 <= 0xF4) n = 4;
        else throw runtime_error("Invalid UTF-8 leading byte");

        string u(1, c);

        for (size_t i = 1; i < n; ++i) {
            if (!file.get(c))
                throw runtime_error("Truncated UTF-8");

            unsigned char bi = static_cast<unsigned char>(c);
            if ((bi & 0xC0) != 0x80)
                throw runtime_error("Invalid UTF-8 continuation byte");

            u += c;
        }

        // Scalar-value exclusions
        if (n == 3) {
            unsigned char b1 = static_cast<unsigned char>(u[1]);
            if (b0 == 0xE0 && b1 < 0xA0) throw runtime_error("Overlong UTF-8");
            if (b0 == 0xED && b1 >= 0xA0) throw runtime_error("UTF-16 surrogate");
        }
        else if (n == 4) {
            unsigned char b1 = static_cast<unsigned char>(u[1]);
            if (b0 == 0xF0 && b1 < 0x90)
                throw runtime_error("Overlong UTF-8");
            if (b0 == 0xF4 && b1 > 0x8F)
                throw runtime_error("Codepoint out of range");
        }

        atoms.push_back(u);
    }

    return atoms;
}

}