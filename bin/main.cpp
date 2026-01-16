#include <string>
#include <vector>

#include "../lib/syntactic_analysis/rules/rule_list.hpp"

#include "../lib/syntactic_analysis/steps/reader.hpp"
#include "../lib/syntactic_analysis/steps/lexer.hpp"
#include "../lib/syntactic_analysis/steps/parser.hpp"

#include "../lib/syntactic_analysis/text/text_source.hpp"

using namespace std;
using namespace my;

int main() {
    /*
    string file_name;
    cin >> file_name;
    */
    string file_name = "input.meleon";

    vector<Rule> rules = {
        Rule("Start",
            {
                Symbol("End"),
            },
        0),
        Rule("Start",
            {
                Symbol("Grammar"),
                Symbol("End"),
            },
        0),

        Rule("Grammar",
            {
                Symbol(TokenKind::Identifier)
            },
        0),
        Rule("Grammar",
            {
                Symbol(TokenKind::LeftRoundBracket),
                Symbol("Grammar"),
                Symbol(TokenKind::RightRoundBracket)
            },
        0),
        
        Rule("End",
            {
                Symbol(TokenKind::NewLine),
                Symbol(TokenKind::EndOfFile)
            },
        0),
        Rule("End",
            {
                Symbol(TokenKind::EndOfFile)
            },
        1)
    };

    RuleList grammar(rules);

    vector<string> text = read(file_name);
    TextSource source(text, file_name);
    Lexer lexer(source);
    Parser parser(lexer, grammar);
    
    bool success = parser.parse();
    
    return 0;

}