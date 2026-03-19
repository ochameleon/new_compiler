#include "lexer.hpp"

#include <cctype>

using namespace std;

namespace my {


Token Lexer::lex() {
    skipWhitespace();

    int start = position;

    if (textEnded()) {
        return makeToken(TokenKind::EndOfFile, start);
    }

    string c = moveForward();

    if (c.size() == 1) {
        char cc = c[0];

        switch (cc) {
        case '\n': return makeToken(TokenKind::NewLine, start);
        case '(': return makeToken(TokenKind::LeftParen, start);
        case ')': return makeToken(TokenKind::RightParen, start);
        case '{': return makeToken(TokenKind::LeftBrace, start);
        case '}': return makeToken(TokenKind::RightBrace, start);
        case '+': return makeToken(TokenKind::Plus, start);
        case '-': return makeToken(TokenKind::Minus, start);
        case '*': return makeToken(TokenKind::Multiply, start);
        case '/': return makeToken(TokenKind::Divide, start);
        case ':': return makeToken(TokenKind::Colon, start);
        case '\'': return makeToken(TokenKind::Quote, start);
        case '"': return makeToken(TokenKind::DoubleQuote, start);
        case ',': return makeToken(TokenKind::Comma, start);
        case ';': return makeToken(TokenKind::SemiColon, start);
        }
    }

    if (isAlpha(c)) {
        return makeIdentifier(start);
    }

    if (is_digit(c)) {
        return makeNumber(start);
    }

    if (is_special(c)) {
        auto Kind = SpecialCharactersMap[c];
        return makeToken(Kind, start);
    }

    throw runtime_error("Unexpected character in lexer");
}

Token Lexer::makeToken(TokenKind Kind, int start) {
    auto text = TextSrc.to_string(TextSpan(start, position - start));
    return Token{Kind, start, text};
}

string Lexer::moveForward() {
    return TextSrc.text()[position++];
}

bool Lexer::textEnded() const {
    return position >= TextSrc.text().size();
}

string Lexer::peek(int offset) {
    if (position + offset >= TextSrc.text().size())
        return "";
    return TextSrc.text()[position + offset];
}

bool Lexer::compareAndMove(const string &expected) {
    if (peek() != expected)
        return false;

    position++;
    return true;
}

void Lexer::skipWhitespace() {
    while (!textEnded()) {
        string c = peek();

        if (c == " " || c == "\t" || c == "\n" || c == "\r")
            position++;
        else
            break;
    }
}

Token Lexer::makeIdentifier(int start) {
    while (isAlpha(peek()) || is_digit(peek())) {
        moveForward();
    }

    return makeToken(TokenKind::Identifier, start);
}

Token Lexer::makeNumber(int start) {
    while (is_digit(peek())) {
        moveForward();
    }

    if (peek() == "." && is_digit(peek(1))) {
        moveForward();

        while (is_digit(peek())) {
            moveForward();
        }

        return makeToken(TokenKind::RealNumber, start);
    }

    return makeToken(TokenKind::Number, start);
}

Token Lexer::makeString(int start) {
    while (!textEnded() && peek() != "\"") {
        moveForward();
    }

    if (!textEnded())
        moveForward();

    return makeToken(TokenKind::String, start);
}

bool Lexer::isAlpha(string c) const {
    if (c == LeftProofBracketStr || c == RightProofBracketStr)
        return false;

    if (c.size() != 1)
        return false;

    return isalpha(static_cast<unsigned char>(c[0])) || c == "_";
}

}
