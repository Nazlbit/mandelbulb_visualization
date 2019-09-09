#pragma once
#include "i_safe_destructible.h"
#include "opengl_gui.h"

#include <GLFW/glfw3.h>
#include <string>
#include <array>
#include <glm/vec2.hpp>
#include <thread>

#include "timer.h"

class application : public i_safe_destructible
{
private:
	timer timer;
	GLFWwindow* window;
	opengl_gui* gui;

	struct
	{
		const glm::ivec2 min_size = { 200, 200 };
		glm::ivec2 size = { 800, 600 };
		glm::ivec2 pos;
	} wnd;
	
	const char* app_name = "Mandelbulb Visualization";
	std::string app_path;
	bool fullscreen = false;
	bool idle = false;

	struct
	{
		glm::dvec2 prev_pos{ 0, 0 };
		glm::dvec2 pos{ 0, 0 };
		bool lmb = false;
		bool rmb = false;
	} cursor;

	static void process_key_events(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void process_framebuffer_size_events(GLFWwindow* window, int width, int height);
	static void process_window_size_events(GLFWwindow* window, int width, int height);
	static void process_window_pos_events(GLFWwindow * window, int posx, int posy);
	static void process_window_iconify_events(GLFWwindow* window, int iconified);
	static void process_cursor_pos_events(GLFWwindow* window, double x, double y);
	static void process_mouse_button_events(GLFWwindow* window, int button, int action, int mods);
	static void process_mouse_scroll_events(GLFWwindow* window, double xoffset, double yoffset);

	void init_window();
	void parse_parameters(int argc, char** argv, bool &resolution_constraint);
	void safe_destroy() noexcept override;
	void camera_movement_control(int key, int action);
	void camera_rotation_control(double x, double y);

public:
	application(int argc, char** argv);
	application(const application&) = delete;
	application& operator=(const application&) = delete;
	~application();

	void run();
	void set_fullscreen(bool fullscreen);
	bool get_fullscreen();
};

