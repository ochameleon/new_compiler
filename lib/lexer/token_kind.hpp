#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <algorithm>

// TODO: populate list
namespace unicode::format {
  constexpr auto TabKey = u8"\u000B";
  constexpr auto FormFeed = u8"\u000C";


};

namespace my {

enum class TokenKind {
    WhiteSpace,
    NewLine,
    EndOfFile,

    LeftRoundBracket,
    RightRoundBracket,

    LeftParen,
    RightParen,
    
    LeftBrace, // {
    RightBrace,  

    Comma,
    SemiColon,  // ; 
    Colon,      // :
    RightArrow, // ↦
 
    Quote,
    DoubleQuote,
    String,


    LeftProofBracket,
    RightProofBracket,

    Number,
    RealNumber,
  
    Plus,
    Minus,
    Multiply,
    Divide,
    

    Identifier,
    BadLexeme,
    CompilationUnit,
};

constexpr auto LeftProofBracketStr = "□";
constexpr auto RightProofBracketStr = "■";

extern std::unordered_map<std::string, TokenKind> SpecialCharactersMap;

const std::vector<std::pair<std::string, TokenKind>> special_character_to_kind = {



    {"□", TokenKind::LeftProofBracket},
    {"■", TokenKind::RightProofBracket}
};


const std::vector<std::string> invisible_characters = {
    "\u000B", "\u000C", "\u00A0", "\u1680", "\u180E",
    "\u2000", "\u2001", "\u2002", "\u2003", "\u2004",
    "\u2005", "\u2006", "\u2007", "\u2008", "\u2009",
    "\u200A", "\u200B", "\u200C", "\u200D", "\u200E",
    "\u200F", "\u2028", "\u2029", "\u202F", "\u205F",
    "\u2060", "\u2061", "\u2062", "\u2063", "\u2064",
    "\uFEFF"
};



bool is_special(const std::string& c);


bool is_invisible(const std::string& c);


bool is_spacing(const std::string& c); 
bool is_digit(const std::string &c);


bool is_newline(const std::string& c);


bool is_allowed(const std::string& c);


template <typename K, typename V>
std::vector<std::pair<V, K>> invert(const std::vector<std::pair<K, V>>& input);



const std::vector<std::pair<TokenKind, std::string>> kind_to_special_characters =
    invert(special_character_to_kind);

bool is_special_kind(TokenKind kind);


std::optional<std::string> kind_to_string(TokenKind kind);



};
