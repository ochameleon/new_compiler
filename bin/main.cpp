#include <iostream>
#include <string>
#include <vector>

#include "../lib/syntactic_analysis/steps/reader.hpp"
#include "../lib/syntactic_analysis/steps/lexer.hpp"
#include "../lib/syntactic_analysis/steps/parser.hpp"
#include "../lib/syntactic_analysis/steps/interpreter.hpp"
#include "../lib/syntactic_analysis/text/text_source.hpp"

using namespace my;

/*

#include "../lib/syntactic_analysis/rules/rule.hpp"

#include "../lib/syntactic_analysis/steps/earley_parser.hpp"

int main() {
    std::string file_name = "input.meleon";
    std::vector<Rule> rules = { ... };

    std::vector<Rule> grammar(rules);
    std::vector<std::string> text = read(file_name);
    TextSource source(text, file_name);
    Lexer lexer(source);
    EarleyParser parser(lexer, grammar);

    std::cout << parser.recognize() << std::endl;
    return 0;
}
*/

int main() {
    try {
        const std::string file_name = "input.meleon";
        std::vector<std::string> text = read(file_name);

        TextSource source(text, file_name);
        Lexer lexer(source);
        Parser parser(lexer);

        Node root = parser.parse();
        std::cout << "AST:" << std::endl;
        std::cout << parser_ast_to_string(root) << std::endl;
        std::cout << "Expressions: " << parser_to_string(root) << std::endl;

        Interpreter interpreter(root);
        Node evaluated = interpreter.evaluate();
        std::cout << "Evaluated AST:" << std::endl;
        std::cout << parser_ast_to_string(evaluated) << std::endl;
        std::cout << "Evaluated Expressions: " << parser_to_string(evaluated) << std::endl;
        return 0;
    }
    catch (const std::exception& ex) {
        std::cerr << "Parse or Intepretation error: " << ex.what() << std::endl;
        return 1;
    }
}