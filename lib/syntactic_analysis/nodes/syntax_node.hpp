#include <vector>
#include <memory>
#include <stdexcept>
#include <iostream>

#include "../text/text_span.hpp"
#include "../token_kind.hpp"

using namespace std;

namespace my {

class SyntaxNode {
protected:
    TokenKind kind_;
    vector<shared_ptr<SyntaxNode>> children_;

public:
    SyntaxNode(const TokenKind& kind, const vector<shared_ptr<SyntaxNode>>& children)
        : kind_(kind), children_(children) {}

    virtual ~SyntaxNode() = default;

    const TokenKind& kind() const {
        return kind_;
    }
    const vector<shared_ptr<SyntaxNode>>& children() const {
        return children_;
    }

    virtual TextSpan span() const {
        if (children_.empty()) {
            return TextSpan(-1, 0);
        }
        const auto& first = children_.front();
        const auto& last  = children_.back();
        return TextSpan(first->span().begin(), last->span().end() - last->span().begin());
    }

    virtual bool is_SyntaxToken() const {
        return false;
    }
};

}
