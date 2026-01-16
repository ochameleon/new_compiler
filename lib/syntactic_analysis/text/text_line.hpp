#pragma once

#include "text_span.hpp"

using namespace std;

namespace my {

class TextLine {
private:
    int begin_;
    int length_;
    int length_with_linebreak_;

public:
    TextLine (const int& begin, const int& length, const int& length_with_linebreak) : begin_(begin), length_(length), length_with_linebreak_(length_with_linebreak) {
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

    int length_with_linebreak() const {
        return length_with_linebreak_;
    }

    TextSpan span() const {
        return TextSpan(begin_, length_);
    }

    TextSpan span_with_linebreak() const {
        return TextSpan(begin_, length_with_linebreak_);
    }
};

}