#include "application.h"
#include <stdexcept>
#include <iostream>
#include <cmath>
#include "auxiliary.h"
#include <filesystem>

void application::process_key_events(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	application* app = reinterpret_cast<application*>(glfwGetWindowUserPointer(window));

	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, 1);
		break;
	case GLFW_KEY_R:
		if (action == GLFW_PRESS)
			app->gui->set_resolution_constraint(!app->gui->get_resolution_constraint());
		break;
	case GLFW_KEY_F11:
		if (action == GLFW_PRESS)
			app->set_fullscreen(!app->get_fullscreen());
		break;
	case GLFW_KEY_PRINT_SCREEN:
		if (action == GLFW_PRESS)
		{
			time_t rawtime;
			struct tm timeinfo;
			char date[30];

			time(&rawtime);
			localtime_s(&timeinfo, &rawtime);

			strftime(date, sizeof(date), "%Y_%m_%d__%H_%M_%S", &timeinfo);

			char path[60];
			sprintf_s(path, "screenshots/%s.png", date);

			app->gui->make_screenshot(path);
		}
		break;
	default:
		break;
	}

	app->camera_movement_control(key, action);
}

void application::process_framebuffer_size_events(GLFWwindow * window, int width, int height)
{
	application* app = reinterpret_cast<application*>(glfwGetWindowUserPointer(window));
	if (width != 0 && height != 0)
	{
		app->gui->on_framebuffer_resize(width, height);
	}
}

void application::process_window_size_events(GLFWwindow * window, int width, int height)
{
	application* app = reinterpret_cast<application*>(glfwGetWindowUserPointer(window));

	if (!app->get_fullscreen())
	{
		app->wnd.size.x = width;
		app->wnd.size.y = height;
	}
}

void application::process_window_pos_events(GLFWwindow * window, int posx, int posy)
{
	application* app = reinterpret_cast<application*>(glfwGetWindowUserPointer(window));
	if (!app->get_fullscreen())
	{
		app->wnd.pos.x = posx;
		app->wnd.pos.y = posy;
	}
}

void application::process_window_iconify_events(GLFWwindow* window, int iconified)
{
	application* app = reinterpret_cast<application*>(glfwGetWindowUserPointer(window));

	if (iconified)
	{
		app->idle = true;
	}
	else
	{
		app->idle = false;
	}
}

void application::process_cursor_pos_events(GLFWwindow* window, double x, double y)
{
	application* app = reinterpret_cast<application*>(glfwGetWindowUserPointer(window));

	app->camera_rotation_control(x, y);

	app->cursor.pos.x = x;
	app->cursor.pos.y = y;
}

void application::process_mouse_button_events(GLFWwindow* window, int button, int action, int mods)
{
	application* app = reinterpret_cast<application*>(glfwGetWindowUserPointer(window));

	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		app->cursor.lmb = action == GLFW_PRESS;
		if (app->cursor.lmb)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else 
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		app->cursor.rmb = action == GLFW_PRESS;
		break;
	default:
		break;
	}
}

void application::process_mouse_scroll_events(GLFWwindow* window, double xoffset, double yoffset)
{
	application* app = reinterpret_cast<application*>(glfwGetWindowUserPointer(window));

	if(glfwGetKey(window, GLFW_KEY_1) != GLFW_RELEASE)
		app->gui->camera.velocity *= pow(0.9, -yoffset);

	if (glfwGetKey(window, GLFW_KEY_2) != GLFW_RELEASE)
	{
		float val = app->gui->shader_params.fov.get_value() + glm::radians(yoffset) * 0.5f;
		if(val > 0 && val < glm::radians(90.f)) app->gui->shader_params.fov.set_value(val);
	}
	if (glfwGetKey(window, GLFW_KEY_3) != GLFW_RELEASE)
	{
		float val = app->gui->shader_params.view_radius.get_value() * pow(0.9, -yoffset);
		if(val > 0) app->gui->shader_params.view_radius.set_value(val);
	}
	if (glfwGetKey(window, GLFW_KEY_4) != GLFW_RELEASE)
	{
		float val = app->gui->shader_params.epsilon.get_value() * pow(0.9, yoffset);
		if (val > 0) app->gui->shader_params.epsilon.set_value(val);
	}
	if (glfwGetKey(window, GLFW_KEY_5) != GLFW_RELEASE)
	{
		float val = app->gui->shader_params.mandelbulb_power.get_value() + yoffset * 0.1f;
		if (val > 0) app->gui->shader_params.mandelbulb_power.set_value(val);
	}
	if (glfwGetKey(window, GLFW_KEY_6) != GLFW_RELEASE)
	{
		int val = app->gui->shader_params.mandelbulb_iter_num.get_value() + yoffset;
		if (val > 0) app->gui->shader_params.mandelbulb_iter_num.set_value(val);
	}
}

void application::init_window()
{
	if (!glfwInit())
	{
		throw std::runtime_error("application::init_window(). glfwInit error!");
	}

#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if(!get_fullscreen())
		window = glfwCreateWindow(wnd.size.x, wnd.size.y, app_name, nullptr, nullptr);
	else
	{
		int count;
		const GLFWvidmode* modes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &count);

		GLFWvidmode best_mode = modes[count - 1];
		window = glfwCreateWindow(best_mode.width, best_mode.height, app_name, glfwGetPrimaryMonitor(), nullptr);
	}

	if (!window)
	{
		throw std::runtime_error("application::init_window(). glfwCreateWindow error!");
	}

	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);
	glfwSwapInterval(1);
	glfwGetWindowPos(window, &wnd.pos.x, &wnd.pos.y);
	glfwSetWindowSizeLimits(window, wnd.min_size.x, wnd.min_size.y, GLFW_DONT_CARE, GLFW_DONT_CARE);

	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	glfwSetKeyCallback(window, process_key_events);
	glfwSetFramebufferSizeCallback(window, process_framebuffer_size_events);
	glfwSetWindowSizeCallback(window, process_window_size_events);
	glfwSetWindowPosCallback(window, process_window_pos_events);
	glfwSetWindowIconifyCallback(window, process_window_iconify_events);
	glfwSetCursorPosCallback(window, process_cursor_pos_events);
	glfwSetMouseButtonCallback(window, process_mouse_button_events);
	glfwSetScrollCallback(window, process_mouse_scroll_events);
}

void application::parse_parameters(int argc, char** argv, bool& resolution_constraint)
{
	app_path = *argv;
	for (const char*const* str = argv + 1; str != argv + argc; str++)
	{
		std::string arg = *str;
		size_t equal_sign_pos = arg.find('=');
		if (equal_sign_pos != std::string::npos)
		{
			std::string param_name = arg.substr(0, equal_sign_pos);
			std::string param_value = arg.substr(equal_sign_pos+1);

			//std::cout << param_name << " = " << param_value << std::endl;

			if (param_name == "wndWidth")
			{
				int wnd_width = std::stoi(param_value);
				wnd.size.x = wnd_width >= wnd.min_size.x ? wnd_width : wnd.min_size.x;
			}
			else if (param_name == "wndHeight")
			{
				int wnd_height = std::stoi(param_value);
				wnd.size.y = wnd_height >= wnd.min_size.y ? wnd_height : wnd.min_size.y;
			}
			else if (param_name == "fullscreen")
			{
				if (param_value == "true") fullscreen = true;
			}
			else if (param_name == "resolution_constraint")
			{
				resolution_constraint = param_value != "false";
			}
		}
	}
}

void application::safe_destroy() noexcept
{
	if (gui) delete gui;
	glfwTerminate();
}

void application::camera_movement_control(int key, int action)
{
	bool pressed = action != GLFW_RELEASE;

	switch (key)
	{
	case GLFW_KEY_W:
		gui->camera.move_front = pressed;
		break;
	case GLFW_KEY_S:
		gui->camera.move_back = pressed;
		break;
	case GLFW_KEY_A:
		gui->camera.move_left = pressed;
		break;
	case GLFW_KEY_D:
		gui->camera.move_right = pressed;
		break;
	case GLFW_KEY_SPACE:
		gui->camera.move_up = pressed;
		break;
	case GLFW_KEY_LEFT_SHIFT:
		gui->camera.move_down = pressed;
		break;
	case GLFW_KEY_Q:
		gui->camera.roll_left = pressed;
		break;
	case GLFW_KEY_E:
		gui->camera.roll_right = pressed;
		break;
	default:
		break;
	}
}

void application::camera_rotation_control(double x, double y)
{
	if (!cursor.lmb) return;

	double dx = x - cursor.pos.x;
	double dy = y - cursor.pos.y;

	float multiplier = gui->shader_params.fov.get_value()*0.002;

	gui->camera.rotate_right(dy * multiplier);
	gui->camera.rotate_up(dx * multiplier);
}

//void application::print_parameters()
//{
//	while (print_thread_is_running)
//	{
//		system("cls");
//
//		std::cout << "1) camera velocity = " << gui->camera.velocity << std::endl;
//		std::cout << "2) fov = " << gui->shader_params.fov.get_value() << std::endl;
//		std::cout << "3) view radius = " << gui->shader_params.view_radius.get_value() << std::endl;
//		std::cout << "4) epsilon = " << gui->shader_params.epsilon.get_value() << std::endl;
//		std::cout << "5) mandelblob power = " << gui->shader_params.mandelbulb_power.get_value() << std::endl;
//		std::cout << "6) mandelblob iterations number = " << gui->shader_params.mandelbulb_iter_num.get_value() << std::endl;
//
//		std::this_thread::sleep_for(std::chrono::duration<unsigned, std::milli>(100));
//	}
//}

application::application(int argc, char** argv)
{
	try
	{
		bool resolution_constraint = true;
		parse_parameters(argc, argv, resolution_constraint);
		init_window();

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			throw std::runtime_error("application::application(int argc, char** argv). Failed to LoadGL!");
		}

		gui = new opengl_gui(resolution_constraint);
	}
	catch (const std::exception&)
	{
		safe_destroy();
		throw;
	}
}

application::~application()
{
	safe_destroy();
}

void application::run()
{
	while (!glfwWindowShouldClose(window))
	{
		if (idle)
		{
			glfwWaitEvents();
			continue;
		}
		else glfwPollEvents();

		timer.tick();
		gui->render(timer.get_delta_time());
		glfwSwapBuffers(window);
	}
}

void application::set_fullscreen(bool fullscreen)
{
	this->fullscreen = fullscreen;

	int count;
	const GLFWvidmode* modes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &count);
	//for (const GLFWvidmode* video_mode = modes; video_mode != modes + count; video_mode++)
	//{
	//	std::cout << video_mode->width << "x" << video_mode->height << std::endl;
	//}

	GLFWvidmode best_mode = modes[count-1];

	if (fullscreen)
	{
		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, best_mode.width, best_mode.height, best_mode.refreshRate);
	}
	else
	{
		glfwSetWindowMonitor(window, nullptr, wnd.pos.x, wnd.pos.y, wnd.size.x, wnd.size.y, GLFW_DONT_CARE);
	}
}

bool application::get_fullscreen()
{
	return fullscreen;
}
