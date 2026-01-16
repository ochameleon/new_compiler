#pragma once

#include <string>
#include <iostream>

#include "../token_kind.hpp"

using namespace std;

namespace my {

class Symbol {
private:
    bool is_terminal_ = false;
    TokenKind terminal_ = TokenKind::BadLexeme;
    string nonterminal_ = "";

public:
    Symbol(const TokenKind& terminal) : is_terminal_(true), terminal_(terminal), nonterminal_("") {}
    Symbol(const string& nonterminal) : is_terminal_(false), terminal_(TokenKind::BadLexeme), nonterminal_(nonterminal) {}
    
    // Explicitly define copy constructor
    Symbol(const Symbol& other) : is_terminal_(other.is_terminal()), terminal_(other.terminal()), nonterminal_(other.nonterminal()) {}
    
    // Explicitly define assignment operator
    Symbol& operator=(const Symbol& other) {
        if (this != &other) {
            is_terminal_ = other.is_terminal();
            terminal_ = other.terminal();
            nonterminal_ = other.nonterminal();
        }
        return *this;
    }
    
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