#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

enum class TokenType {
	Invalid,
	OpenBrace,
	CloseBrace,
	OpenParen,
	CloseParen,
	Semicolon,
	Int,
	Return,
	Identifier, // all non-keyword words
	IntegerLiteral, // all integers
	Minus,
	Tilde,
	Bang,
};
std::unordered_map<std::string, TokenType> keywords{
	{"return", TokenType::Return},
	{"int", TokenType::Int},
};

struct Token {
	TokenType type;
	std::string value;
};

std::vector<Token> tokenize(const std::string& source) {
	std::vector<Token> tokens;

	size_t i = 0;
	while (i < source.size()) {
		char ch = source[i];
		if (std::isspace(ch)) { i++; continue; }

		if (source[i] == '{') { tokens.push_back({ TokenType::OpenBrace, std::string{ch} }); ++i; continue; }
		if (source[i] == '}') { tokens.push_back({ TokenType::CloseBrace, std::string{ch} }); ++i; continue; }
		if (source[i] == '(') { tokens.push_back({ TokenType::OpenParen, std::string{ch} }); ++i; continue; }
		if (source[i] == ')') { tokens.push_back({ TokenType::CloseParen, std::string{ch} }); ++i; continue; }
		if (source[i] == ';') { tokens.push_back({ TokenType::Semicolon, std::string{ch} }); ++i; continue; }
		if (source[i] == '-') { tokens.push_back({ TokenType::Minus, std::string{ch} }); ++i; continue; }
		if (source[i] == '~') { tokens.push_back({ TokenType::Tilde, std::string{ch} }); ++i; continue; }
		if (source[i] == '!') { tokens.push_back({ TokenType::Bang, std::string{ch} }); ++i; continue; }
			
		if (std::isdigit(ch)) {
			std::string val;
			while (i < source.size() && std::isdigit(source[i])) { val += source[i++]; }
			tokens.push_back({ TokenType::IntegerLiteral, val });
			continue;
		}
		if (std::isalpha(ch)) {
			std::string val;
			while (i < source.size() && std::isalnum(source[i])) { val += source[i++]; }
			if (keywords.contains(val)) {
				tokens.push_back({ keywords[val], val });
			}
			else {
				tokens.push_back({ TokenType::Identifier, val });
			}
			continue;
		}
		tokens.push_back({ TokenType::Invalid, std::string{ch} });
		++i;
	}
	return tokens;
};
