#pragma once

#include "../../diagnostics/diagnostic_list.hpp"
#include "../steps/lexer.hpp"
#include "../rules/rule_list.hpp"

using namespace std;

namespace my {

class Earley {
private:
    Rule rule_;
    int dot_;
    int origin_;

public:
    Earley(Rule rule, int dot, int origin) : rule_(rule), dot_(dot), origin_(origin) {
    }

    Rule rule() const { return rule_; }
    int dot() const { return dot_; }
    int origin() const { return origin_; }

    bool operator==(const Earley& o) const {
        return rule_ == o.rule() && dot_ == o.dot() && origin_ == o.origin();
    }
};

class Parser {
private:
    Lexer lexer_;
    DiagnosticList diagnostics_ = {};
    RuleList rules_;

    vector<SyntaxToken> tokens_;
    vector<vector<Earley>> chart_;

    void tokenize_all_() {
        while (true) {
            SyntaxToken t = lexer_.lex();
            tokens_.push_back(t);
            if (t.kind() == TokenKind::EndOfFile) break;
        }
    }

    void initialize_chart_() {
        chart_.clear();
        chart_.resize(tokens_.size() + 1);
    }

    void print_item_(const Earley& item) {
        cout << "[" << item.rule().lhs() << " -> ";
        int dot = item.dot();
        const vector<Symbol>& rhs = item.rule().rhs();
        for (int i = 0; i < (int)rhs.size(); i++) {
            if (i == dot) cout << "• ";
            const Symbol& sym = rhs[i];
            cout << "{T=" << sym.is_terminal() << ":";
            if (sym.is_terminal()) {
                cout << "TERM";
            } else {
                cout << sym.nonterminal();
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

    void completer(int position, const Earley& completed_item) {
        cout << "  [COMPLETER at pos " << position << "] ";
        print_item_(completed_item);
        cout << endl;
        
        // When a rule is completed, look back at the origin position
        // Find all items waiting for this completed nonterminal
        int origin = completed_item.origin();
        string completed_nonterminal = completed_item.rule().lhs();
        
        // Look at all items at the origin position
        for (int i = 0; i < (int)chart_.at(origin).size(); i++) {
            const Earley& parent_item = chart_.at(origin).at(i);
            const Rule& parent_rule = parent_item.rule();
            int parent_dot = parent_item.dot();
            int parent_rhs_size = parent_rule.rhs().size();
            
            // Check if this parent item is waiting for a nonterminal at its dot
            if (parent_dot < parent_rhs_size) {
                const Symbol& symbol_at_dot = parent_rule.rhs().at(parent_dot);
                
                // If it's a nonterminal and matches what we just completed
                if (!symbol_at_dot.is_terminal() && symbol_at_dot.nonterminal() == completed_nonterminal) {
                    // Advance the parent item and add to current position
                    Earley advanced_item(parent_rule, parent_dot + 1, parent_item.origin());
                    chart_.at(position).push_back(advanced_item);
                    cout << "    -> Added: ";
                    print_item_(advanced_item);
                    cout << endl;
                }
            }
        }
    }

    void predictor(int position, const Earley& item) {
        cout << "  [PREDICTOR at pos " << position << "] ";
        print_item_(item);
        cout << endl;
        
        // When we need to parse a nonterminal, add all rules for that nonterminal
        const Rule& rule = item.rule();
        int dot = item.dot();
        const Symbol& symbol_at_dot = rule.rhs().at(dot);
        string nonterminal = symbol_at_dot.nonterminal();
        
        cout << "    Looking for rules with LHS = " << nonterminal << endl;
        
        // Find all rules where LHS matches this nonterminal
        int total_rules = rules_.rules().size();
        for (int i = 0; i < total_rules; i++) {
            const Rule& candidate_rule = rules_.rules().at(i);
            if (candidate_rule.lhs() == nonterminal) {
                // Create new item: [candidate_rule, 0, position]
                Earley new_item(candidate_rule, 0, position);
                chart_.at(position).push_back(new_item);
                cout << "    -> Added: ";
                print_item_(new_item);
                cout << endl;
            }
        }
    }

    void scanner(int position, const Earley& item) {
        cout << "  [SCANNER at pos " << position << "] ";
        print_item_(item);
        cout << endl;
        
        // When we see a terminal, consume the next token if it matches
        // Check if we're not at the end of input
        if (position >= (int)tokens_.size()) {
            cout << "    -> At end of input" << endl;
            return;
        }
        
        const Rule& rule = item.rule();
        int dot = item.dot();
        const Symbol& symbol_at_dot = rule.rhs().at(dot);
        TokenKind expected_terminal = symbol_at_dot.terminal();
        
        // Get current token
        const SyntaxToken& current_token = tokens_.at(position);
        TokenKind current_token_kind = current_token.kind();
        
        cout << "    Expected token, got: " << current_token.text() << endl;
        
        // If it matches, advance the dot
        if (expected_terminal == current_token_kind) {
            Earley advanced_item(rule, dot + 1, item.origin());
            chart_.at(position + 1).push_back(advanced_item);
            cout << "    -> MATCH! Added at pos " << (position + 1) << ": ";
            print_item_(advanced_item);
            cout << endl;
        } else {
            cout << "    -> No match" << endl;
        }
    }

public:
    Parser(Lexer lexer, RuleList rules) : lexer_(lexer), rules_(rules) {
    }

    const vector<vector<Earley>>& chart() const { return chart_; }
    const vector<SyntaxToken>& tokens() const { return tokens_; }

    void seed_chart_() {
        // Add initial items for the start symbol
        // Find all rules where LHS = "Start" and add them to chart[0]
        int total_rules = rules_.rules().size();
        for (int i = 0; i < total_rules; i++) {
            const Rule& rule = rules_.rules().at(i);
            if (rule.lhs() == "Start") {
                Earley initial_item(rule, 0, 0);
                chart_.at(0).push_back(initial_item);
                cout << "  [INIT] Added: ";
                print_item_(initial_item);
                cout << endl;
            }
        }
    }

    bool parse() {
        tokenize_all_();
        
        cout << "=== Starting Earley Parse ===" << endl;
        cout << "Total tokens: " << tokens_.size() << endl;
        cout << "Total rules: " << rules_.rules().size() << endl << endl;

        initialize_chart_();
        seed_chart_();

        for (int k = 0; k <= (int)tokens_.size(); k++) {
            cout << "POSITION " << k << endl;
            for (int i = 0; i < (int)chart_.at(k).size(); i++) {
                const Earley& item = chart_.at(k).at(i);
                const Rule& rule = item.rule();
                int dot = item.dot();
                int rhs_size = rule.rhs().size();
                
                if (dot == rhs_size) {
                    completer(k, item);
                }
                else if (dot < rhs_size) {
                    const Symbol& next_symbol = rule.rhs().at(dot);
                    if (next_symbol.is_terminal()) {
                        scanner(k, item);
                    }
                    else {
                        predictor(k, item);
                    }
                }
            }
            print_chart_at_position_(k);
            cout << endl;
        }
        
        cout << "=== Parse Complete ===" << endl;
        
        // Check if parse succeeded
        // Look for a completed item at the final position that started at origin 0
        int final_position = tokens_.size();
        for (int i = 0; i < (int)chart_.at(final_position).size(); i++) {
            const Earley& item = chart_.at(final_position).at(i);
            const Rule& rule = item.rule();
            int dot = item.dot();
            int rhs_size = rule.rhs().size();
            int origin = item.origin();
            
            // Check if this item is completed and started from the beginning
            if (dot == rhs_size && origin == 0) {
                // We successfully parsed the entire input
                cout << "SUCCESS: Found complete parse!" << endl;
                return true;
            }
        }
        
        cout << "FAILED: No complete parse found." << endl;
        return false;
    }
};

}
