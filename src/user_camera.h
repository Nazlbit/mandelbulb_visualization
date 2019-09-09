#pragma once
#include "virtual_camera.h"

class user_camera : public virtual_camera
{
public:
	user_camera() = default;
	user_camera(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& up) noexcept;

	void update(float delta_time) noexcept;

	bool move_front = false;
	bool move_back = false;
	bool move_left = false;
	bool move_right = false;
	bool move_up = false;
	bool move_down = false;
	bool roll_left = false;
	bool roll_right = false;

	float velocity = 1;
};