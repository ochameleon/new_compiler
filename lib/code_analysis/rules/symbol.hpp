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
    Symbol(const TokenKind& terminal) : terminal_(terminal) {
        is_terminal_ = true;
    }
    Symbol(const string& nonterminal) : nonterminal_(nonterminal_) {
        is_terminal_ = false;
    }
    bool is_terminal(){
        return is_terminal_;
    }
};

}