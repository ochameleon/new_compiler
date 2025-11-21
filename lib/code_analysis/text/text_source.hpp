#pragma once

#include <string>
#include <vector>

#include "text_line.hpp"
#include "text_span.hpp"

using namespace std;

namespace my {

class TextSource {
private:
    vector<string> text_;
    string file_name_;
    vector<TextLine> lines_;

    vector<TextLine> split_lines() const {
        vector<TextLine> lines;
        int position = 0;
        int line_start = 0;

        while (position < text_.size()) {
            int line_break_width = line_break_width_(position);

            if (line_break_width == 0) {
                position += 1;
            } else {
                lines.push_back(line_add(position, line_start, line_break_width));
                position += line_break_width;
                line_start = position;
            }
        }

        if (position >= line_start) {
            lines.push_back(line_add(position, line_start, 0));
        }

        return lines;
    }

    TextLine line_add(int position, int line_start, int line_break_width) const {
        int line_length = position - line_start;
        int line_length_with_break = line_length + line_break_width;
        return TextLine(line_start, line_length, line_length_with_break);
    }

    int line_break_width_(int position) const {
        string current = text_[position];
        string next = (position + 1 < static_cast<int>(text_.size())) ? text_[position + 1] : "";

        if (current == "\r" && next == "\n") return 2;
        if (current == "\r" || current == "\n") return 1;
        return 0;
    }

public:
    TextSource(const vector<string>& text, const string& file_name) : text_(text), file_name_(file_name) {
        this->lines_ = split_lines();
    }

    vector<string> text() const {
        return text_;
    }

    vector<TextLine> lines() const {
        return lines_;
    }

    string file_name() const {
        return file_name_;
    }

    int line_index(int position) const {
        int lower = 0;
        int upper = lines_.size() - 1;

        while (lower <= upper) {
            int index = lower + (upper - lower) / 2;
            auto intermediate = lines_.at(index);
            int begin = intermediate.begin();

            if (position == begin) {
                lower = index + 1;
                upper = index;
            } else if (begin > position) {
                upper = index - 1;
            } else {
                lower = index + 1;
            }
        }
        return lower - 1;
    }

    string to_string(const TextSpan& span) const {
        string result;
        for (int i = span.begin(); i < span.end(); ++i) {
            result.append(text_.at(i));
        }
        return result;
    }
};

}