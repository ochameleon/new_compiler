#pragma once

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

    const string& lhs() const { return lhs_; }
    const vector<Symbol>& rhs() const { return rhs_; }
    int precedence() const { return precedence_; }

    bool operator==(const Rule& r) const {
        return lhs_ == r.lhs() && rhs_ == r.rhs() && precedence_ == r.precedence();
    }
};

}