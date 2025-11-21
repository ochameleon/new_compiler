#pragma once

#include "text_source.hpp"
#include "text_span.hpp"

using namespace std;

namespace my {

class TextLocation {
private:
    TextSource source_;
    TextSpan span_;

public:
    TextLocation(TextSource source, TextSpan span) : source_(source), span_(span) {
    }

    TextSource source() const {
        return source_;
    }

    TextSpan span() const {
        return span_;
    }

    int begin_line() const {
        return source_.line_index(span_.begin());
    }
    int end_line() const {
        return source_.line_index(span_.end());
    }

    int begin_character( )const {
        return span_.begin() - source_.lines().at(this->begin_line()).begin();
    }
    int end_character() const {
        return span_.end() - source_.lines().at(this->end_line()).begin();
    }
};

}