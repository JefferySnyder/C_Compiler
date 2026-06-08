#include <iostream>
#include <fstream>
#include "generate_assembly.hpp"

int main(int argc, char* argv[]) {
	std::fstream file("inputs/return_2.c");
	std::string file_contents((std::istreambuf_iterator<char>(file)),
							   std::istreambuf_iterator<char>());
	auto tokens = tokenize(file_contents);
	Parser parser{ tokens };
	auto ast = parser.parse();

	generateAssembly(ast.get());

	std::cin.get();
	return 0;
}
