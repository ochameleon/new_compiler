#pragma once

#include <functional>
#include <string>

#include "../text/text_source.hpp"
#include "../../diagnostics/diagnostic_list.hpp"
#include "../token.hpp"
#include "../token_kind.hpp"

namespace my {

class Lexer {
private:
    TextSource source_;
    int position_ = 0;
    DiagnosticList diagnostics_ = {};

public:
    Lexer(const TextSource& source) : source_(source) {
    }

    string peek(const int& offset) {
        if (position_ + offset >= source_.text().size()) return "";
        else return source_.text().at(position_ + offset);
    }
    string current() {
        return peek(0);
    }
    string lookahead() {
        return peek(1);
    }

    string consume(const function<bool(const string&)>& predicate) {
        std::string accumulation;
        while (position_ < source_.text().size() && predicate(source_.text().at(position_))) {
            accumulation.append(source_.text().at(position_));
            ++position_;
        }
        return accumulation;
    }

    string read_spacing() {
        return consume(is_spacing);
    }
    string read_name() {
        return consume(is_allowed);
    }

    Token lex() {
        int begin = position_;
        TokenKind kind;
        string text;

        string c = this->current();

        if (c == " " || c == "\t") {
            this->read_spacing();
            kind = TokenKind::WhiteSpace;
        }
        else if (c == "\n") {
            position_++;
            kind = TokenKind::NewLine;
        }
        else if (c == "\r") {
            position_++;
            kind = TokenKind::NewLine;
        }
        else if (c == "") {
            kind = TokenKind::EndOfFile;
        }
        else if (is_special(c)) {
            position_++;
            auto it = std::find_if(
                special_character_to_kind.begin(),
                special_character_to_kind.end(),
                [&](auto& p) { return p.first == c; });
            if (it != special_character_to_kind.end())
                kind = it->second;
        }
        else {
            this->read_name();
            kind = TokenKind::Identifier;
        }

        if (kind == TokenKind::EndOfFile) {
            text = "";
        } else {
            text = source_.to_string(TextSpan(begin, position_ - begin));
        }

        return Token(kind, position_, text);
    }
};

}