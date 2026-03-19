#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <algorithm>

#include "token_kind.hpp"

// TODO: populate list

namespace my {

std::unordered_map<std::string, TokenKind> SpecialCharactersMap = {
    {"□", TokenKind::LeftProofBracket},
    {"■", TokenKind::RightProofBracket}
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

bool is_digit(const std::string &c) {
  return c.size() == 1 && c[0] >= '0' && c[0] <='9';
  
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

};
