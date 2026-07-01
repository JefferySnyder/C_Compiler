#include <iostream>
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
	Plus,
	Asterisk,
	ForwardSlash,
	AND,
	OR,
	Equal,
	NotEqual,
	LessThan,
	LessThanOrEqual,
	GreaterThan,
	GreaterThanOrEqual,
	Assignment,
	PlusEquals,
	MinusEquals,
	If,
	Else,
	Colon,
	QuestionMark,
	For,
	While,
	Do,
	Break,
	Continue,
	Comma,
	Bool,
	True,
	False,
	Char,
	CharLiteral,
	StringLiteral,
};
std::unordered_map<std::string, TokenType> keywords{
	{"return", TokenType::Return},
	{"int", TokenType::Int},
	{"bool", TokenType::Bool},
	{"char", TokenType::Char},
	{"if", TokenType::If},
	{"else", TokenType::Else},
	{"for", TokenType::For},
	{"while", TokenType::While},
	{"do", TokenType::Do},
	{"break", TokenType::Break},
	{"continue", TokenType::Continue},
	{"true", TokenType::True},
	{"false", TokenType::False},
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
		if (source[i] == '~') { tokens.push_back({ TokenType::Tilde, std::string{ch} }); ++i; continue; }
		if (source[i] == ':') { tokens.push_back({ TokenType::Colon, std::string{ch} }); ++i; continue; }
		if (source[i] == '?') { tokens.push_back({ TokenType::QuestionMark, std::string{ch} }); ++i; continue; }
		if (source[i] == ',') { tokens.push_back({ TokenType::Comma, std::string{ch} }); ++i; continue; }
		if (source[i] == '+') {
			if (source.at(i + 1) == '=') {
				tokens.push_back({ TokenType::PlusEquals, "+="});
				i += 2; continue; 
			}
			tokens.push_back({ TokenType::Plus, std::string{ch} });
			++i; continue; 
		}
		if (source[i] == '-') {
			if (source.at(i + 1) == '=') {
				tokens.push_back({ TokenType::MinusEquals, "-="});
				i += 2; continue; 
			}
			tokens.push_back({ TokenType::Minus, std::string{ch} });
			++i; continue; 
		}
		if (source[i] == '*') { tokens.push_back({ TokenType::Asterisk, std::string{ch} }); ++i; continue; }
		if (source[i] == '/') { tokens.push_back({ TokenType::ForwardSlash, std::string{ch} }); ++i; continue; }
		if (source[i] == '!') {
			if (source.at(i + 1) == '=') {
				tokens.push_back({ TokenType::NotEqual, "!="});
				i += 2; continue; 
			}
			tokens.push_back({ TokenType::Bang, std::string{ch} });
			++i; continue; 
		}
		if (source[i] == '<') {
			if (source.at(i + 1) == '=') {
				tokens.push_back({ TokenType::LessThanOrEqual, "<="});
				i += 2; continue; 
			}
			tokens.push_back({ TokenType::LessThan, std::string{ch} });
			++i; continue; 
		}
		if (source[i] == '>') {
			if (source.at(i + 1) == '=') {
				tokens.push_back({ TokenType::GreaterThanOrEqual, ">="});
				i += 2; continue; 
			}
			tokens.push_back({ TokenType::GreaterThan, std::string{ch} });
			++i; continue; 
		}
		if (source[i] == '=') {
			if (source.at(i + 1) == '=') {
				tokens.push_back({ TokenType::Equal, "=="});
				i += 2; continue; 
			}
			tokens.push_back({ TokenType::Assignment, std::string{ch} });
			++i; continue; 
		}
		if (source[i] == '&' && source.at(i+1) == '&') {
			tokens.push_back({ TokenType::AND, "&&"});
			i += 2; continue; 
		}
		if (source[i] == '|' && source.at(i+1) == '|') {
			tokens.push_back({ TokenType::OR, "||"});
			i += 2; continue; 
		}
			
		if (source[i] == '\'') {
			i++;
			char val = source.at(i++);
			if (source.at(i++) != '\'')
				tokens.push_back({ TokenType::Invalid, std::string{val} });
			else
				tokens.push_back({ TokenType::CharLiteral, std::to_string(val) });
			continue;
		}
		if (source[i] == '\"') {
			i++;
			std::string val;
			while (i < source.size() && source[i] != '\"') { val += source[i++]; }
			tokens.push_back({ TokenType::StringLiteral, val });
			i++;
			continue;
		}
		if (std::isdigit(ch)) {
			std::string val;
			while (i < source.size() && std::isdigit(source[i])) { val += source[i++]; }
			tokens.push_back({ TokenType::IntegerLiteral, val });
			continue;
		}
		if (std::isalpha(ch)) {
			std::string val;
			while (i < source.size() && (std::isalnum(source[i]) || source[i] == '_')) { val += source[i++]; }
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
