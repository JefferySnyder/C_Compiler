#include <iostream>
#include <fstream>
#include <filesystem>
#include <format>
#include "generate_assembly.hpp"

namespace fs = std::filesystem;

void run_compiler(std::string input, bool test_parse = false) {
	std::fstream file(input);
	std::string file_contents((std::istreambuf_iterator<char>(file)),
							   std::istreambuf_iterator<char>());
	auto tokens = tokenize(file_contents);
	Parser parser{ tokens };
	auto ast = parser.parse();

	if (!test_parse) {
		generateAssembly(ast.get());
	}
}

int main(int argc, char* argv[]) {
	bool test_local = true;

	if (argc == 2 || test_local) {
		auto input = argc == 2 ? argv[1] : "inputs/input.c";
		try {
			run_compiler(input);
		}
		catch (const std::runtime_error& r) {
			std::cerr << "Error: " << r.what() << " - inputs.c\n";
		}
		catch (const std::out_of_range&) {
			std::cerr << "Error: vector out of range - inputs.c\n";
		}

		std::cin.get();
		return 0;
	}

	// test parsing with writing_a_c_compiler repo
    int stage_nums = 8;
    std::string is_valid = "valid";

	std::string full_current_path;

	std::vector<std::string> leftover_list;

	size_t fail_count = 0, file_count = 0;

    try {
        for (int i = 1; i <= stage_nums; i++) {

			fs::path dir_path = std::format("../write_a_c_compiler/stage_{}/{}/", i, is_valid);

			if (fs::exists(dir_path) && fs::is_directory(dir_path)) {
				for (const auto& entry : fs::directory_iterator(dir_path)) {
					// Check if it's a regular file (skips folders)
					if (fs::is_regular_file(entry.status())) {
						file_count++;
						auto c_file = entry.path().filename().string();
						full_current_path = dir_path.string() + c_file;
						try {
							leftover_list.push_back(full_current_path);
							run_compiler(full_current_path, !test_local);
						}
						catch (const std::runtime_error& r) {
							std::cerr << "Error: " << r.what() << " at " << full_current_path << "\n";
							leftover_list.pop_back();
							fail_count++;
						}
						catch (const std::out_of_range&) {
							std::cerr << "Error: vector out of range at " << full_current_path << "\n";
							leftover_list.pop_back();
							fail_count++;
						}
					}
				}
			}
        }

    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
	if (is_valid == "invalid") {
		std::cout << "Files: " << file_count << " , Fails: " << fail_count << std::endl;
		std::cout << "Files that weren't counted as invalid:" << std::endl;
		for (const auto& file : leftover_list) {
			std::cout << file << std::endl;
		}
	}
	else {
		std::cout << "if there is nothing, you're good";
	}

	std::cin.get();
	return 0;
}
