#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <sstream>

#include "../../diagnostics/diagnostic_list.hpp"
#include "../steps/lexer.hpp"
#include "../rules/rule.hpp"

using namespace std;

namespace my {

class EarleyItem {
private:
    Rule rule_;
    int dot_;
    int origin_;

public:
    EarleyItem(Rule rule, int dot, int origin) : rule_(rule), dot_(dot), origin_(origin) {
    }

    Rule rule() const { return rule_; }
    int dot() const { return dot_; }
    int origin() const { return origin_; }

    EarleyItem& operator=(const EarleyItem& other) {
        if (this != &other) {
            rule_ = other.rule();
            dot_ = other.dot();
            origin_ = other.origin();
        }
        return *this;
    }

    bool operator==(EarleyItem o) const {
        return rule_ == o.rule() && dot_ == o.dot() && origin_ == o.origin();
    }
};

class EarleyParser {
private:
    Lexer lexer_;
    DiagnosticList diagnostics_ = {};
    vector<Rule> rules_;

    vector<Token> tokens_;
    vector<vector<EarleyItem>> chart_;
    std::unordered_map<std::string, std::shared_ptr<SyntaxNode>> memo_;

    void print_item_(const EarleyItem& item) {
        cout << "[" << item.rule().lhs() << " -> ";
        int dot = item.dot();
        const vector<Symbol>& rhs = item.rule().rhs();
        for (int i = 0; i < (int)rhs.size(); i++) {
            if (i == dot) cout << "• ";
            const Symbol& s = rhs[i];
            cout << "{T=" << s.is_terminal() << ":";
            if (s.is_terminal()) {
                cout << "TERM";
            } else {
                cout << s.nonterminal();
            }
            cout << "} ";
        }
        if (dot == (int)rhs.size()) cout << "•";
        cout << ", origin=" << item.origin() << "]";
    }

    void print_chart_at_position_(int position) {
        cout << "  Chart[" << position << "]:" << endl;
        for (int i = 0; i < (int)chart_.at(position).size(); i++) {
            cout << "    ";
            print_item_(chart_.at(position).at(i));
            cout << endl;
        }
    }

    bool add_to_chart_(int position, const EarleyItem& item) {
        for (const auto& existing_item : chart_.at(position)) {
            if (existing_item == item) {
                return false;
            }
        }
        chart_.at(position).push_back(item);
        return true;
    }

public:
    EarleyParser(Lexer lexer, vector<Rule> rules) : lexer_(lexer), rules_(rules) {
    }

    const vector<vector<EarleyItem>>& chart() const { return chart_; }
    const vector<Token>& tokens() const { return tokens_; }

    void tokenize_all() {
        while (true) {
            Token t = lexer_.lex();
            tokens_.push_back(t);
            if (t.kind() == TokenKind::EndOfFile) break;
        }
    }

    void initialize_chart() {
        vector<EarleyItem> initial_items = {};
        for (int i = 0; i < rules_.size(); i++) {
            const Rule rule = rules_.at(i);
            if (rule.lhs() == "start") {
                EarleyItem initial_item(rule, 0, 0);
                initial_items.push_back(initial_item);
                cout << "  [INIT] Added: ";
                print_item_(initial_item);
                cout << endl;
            }
        }

        chart_.push_back(initial_items);
        chart_.resize(tokens_.size() + 1);
    }

    void completer(int position, EarleyItem completed_item) {
        cout << "  [COMPLETER at pos " << position << "] ";
        print_item_(completed_item);
        cout << endl;

        int origin = completed_item.origin();
        string completed_nonterminal = completed_item.rule().lhs();

        for (int i = 0; i < (int)chart_.at(origin).size(); i++) {
            const EarleyItem parent_item = chart_.at(origin).at(i);
            const Rule parent_rule = parent_item.rule();
            int parent_dot = parent_item.dot();
            int parent_rhs_size = parent_rule.rhs().size();

            if (parent_dot < parent_rhs_size) {
                const Symbol symbol_at_dot = parent_rule.rhs().at(parent_dot);

                if (!symbol_at_dot.is_terminal() && symbol_at_dot.nonterminal() == completed_nonterminal) {
                    EarleyItem advanced_item(parent_rule, parent_dot + 1, parent_item.origin());
                    if (add_to_chart_(position, advanced_item)) {
                        cout << "    -> Added: ";
                        print_item_(advanced_item);
                        cout << endl;
                    }
                }
            }
        }
    }

    void predictor(int position, EarleyItem item) {
        cout << "  [PREDICTOR at pos " << position << "] ";
        print_item_(item);
        cout << endl;

        const Rule rule = item.rule();
        int dot = item.dot();
        const Symbol symbol_at_dot = rule.rhs().at(dot);

        int total_rules = rules_.size();
        for (int i = 0; i < total_rules; i++) {
            const Rule candidate_rule = rules_.at(i);
            if (candidate_rule.lhs() == symbol_at_dot.nonterminal()) {
                EarleyItem new_item(candidate_rule, 0, position);
                if (add_to_chart_(position, new_item)) {
                    cout << "    -> Added: ";
                    print_item_(new_item);
                    cout << endl;
                }
            }
        }
    }

    void scanner(int position, EarleyItem item) {
        cout << "  [SCANNER at pos " << position << "] ";
        print_item_(item);
        cout << endl;

        if (position >= (int)tokens_.size()) {
            cout << "    -> At end of input" << endl;
            return;
        }

        const Rule rule = item.rule();
        int dot = item.dot();
        const Symbol symbol_at_dot = rule.rhs().at(dot);
        TokenKind expected_terminal = symbol_at_dot.terminal();

        const Token current_token = tokens_.at(position);
        TokenKind current_token_kind = current_token.kind();

        cout << "    Expected token, got: " << current_token.text() << endl;

        if (expected_terminal == current_token_kind) {
            EarleyItem advanced_item(rule, dot + 1, item.origin());
            if (add_to_chart_(position + 1, advanced_item)) {
                cout << "    -> MATCH! Added at pos " << (position + 1) << ": ";
                print_item_(advanced_item);
                cout << endl;
            }
        } else {
            cout << "    -> No match" << endl;
        }
    }

    bool recognize() {
        tokenize_all();
        initialize_chart();

        for (int k = 0; k <= tokens_.size(); k++) {
            cout << "POSITION " << k << endl;
            int i = 0;
            while (i < chart_.at(k).size()) {
                EarleyItem item = chart_.at(k).at(i);
                Rule rule = item.rule();
                int dot = item.dot();
                int size = rule.rhs().size();

                if (dot == size) {
                    completer(k, item);
                }
                else if (dot < size) {
                    const Symbol next_symbol = rule.rhs().at(dot);
                    if (next_symbol.is_terminal()) {
                        scanner(k, item);
                    }
                    else {
                        predictor(k, item);
                    }
                }
                i++;
            }
            print_chart_at_position_(k);
            cout << endl;
        }

        int final_position = tokens_.size();
        for (int i = 0; i < (int)chart_.at(final_position).size(); i++) {
            const EarleyItem item = chart_.at(final_position).at(i);
            const Rule rule = item.rule();
            int dot = item.dot();
            int size = rule.rhs().size();
            int origin = item.origin();

            if (dot == size && origin == 0) {
                return true;
            }
        }
        return false;
    }

    std::shared_ptr<SyntaxNode> parse() {
        return nullptr;
    }
};

}
