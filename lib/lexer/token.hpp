#pragma once

#include <string>

#include "../syntax/syntax_node.hpp"
#include "token_kind.hpp"
#include "../text/text_span.hpp"

namespace my {

class Token : public SyntaxNode {
private:
    int position_;
    std::string text_;

public:
    Token(const TokenKind& kind, int position, const std::string& text) : SyntaxNode(kind, {}), position_(position), text_(text) {
    }

    bool is_Token() const override {
        return true;
    }

    int position() const {
        return position_;
    }
    const std::string& text() const {
        return text_;
    }

    TextSpan span() const override {
        return TextSpan(position_, text_.size());
    }
};

}
