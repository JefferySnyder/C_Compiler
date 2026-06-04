#include <iostream>
#include <fstream>
#include <string>
#include <vector>

enum class Token {
	open_brace,
	close_brace,
	open_paren,
	close_paren,
	semicolon,
	keyword,
	identifier, // all non-keyword words
	integer_literal, // all integers
};

int main(int argc, char* argv[]) {
	std::vector<Token> tokens;

	std::fstream file("inputs/return_2.c");

	char ch;
	while (file >> ch) {
		switch (ch) {
		case '{':
			tokens.push_back(Token::open_brace);
			break;
		default:
			if (std::isalpha(ch)) {
			}
			else if (std::isdigit(ch)) {
			}
		}
	}
	std::cin.get();
	return 0;
}