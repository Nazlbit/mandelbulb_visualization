#include "async_image_saver.h"

using namespace image;

async_image_saver::async_image_saver(int max_queue_length, int num_threads)
	: max_queue_length(max_queue_length)
{
	try
	{
		for (int i = 0; i < num_threads; i++)
		{
			threads.push_back(std::thread([this] { saver(); }));
		}
	}
	catch (const std::exception&)
	{
		safe_destroy();
		throw;
	}
}

async_image_saver::~async_image_saver()
{
	safe_destroy();
}

void async_image_saver::save_image(const img_info& info)
{
	{
		std::unique_lock lock(queue_mutex);
		queue_size_condition.wait(lock, [this] {return max_queue_length > images_to_save.size() || exit; });
		images_to_save.push(info);
	}
	queue_size_condition.notify_one();
}

void async_image_saver::save_image(img_info&& info)
{
	{
		std::unique_lock lock(queue_mutex);
		queue_size_condition.wait(lock, [this] { return max_queue_length > images_to_save.size() || exit; });
		images_to_save.push(std::move(info));
	}
	queue_size_condition.notify_one();
}

void async_image_saver::saver()
{
	while (!exit)
	{
		std::unique_lock lock(queue_mutex);
		queue_size_condition.wait(lock, [this] { return !images_to_save.empty() || exit; });

		while(!images_to_save.empty() && !exit)
		{
			//get img_info
			img_info info = std::move(images_to_save.front());
			images_to_save.pop();
			lock.unlock();
			queue_size_condition.notify_one();

			//save img
			image::save_image(info);

			lock.lock();
		}
	}
}

void image::async_image_saver::safe_destroy() noexcept
{
	exit = true;
	queue_size_condition.notify_all();
	for (auto& th : threads) th.join();
}
