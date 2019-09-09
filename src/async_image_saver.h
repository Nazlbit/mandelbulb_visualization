#pragma once
#include "image.h"
#include "i_safe_destructible.h"

#include <mutex>
#include <thread>
#include <queue>
#include <atomic>

namespace image
{
	class async_image_saver : public i_safe_destructible
	{
	public:
		async_image_saver(int max_queue_length = 10, int num_threads = 1);
		~async_image_saver();
		void save_image(const img_info& info);
		void save_image(img_info&& info);

	private:
		std::vector<std::thread> threads;
		std::mutex queue_mutex;
		std::queue<img_info> images_to_save;
		int max_queue_length;
		std::condition_variable queue_size_condition;
		std::atomic<bool> exit = false;
		void saver();
	protected:
		void safe_destroy() noexcept override;
	};
}
