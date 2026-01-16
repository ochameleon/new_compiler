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

    // Explicit copy constructor
    Rule(const Rule& other) : lhs_(other.lhs()), rhs_(other.rhs()), precedence_(other.precedence()) {}
    
    // Explicit assignment operator
    Rule& operator=(const Rule& other) {
        if (this != &other) {
            lhs_ = other.lhs();
            rhs_ = other.rhs();
            precedence_ = other.precedence();
        }
        return *this;
    }

    const string& lhs() const { return lhs_; }
    const vector<Symbol>& rhs() const { return rhs_; }
    int precedence() const { return precedence_; }

    bool operator==(const Rule& r) const {
        return lhs_ == r.lhs() && rhs_ == r.rhs() && precedence_ == r.precedence();
    }
};

}