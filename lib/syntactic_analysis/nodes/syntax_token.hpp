#pragma once

#include <string>

#include "syntax_node.hpp"
#include "../token_kind.hpp"
#include "../text/text_span.hpp"

using namespace std;

namespace my {

class SyntaxToken : public SyntaxNode {
private:
    int position_;
    string text_;

public:
    SyntaxToken(const TokenKind& kind, int position, const string& text) : SyntaxNode(kind, {}), position_(position), text_(text) {
    }

    bool is_SyntaxToken() const override {
        return true;
    }

    int position() const {
        return position_;
    }
    const string& text() const {
        return text_;
    }

    TextSpan span() const override {
        return TextSpan(position_, text_.size());
    }
};

}