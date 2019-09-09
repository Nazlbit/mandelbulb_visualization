#pragma once
#include <string>
#include <vector>

namespace auxiliary
{
	std::vector<std::byte> read_binary_file(const std::string file_path);
	std::string read_text_file(const std::string file_path);
}