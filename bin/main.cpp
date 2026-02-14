#include <string>
#include <vector>

#include "../lib/syntactic_analysis/rules/rule.hpp"

#include "../lib/syntactic_analysis/steps/reader.hpp"
#include "../lib/syntactic_analysis/steps/lexer.hpp"
#include "../lib/syntactic_analysis/steps/parser.hpp"

#include "../lib/syntactic_analysis/text/text_source.hpp"

using namespace my;

int main() {
    /*
    std::string file_name;
    std::cin >> file_name;
    */
    string file_name = "input.meleon";

    vector<Rule> rules = {
        Rule("start",
            {
                Symbol("end")
            },
        0, false),
        Rule("start",
            {
                Symbol("commands"),
                Symbol("end")
            },
        0, false),
        Rule("end",
            {
                Symbol(TokenKind::EndOfFile)
            },
        1, false),




        Rule("commands",
            {
                Symbol("command"),
                Symbol(TokenKind::NewLine),
                Symbol("commands")
            },
        0, false),
        Rule("commands",
            {
                Symbol("command")
            },
        0, false),




        Rule("command",
            {
                Symbol("expression")
            },
        0, false),


        

        Rule("expression",
            {
                Symbol("isolated_expression")
            },
        0, false),
        Rule("expression",
            {
                Symbol("isolated_expression"),
                Symbol(TokenKind::LeftRoundBracket),
                Symbol("expressions_separated_by_comma"),
                Symbol(TokenKind::RightRoundBracket),
            },
        0, false),
        Rule("expression",
            {
                Symbol(TokenKind::Identifier),
                Symbol("whitespaces_or_newlines"),
                Symbol(TokenKind::BarArrow),
                Symbol("whitespaces_or_newlines"),
                Symbol("expression")
            },
        0, false),




        Rule("isolated_expression",
            {
                Symbol(TokenKind::Identifier)
            },
        0, false),
        Rule("isolated_expression",
            {
                Symbol(TokenKind::LeftRoundBracket),
                Symbol("expression"),
                Symbol(TokenKind::RightRoundBracket)
            },
        0, false),
        

        Rule("expressions_separated_by_comma",
            {
                Symbol("expression")
            },
        0, false),
        Rule("expressions_separated_by_comma",
            {
                Symbol("expression"),
                Symbol(TokenKind::Comma),
                Symbol("whitespaces_or_newlines"),
                Symbol("expressions_separated_by_comma")
            },
        0, false),



        Rule("whitespace_or_newline",
            {
                Symbol(TokenKind::WhiteSpace)
            },
        0, false),
        Rule("whitespace_or_newline",
            {
                Symbol(TokenKind::NewLine)
            },
        0, false),



        Rule("whitespaces_or_newlines",
            {
                Symbol("whitespace_or_newline")
            },
        0, false),
        Rule("whitespaces_or_newlines",
            {
                Symbol("whitespace_or_newline"),
                Symbol("whitespaces_or_newlines")
            },
        0, false),
    };

    std::vector<Rule> grammar(rules);

    std::vector<std::string> text = read(file_name);
    TextSource source(text, file_name);
    Lexer lexer(source);
    Parser parser(lexer, grammar);

    parser.parse();

    return 0;
}