#pragma once

#include <vector>

#include "rule.hpp"

using namespace std;

namespace my {

class RuleList {
private:
    vector<Rule> rules_ = {};
public:
    RuleList(const vector<Rule>& rules) : rules_(rules){
    }

    const vector<Rule>& rules() const { return rules_; }
};

}