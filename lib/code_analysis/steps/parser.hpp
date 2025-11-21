#include "../diagnostics/diagnostic_list.hpp"
#include "../steps/lexer.hpp"
#include "../rules/rule_list.hpp"

using namespace std;

namespace my {

struct Earley {
    int rule;
    int dot;
    int origin;

    bool operator==(const Earley& o) const {
        return rule == o.rule && dot == o.dot && origin == o.origin;
    }
};

class Parser {
private:
    Lexer lexer_;
    DiagnosticList diagnostics_ = {};
    RuleList rules_;

    vector<SyntaxToken> tokens_;

    void tokenize_all() {
        while (true) {
            SyntaxToken t = lexer_.lex();
            tokens_.push_back(t);
            if (t.kind() == TokenKind::EndOfFile) break;
        }
    }
public:
    Parser(Lexer lexer, RuleList rules) : lexer_(lexer), rules_(rules) {
    }
};

}
