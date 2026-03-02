#pragma once

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "lexer.hpp"

namespace my {

struct Node {
    enum class Kind {
        Empty,
        Program,
        Identifier,
        StringLiteral,
        Quote,
        Tuple,
        Call
    };

    Kind kind;
    std::string name;
    std::vector<Node> arguments;

    static Node empty() {
        return Node{Kind::Empty, "", {}};
    }

    static Node program(const std::vector<Node>& expressions) {
        return Node{Kind::Program, "", expressions};
    }

    static Node identifier(const std::string& identifier_name) {
        return Node{Kind::Identifier, identifier_name, {}};
    }

    static Node string_literal(const std::string& value) {
        return Node{Kind::StringLiteral, value, {}};
    }

    static Node quote(const Node& quoted_expression) {
        return Node{Kind::Quote, "", {quoted_expression}};
    }

    static Node call(const Node& callable_expression, const std::vector<Node>& call_arguments) {
        std::vector<Node> all_nodes;
        all_nodes.push_back(callable_expression);
        all_nodes.insert(all_nodes.end(), call_arguments.begin(), call_arguments.end());
        return Node{Kind::Call, "", all_nodes};
    }

    static Node tuple(const std::vector<Node>& tuple_items) {
        return Node{Kind::Tuple, "", tuple_items};
    }
};

class Parser {
private:
    Lexer lexer_;
    Token current_ = Token(TokenKind::EndOfFile, 0, "");
    bool had_spacing_before_current_ = false;
    bool had_newline_before_current_ = false;

    bool is_spacing_token_(TokenKind kind) const {
        return kind == TokenKind::WhiteSpace || kind == TokenKind::NewLine;
    }

    void advance_() {
        bool had_spacing = false;
        bool had_newline = false;

        while (true) {
            Token token = lexer_.lex();
            if (is_spacing_token_(token.kind())) {
                had_spacing = true;
                if (token.kind() == TokenKind::NewLine) {
                    had_newline = true;
                }
                continue;
            }

            current_ = token;
            had_spacing_before_current_ = had_spacing;
            had_newline_before_current_ = had_newline;
            return;
        }
    }

    std::string token_name_(TokenKind kind) const {
        switch (kind) {
            case TokenKind::Identifier: return "Identifier";
            case TokenKind::DoubleQuote: return "\"";
            case TokenKind::Quote: return "'";
            case TokenKind::LeftRoundBracket: return "(";
            case TokenKind::RightRoundBracket: return ")";
            case TokenKind::Comma: return ",";
            case TokenKind::EndOfFile: return "EndOfFile";
            default: return "Token";
        }
    }

    [[noreturn]] void throw_unexpected_(TokenKind expected) const {
        std::ostringstream message;
        message << "Expected " << token_name_(expected) << ", found '" << current_.text() << "'";
        throw std::runtime_error(message.str());
    }

    void expect_(TokenKind kind) {
        if (current_.kind() != kind) {
            throw_unexpected_(kind);
        }
    }

    std::vector<Node> parse_comma_separated_expressions_() {
        std::vector<Node> items;

        if (current_.kind() == TokenKind::RightRoundBracket) {
            return items;
        }

        while (true) {
            items.push_back(parse_expression_());

            if (current_.kind() != TokenKind::Comma) {
                break;
            }

            advance_();

            if (!had_spacing_before_current_) {
                throw std::runtime_error("Whitespace/newline is required after ','");
            }
        }

        return items;
    }

    Node parse_tuple_or_group_() {
        expect_(TokenKind::LeftRoundBracket);
        advance_();

        std::vector<Node> items = parse_comma_separated_expressions_();
        expect_(TokenKind::RightRoundBracket);
        advance_();

        if (items.size() == 1) {
            return items.at(0);
        }

        return Node::tuple(items);
    }

    Node parse_string_literal_() {
        expect_(TokenKind::DoubleQuote);

        std::string value;
        while (true) {
            Token token = lexer_.lex();

            if (token.kind() == TokenKind::EndOfFile) {
                throw std::runtime_error("Unterminated string literal");
            }

            if (token.kind() == TokenKind::DoubleQuote) {
                advance_();
                return Node::string_literal(value);
            }

            value.append(token.text());
        }
    }

    Node parse_quote_expression_() {
        expect_(TokenKind::Quote);
        advance_();

        Node quoted_expression = parse_expression_();

        expect_(TokenKind::Quote);
        advance_();

        return Node::quote(quoted_expression);
    }

    Node parse_primary_() {
        if (current_.kind() == TokenKind::Quote) {
            return parse_quote_expression_();
        }

        if (current_.kind() == TokenKind::LeftRoundBracket) {
            return parse_tuple_or_group_();
        }

        if (current_.kind() == TokenKind::DoubleQuote) {
            return parse_string_literal_();
        }

        expect_(TokenKind::Identifier);
        std::string identifier_name = current_.text();
        advance_();

        return Node::identifier(identifier_name);
    }

    Node parse_expression_() {
        Node expression = parse_primary_();

        while (current_.kind() == TokenKind::LeftRoundBracket) {
            if (had_spacing_before_current_) {
                throw std::runtime_error("Whitespace/newline is not allowed between callable expression and '('");
            }

            advance_();
            std::vector<Node> arguments = parse_comma_separated_expressions_();
            expect_(TokenKind::RightRoundBracket);
            advance_();

            expression = Node::call(expression, arguments);
        }

        return expression;
    }

public:
    explicit Parser(Lexer lexer) : lexer_(lexer) {
    }

    Node parse() {
        advance_();

        if (current_.kind() == TokenKind::EndOfFile) {
            return Node::empty();
        }

        std::vector<Node> expressions;
        expressions.push_back(parse_expression_());

        while (current_.kind() != TokenKind::EndOfFile) {
            if (!had_newline_before_current_) {
                throw std::runtime_error("Unexpected trailing tokens: expressions must be on distinct lines");
            }

            expressions.push_back(parse_expression_());
        }

        if (expressions.size() == 1) {
            return expressions.at(0);
        }

        return Node::program(expressions);
    }
};

inline std::string parser_to_string(const Node& node) {
    if (node.kind == Node::Kind::Empty) {
        return "";
    }

    if (node.kind == Node::Kind::Program) {
        std::ostringstream program;
        for (int i = 0; i < static_cast<int>(node.arguments.size()); ++i) {
            if (i > 0) {
                program << "\n";
            }
            program << parser_to_string(node.arguments.at(i));
        }
        return program.str();
    }

    if (node.kind == Node::Kind::Identifier) {
        return node.name;
    }

    if (node.kind == Node::Kind::StringLiteral) {
        return "\"" + node.name + "\"";
    }

    if (node.kind == Node::Kind::Quote) {
        return "'" + parser_to_string(node.arguments.at(0)) + "'";
    }

    if (node.kind == Node::Kind::Tuple) {
        std::ostringstream tuple;
        tuple << "(";
        for (int i = 0; i < static_cast<int>(node.arguments.size()); ++i) {
            if (i > 0) {
                tuple << ", ";
            }
            tuple << parser_to_string(node.arguments.at(i));
        }
        tuple << ")";
        return tuple.str();
    }

    if (node.arguments.empty()) {
        return "";
    }

    std::ostringstream result;
    result << parser_to_string(node.arguments.at(0)) << "(";

    for (int i = 1; i < static_cast<int>(node.arguments.size()); ++i) {
        if (i > 1) {
            result << ", ";
        }
        result << parser_to_string(node.arguments.at(i));
    }

    result << ")";
    return result.str();
}

inline std::string parser_ast_to_string(const Node& node, int depth = 0) {
    const std::string indent(static_cast<size_t>(depth) * 2, ' ');

    if (node.kind == Node::Kind::Empty) {
        return indent + "Empty";
    }

    if (node.kind == Node::Kind::Program) {
        std::ostringstream program;
        program << indent << "Program";
        for (const Node& expression : node.arguments) {
            program << "\n" << parser_ast_to_string(expression, depth + 1);
        }
        return program.str();
    }

    if (node.kind == Node::Kind::Identifier) {
        return indent + "Identifier(" + node.name + ")";
    }

    if (node.kind == Node::Kind::StringLiteral) {
        return indent + "String(" + node.name + ")";
    }

    if (node.kind == Node::Kind::Quote) {
        const Node& quoted = node.arguments.at(0);
        std::ostringstream quote;
        quote << indent << "Quote";
        quote << "\n" << parser_ast_to_string(quoted, depth + 1);
        return quote.str();
    }

    if (node.kind == Node::Kind::Tuple) {
        std::ostringstream tuple;
        tuple << indent << "Tuple";
        for (const Node& item : node.arguments) {
            tuple << "\n" << parser_ast_to_string(item, depth + 1);
        }
        return tuple.str();
    }

    std::ostringstream result;
    if (node.arguments.empty()) {
        result << indent << "Call";
        return result.str();
    }

    result << indent << "Call";
    result << "\n" << parser_ast_to_string(node.arguments.at(0), depth + 1);
    result << "\n";

    for (int i = 1; i < static_cast<int>(node.arguments.size()); ++i) {
        const Node& argument = node.arguments.at(i);
        result << "\n" << parser_ast_to_string(argument, depth + 1);
    }

    return result.str();
}

}
