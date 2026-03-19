#pragma once

#include <sstream>
#include <string>

#include "severity.hpp"
#include "../text/text_location.hpp"

using namespace std;

namespace my {

class Diagnostic {
private:
    TextLocation location_;
    string message_;
    Severity severity_;

public:
    Diagnostic(const TextLocation& location, const string& message, const Severity& severity) : location_(location), message_(message), severity_(severity) {
    }

    string to_string() {
        ostringstream buffer;

        TextSource source = location_.source();

        std::string file_name = source.file_name();

        int begin_line = location_.begin_line() + 1;
        int end_line = location_.end_line() + 1;
        
        int begin_character = location_.begin_character() + 1;
        int end_character = location_.end_character() + 1;

        TextSpan error_span = location_.span();
        int line_index = source.line_index(error_span.begin());
        TextLine line = source.lines().at(line_index);

        buffer << "File \"" << file_name << "\", lines "
               << begin_line << " - " << end_line
               << ", characters " << begin_character << " - " << end_character
               << ":\n";

        TextSpan prefix_span(line.begin(), error_span.begin() - line.begin());
        TextSpan suffix_span(error_span.begin() + error_span.length(), (line.begin() + line.length()) - (error_span.begin() + error_span.length()));

        string prefix = source.to_string(prefix_span);
        string error = source.to_string(error_span);
        string suffix = source.to_string(suffix_span);

        for (int line_no = begin_line; line_no <= end_line; ++line_no) {
            buffer << line_no << " | " << prefix << error << suffix << "\n";
        }

        /* REPLACE .size() WITH THE ACTUAL UNICODE SIZE */
        if (!error.empty()) {
            string caret(prefix.size(), ' ');
            caret += string(error.size(), '^');
            buffer << "    " << caret;
        }
        buffer << "\n" << message_;

        return buffer.str();
    }
};

}
