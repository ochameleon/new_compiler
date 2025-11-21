#include <string>
#include <vector>

#include "symbol.hpp"

using namespace std;

namespace my {

class Rule {
private:
    string lhs_;
    vector<Symbol> rhs_;
    int precedence_;
public:
    Rule(const string& lhs, const vector<Symbol>& rhs, const int& precedence) : lhs_(lhs), rhs_(rhs), precedence_(precedence) {
    }
};

}