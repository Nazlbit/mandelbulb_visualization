#include "auxiliary.h"
#include <fstream>
#include <sstream>
#include <ctime>

std::vector<std::byte> auxiliary::read_binary_file(const std::string file_path)
{
	std::ifstream file(file_path, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file: \"" + file_path + "\"");
	}

	size_t file_size = file.tellg();
	file.seekg(0);

	std::vector<std::byte> buffer(file_size);

	file.read(reinterpret_cast<char*>(buffer.data()), file_size);

	file.close();

	return buffer;
}

std::string auxiliary::read_text_file(const std::string file_path)
{
	std::ifstream file(file_path);

	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file: \"" + file_path + "\"");
	}

	std::stringstream sstr;
	sstr << file.rdbuf();

	file.close();

	return sstr.str();
}
