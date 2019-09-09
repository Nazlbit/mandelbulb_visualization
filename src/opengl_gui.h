#pragma once
#include "i_renderer.h"
#include "user_camera.h"

#include <glad/glad.h>
#include <string>
#include <array>
#include <glm/glm.hpp>
#include <type_traits>
#include <filesystem>
#include "async_image_saver.h"

class opengl_gui : public i_renderer
{
private:
	template <typename T>
	class uniform
	{
	private:
		T value{};
		GLint location = -1;
		GLuint program = 0;

		bool changed = false;
	public:
		uniform() = default;
		uniform(GLuint program, GLint loc) : program(program), location(loc) {}
		uniform(GLuint program, std::string name) : program(program)
		{
			location = glGetUniformLocation(program, name.c_str());
		}

		void set_value(const T& value)
		{
			uniform::value = value;
			changed = true;
		}

		const T& get_value() const noexcept
		{
			return value;
		}

		void update()
		{
			if (!changed) return;
			changed = false;
			if constexpr (std::is_same_v<T, int>)
				glUniform1i(location, value);
			else if constexpr (std::is_same_v<T, float>)
				glUniform1f(location, value);
			else if constexpr (std::is_same_v<T, glm::ivec2>)
				glUniform2iv(location, 1, &value[0]);
			else if constexpr (std::is_same_v<T, glm::vec3>)
				glUniform3fv(location, 1, &value[0]);
			else if constexpr (std::is_same_v<T, glm::mat3>)
				glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
		}
	};

	static void APIENTRY gl_debug_callback_func(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
	static GLuint load_shader(std::string file_path);
	template<size_t N>
	static GLuint create_program(const std::array<GLuint, N> shaders);

	void init_shaders();
	void init_off_screen_fbo();

	GLuint shader_program_off_screen = 0;
	GLuint shader_program_screen = 0;

	GLuint VAO = 0;
	GLuint off_screen_fbo = 0;
	GLuint off_screen_color_texture = 0;

	glm::ivec2 default_fbo_size;
	glm::ivec2 off_screen_fbo_size = { 1920*2, 1080*2 };
	bool resolution_constraint = true;

	image::async_image_saver img_svr{ 10, std::thread::hardware_concurrency() };

protected:
	virtual void safe_destroy() noexcept override;
public:
	opengl_gui(bool resolution_constraint);
	~opengl_gui();
	virtual void on_framebuffer_resize(int x, int y) override;
	virtual void render(const double deltaTime) override;
	void set_resolution_constraint(bool val);
	bool get_resolution_constraint() const;
	void make_screenshot(std::filesystem::path path);

	user_camera camera{ glm::vec3{0, 0, -2}, glm::vec3{0, 0, 1}, glm::vec3{0, 1, 0} };

	struct
	{
		uniform<float> epsilon;
		uniform<float> fov;
		uniform<float> mandelbulb_power;
		uniform<float> view_radius;
		uniform<int> mandelbulb_iter_num;
		uniform<glm::ivec2> offscreen_size_mandelbulb;
		uniform<glm::mat3> cam_basis;
		uniform<glm::vec3> cam_pos;

		uniform<int> screen_texture;
		uniform<glm::ivec2> offscreen_size;
		uniform<glm::ivec2> screen_size;

	} shader_params;
};