#include "Timer.h"

timer::timer() :
	start_time(clock::now()), delta_time(), current_time(), previous_time()
{
}

void timer::tick()
{
	current_time = clock::now() - start_time;
	delta_time = current_time - previous_time;
	previous_time = current_time;
}

double timer::get_time() const
{
	return current_time.count();
}

void timer::reset_time()
{
	current_time = time_duration::zero();
	previous_time = time_duration::zero();
	start_time = clock::now();
}

double timer::get_delta_time() const
{
	return delta_time.count();
}
