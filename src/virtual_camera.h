#pragma once
#include <glm/vec3.hpp>

class virtual_camera
{
	glm::vec3 pos{ 0, 0, 0 };
	glm::vec3 right{ 1, 0, 0 };
	glm::vec3 up{ 0, 1, 0 };
	glm::vec3 dir{ 0, 0, 1};
public:
	virtual_camera() = default;
	virtual_camera(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& up) noexcept;

	void rotate_up(const float angle) noexcept;
	void rotate_right(const float angle) noexcept;
	void rotate_dir(const float angle) noexcept;

	void move(const glm::vec3& dir) noexcept;

	const glm::vec3& get_pos() const noexcept;
	const glm::vec3& get_right() const noexcept;
	const glm::vec3& get_up() const noexcept;
	const glm::vec3& get_dir() const noexcept;

	void set_pos(const glm::vec3& pos) noexcept;
	void set_orientation(const glm::vec3& dir, const glm::vec3& up) noexcept;
};