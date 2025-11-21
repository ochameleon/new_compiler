#include <string>
#include <vector>

#include "../lib/code_analysis/rules/rule_list.hpp"

#include "../lib/code_analysis/steps/reader.hpp"
#include "../lib/code_analysis/steps/lexer.hpp"

#include "../lib/code_analysis/text/text_source.hpp"

using namespace std;
using namespace my;

int main() {
    /*
    string file_name;
    cin >> file_name;
    */
    string file_name = "input.meleon";

    RuleList grammar(
        {}
    );

    vector<string> text = read(file_name);
    TextSource source(text, file_name);
    Lexer lexer(source);

    while (true) {
        SyntaxToken l = lexer.lex();
        TokenKind k = l.kind();
        if (kind_to_string(k).has_value()) {
            cout << kind_to_string(k).value();
            if (k == TokenKind::EndOfFile) break;
        }
        else {
            cout << l.text();
        }
        cout << endl;
    }
}
