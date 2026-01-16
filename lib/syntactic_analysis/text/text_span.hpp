#pragma once

using namespace std;

namespace my {

class TextSpan {
private:
    int begin_;
    int length_;

public:
    TextSpan(const int& begin, const int& length) : begin_(begin), length_(length) {
    }

    int begin() const {
        return begin_;
    }
    int length() const {
        return length_;
    }
    int end() const {
        return begin_ + length_;
    }

};

}