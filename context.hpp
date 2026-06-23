#include <unordered_map>
#include <string>

struct Context {
	std::unordered_map<std::string, int> var_map;
	int stack_index = 0;
	std::string break_label;
	std::string continue_label;
};