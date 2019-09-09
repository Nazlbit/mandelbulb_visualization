#include "user_camera.h"
user_camera::user_camera(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& up) noexcept : virtual_camera(pos, dir, up)
{

}

void user_camera::update(float delta_time) noexcept
{
	glm::vec3 move_dir(0);

	move_dir.z = ((float)move_front - move_back);
	move_dir.x = ((float)move_right - move_left);
	move_dir.y = ((float)move_up - move_down);

	move_dir *= delta_time * velocity;

	move(move_dir);

	rotate_dir(((float)roll_left - roll_right)* delta_time);
}
