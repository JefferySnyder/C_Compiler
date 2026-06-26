#include <unordered_map>
#include <string>

struct Info {
	TokenType type;
	int stack_index;
	Info(TokenType t, int si) : type{ t }, stack_index{ si } {};
};

struct Context {
	std::unordered_map<std::string, Info> var_map;
	int stack_index = 0;
	std::string break_label;
	std::string continue_label;
};