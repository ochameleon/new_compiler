#pragma once

#include <string>

#include "../token_kind.hpp"

using namespace std;

namespace my {

class Symbol {
private:
    bool is_terminal_;
    TokenKind terminal_;
    string nonterminal_;

public:
    Symbol(const TokenKind& terminal) : is_terminal_(true), terminal_(terminal), nonterminal_("") {}
    Symbol(const string& nonterminal) : is_terminal_(false), terminal_(TokenKind::BadLexeme), nonterminal_(nonterminal) {}
    bool is_terminal() const {
        return is_terminal_;
    }
    const string& nonterminal() const {
        return nonterminal_;
    }
    TokenKind terminal() const {
        return terminal_;
    }

    bool operator==(const Symbol& s) const {
        return is_terminal_ == s.is_terminal() && terminal_ == s.terminal() && nonterminal_ == s.nonterminal();
    }
};

}