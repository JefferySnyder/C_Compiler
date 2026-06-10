#include <iostream>
#include <fstream>
#include <filesystem>
#include <format>
#include "generate_assembly.hpp"

namespace fs = std::filesystem;

void run_compiler(std::string input) {
	std::fstream file(input);
	std::string file_contents((std::istreambuf_iterator<char>(file)),
							   std::istreambuf_iterator<char>());
	auto tokens = tokenize(file_contents);
	Parser parser{ tokens };
	auto ast = parser.parse();

	generateAssembly(ast.get());
}

int main(int argc, char* argv[]) {
	bool test_local = true;
	if (test_local) {
		run_compiler("inputs/input.c");

		std::cin.get();
		return 0;
	}

    int stage_nums = 5;
    std::string is_valid = "valid";

	std::string full_current_path;

    try {
        for (int i = 1; i <= stage_nums; i++) {

			fs::path dir_path = std::format("../write_a_c_compiler/stage_{}/{}/", i, is_valid);

			if (fs::exists(dir_path) && fs::is_directory(dir_path)) {
				for (const auto& entry : fs::directory_iterator(dir_path)) {
					// Check if it's a regular file (skips folders)
					if (fs::is_regular_file(entry.status())) {
						auto c_file = entry.path().filename().string();
						full_current_path = dir_path.string() + c_file;
						run_compiler(full_current_path);
					}
				}
			}
        }

    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    catch (const std::runtime_error& r) {
        std::cerr << "Error: " << r.what() << " at " << full_current_path << "\n";
    }

    std::cout << "if there is nothing, you're good";

	std::cin.get();
	return 0;
}
