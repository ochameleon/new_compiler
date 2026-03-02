#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <algorithm>

namespace my {

enum class TokenKind {
    WhiteSpace,
    NewLine,
    EndOfFile,

    LeftRoundBracket,
    RightRoundBracket,

    Comma,
    SemiColon,

    Quote,
    DoubleQuote,

    LeftProofBracket,
    RightProofBracket,

    Identifier,
    BadLexeme,
    CompilationUnit,
};

const std::vector<std::pair<std::string, TokenKind>> special_character_to_kind = {
    {"(", TokenKind::LeftRoundBracket},
    {")", TokenKind::RightRoundBracket},

    {",", TokenKind::Comma},
    {";", TokenKind::SemiColon},

    {"'", TokenKind::Quote},
    {"\"", TokenKind::DoubleQuote},

    {"□", TokenKind::LeftProofBracket},
    {"■", TokenKind::RightProofBracket}
};

const std::vector<std::string> invisible_characters = {
    u8"\u000B", u8"\u000C", u8"\u00A0", u8"\u1680", u8"\u180E",
    u8"\u2000", u8"\u2001", u8"\u2002", u8"\u2003", u8"\u2004",
    u8"\u2005", u8"\u2006", u8"\u2007", u8"\u2008", u8"\u2009",
    u8"\u200A", u8"\u200B", u8"\u200C", u8"\u200D", u8"\u200E",
    u8"\u200F", u8"\u2028", u8"\u2029", u8"\u202F", u8"\u205F",
    u8"\u2060", u8"\u2061", u8"\u2062", u8"\u2063", u8"\u2064",
    u8"\uFEFF"
};

bool is_special(const std::string& c) {
    return any_of(
        special_character_to_kind.begin(),
        special_character_to_kind.end(),
        [&](const auto& p) { return p.first == c; }
    );
}

bool is_invisible(const std::string& c) {
    return find(invisible_characters.begin(), invisible_characters.end(), c) != invisible_characters.end();
}

bool is_spacing(const std::string& c) {
    return c == " " || c == "\t";
}

bool is_newline(const std::string& c) {
    return c == "\n";
}

bool is_allowed(const std::string& c) {
    return !(is_spacing(c) || is_newline(c) || is_special(c) || is_invisible(c));
}

template <typename K, typename V>
std::vector<std::pair<V, K>> invert(const std::vector<std::pair<K, V>>& input) {
    std::vector<std::pair<V, K>> result;
    for (const auto& [k, v] : input)
        result.emplace_back(v, k);
    return result;
}

const std::vector<std::pair<TokenKind, std::string>> kind_to_special_characters =
    invert(special_character_to_kind);

bool is_special_kind(TokenKind kind) {
    return any_of(
        kind_to_special_characters.begin(),
        kind_to_special_characters.end(),
        [&](const auto& p) { return p.first == kind; }
    );
}

std::optional<std::string> kind_to_string(TokenKind kind) {
    switch (kind) {
        case TokenKind::WhiteSpace: return " ";
        case TokenKind::NewLine: return "\n";
        case TokenKind::EndOfFile: return "";
        default:
            if (is_special_kind(kind)) {
                auto it = std::find_if(
                    kind_to_special_characters.begin(),
                    kind_to_special_characters.end(),
                    [&](const auto& p) { return p.first == kind; }
                );
                if (it != kind_to_special_characters.end())
                    return it->second;
            }
            return std::nullopt;
    }
}

}