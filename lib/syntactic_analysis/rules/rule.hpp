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
    bool associativity_;
public:
    Rule(string lhs, vector<Symbol> rhs, int precedence, bool associativity) : lhs_(lhs), rhs_(rhs), precedence_(precedence), associativity_(associativity){
    }

    // Explicit copy constructor
    Rule(const Rule& other) : lhs_(other.lhs()), rhs_(other.rhs()), precedence_(other.precedence()), associativity_(other.associativity()) {}
    
    // Explicit assignment operator
    Rule& operator=(const Rule& other) {
        if (this != &other) {
            lhs_ = other.lhs();
            rhs_ = other.rhs();
            precedence_ = other.precedence();
            associativity_ = other.associativity();
        }
        return *this;
    }

    string lhs() const { return lhs_; }
    vector<Symbol> rhs() const { return rhs_; }
    int precedence() const { return precedence_; }
    bool associativity() const { return associativity_; }

    bool operator==(Rule r) const {
        return lhs_ == r.lhs() && rhs_ == r.rhs() && precedence_ == r.precedence() && associativity_ == r.associativity();
    }
};

}