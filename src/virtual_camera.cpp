#include "virtual_camera.h"
#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>

virtual_camera::virtual_camera(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& up) noexcept
{
	virtual_camera::pos = pos;
	virtual_camera::dir = glm::normalize(dir);
	virtual_camera::right = glm::normalize(glm::cross(glm::normalize(up), virtual_camera::dir));
	virtual_camera::up = glm::normalize(glm::cross(virtual_camera::dir, virtual_camera::right));
}

void virtual_camera::rotate_up(const float angle) noexcept
{
	dir = glm::normalize(glm::rotate(dir, angle, up));
	right = glm::normalize(glm::cross(up, dir));
}

void virtual_camera::rotate_right(const float angle) noexcept
{
	dir = glm::normalize(glm::rotate(dir, angle, right));
	up = glm::normalize(glm::cross(dir, right));
}

void virtual_camera::rotate_dir(const float angle) noexcept
{
	up = glm::normalize(glm::rotate(up, angle, dir));
	right = glm::normalize(glm::cross(up, dir));
}

void virtual_camera::move(const glm::vec3& move_dir) noexcept
{
	pos += move_dir.x * right + move_dir.y * up + move_dir.z * dir;
}

const glm::vec3& virtual_camera::get_pos() const noexcept
{
	return pos;
}

const glm::vec3& virtual_camera::get_right() const noexcept
{
	return right;
}

const glm::vec3& virtual_camera::get_up() const noexcept
{
	return up;
}

const glm::vec3& virtual_camera::get_dir() const noexcept
{
	return dir;
}

void virtual_camera::set_pos(const glm::vec3& pos) noexcept
{
	virtual_camera::pos = pos;
}

void virtual_camera::set_orientation(const glm::vec3& dir, const glm::vec3& up) noexcept
{
	virtual_camera::dir = glm::normalize(dir);
	virtual_camera::right = glm::normalize(glm::cross(glm::normalize(up), virtual_camera::dir));
	virtual_camera::up = glm::normalize(glm::cross(virtual_camera::dir, virtual_camera::right));
}
