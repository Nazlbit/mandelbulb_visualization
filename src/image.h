#pragma once
#include <vector>
#include <filesystem>

namespace image
{
	struct img_info
	{
		std::filesystem::path path;
		int width;
		int height;
		struct pixel_format
		{
			enum : uint8_t
			{
				GRAYSCALE,
				GRAYSCALE_A,
				RGB,
				RGB_A
			} color_type;
			enum : uint8_t
			{
				BIT_DEPTH_8 = 8,
				BIT_DEPTH_16 = 16,
				BIT_DEPTH_32 = 32
			} bit_depth;
		} format;

		std::vector<uint8_t> data;
	};

	void save_image(const img_info& info);
	void save_png(const img_info& info);

	std::filesystem::path add_sufix_if_file_exists(const std::filesystem::path& path);

}