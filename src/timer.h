#pragma once
#include <chrono>

class timer
{
private:
	using clock = std::chrono::steady_clock;
	using time_point = std::chrono::time_point<clock>;
	using time_duration = std::chrono::duration<double>;

	time_point start_time;
	time_duration current_time;
	time_duration previous_time;
	time_duration delta_time;
public:
	timer();
	void tick();
	double get_time() const;
	void reset_time();
	double get_delta_time() const;
};

