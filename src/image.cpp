#include "image.h"
#include <iostream>
#include <fstream>
#include <string>

#include <png.h>

void image::save_image(const img_info& info)
{
	if (!info.path.has_extension()) return;
	
	auto ext = info.path.extension();
	if (ext == ".png") save_png(info);

}

template<typename T>
T swap_bytes(const T& val)
{
	T result;
	char* result_ref = reinterpret_cast<char*>(&result);
	char* val_ref = reinterpret_cast<char*>(&val);
	for (int i = 0; i < sizeof(T); i++)
	{
		result_ref[i] = val_ref[sizeof(T) - i - 1];
	}
	return result;
}


void image::save_png(const img_info& info)
{
	auto path = add_sufix_if_file_exists(info.path);
	if (!path.has_extension()) path.replace_extension(".png");

	std::cout << path << std::endl;

	std::filesystem::create_directories(path.parent_path());

	png_image png_image_struct = {};
	png_image_struct.version = PNG_IMAGE_VERSION;
	png_image_struct.width = info.width;
	png_image_struct.height = info.height;
	int num_channels = 0;

	if (info.format.bit_depth == img_info::pixel_format::BIT_DEPTH_16)
		png_image_struct.flags = PNG_FORMAT_FLAG_LINEAR;
	else if (info.format.bit_depth == img_info::pixel_format::BIT_DEPTH_32)
		return;

	if (info.format.color_type == img_info::pixel_format::GRAYSCALE)
	{
		num_channels = 1;
		png_image_struct.format = PNG_FORMAT_GRAY;
	}
	else if (info.format.color_type == img_info::pixel_format::GRAYSCALE_A)
	{
		num_channels = 2;
		png_image_struct.format = PNG_FORMAT_GA;
		png_image_struct.flags |= PNG_FORMAT_FLAG_ALPHA;
	}
	else if (info.format.color_type == img_info::pixel_format::RGB)
	{
		num_channels = 3;
		png_image_struct.format = PNG_FORMAT_RGB;
		png_image_struct.flags |= PNG_FORMAT_FLAG_COLOR;
	}
	else if (info.format.color_type == img_info::pixel_format::RGB_A)
	{
		num_channels = 4;
		png_image_struct.format = PNG_FORMAT_RGBA;
		png_image_struct.flags |= PNG_FORMAT_FLAG_ALPHA | PNG_FORMAT_FLAG_COLOR;
	}

	png_image_write_to_file(&png_image_struct, path.string().c_str(), false, info.data.data(), -info.width * num_channels, 0);
}

std::filesystem::path image::add_sufix_if_file_exists(const std::filesystem::path& path)
{
	std::filesystem::path result = path;
	std::string ext = path.extension().string();
	std::string name = path.filename().stem().string();
	std::string parent_path = path.parent_path().string();

	int num = 1;
	while (std::filesystem::exists(result))
	{
		result = parent_path + "/" + name + "_" + std::to_string(num) + ext;
		num++;
	}

	return result;
}
