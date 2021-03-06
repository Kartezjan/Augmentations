#pragma once

#include <vector>
#include <string>

namespace augs {
	class file_in {

	};

	extern void create_directory(std::string filename);
	extern void create_directory(std::wstring filename);
	extern bool file_exists(std::wstring filename);
	extern std::vector<std::wstring> get_all_files_in_directory(std::wstring dir);
	extern std::string get_file_contents(std::string filename);
	extern std::string get_file_contents(std::wstring filename);
}