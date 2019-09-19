#include "opengl_gui.h"

#include "auxiliary.h"
#include <stdexcept>
#include <iostream>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtc/constants.hpp>

void opengl_gui::gl_debug_callback_func(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	// ignore non-significant error/warning codes
	//if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}

GLuint opengl_gui::load_shader(const std::string file_path)
{
	std::string extension = file_path.substr(file_path.find_last_of('.')+1);

	GLenum shader_type = 0;

	if (extension == "vert") shader_type = GL_VERTEX_SHADER;
	else if (extension == "tesc") shader_type = GL_TESS_CONTROL_SHADER;
	else if (extension == "tese") shader_type = GL_TESS_EVALUATION_SHADER;
	else if (extension == "geom") shader_type = GL_GEOMETRY_SHADER;
	else if (extension == "frag") shader_type = GL_FRAGMENT_SHADER;
	else if (extension == "comp") shader_type = GL_COMPUTE_SHADER;
	else
	{
		throw std::runtime_error("opengl_gui::load_shader(...). Can not identify shader type!");
	}

	std::string shader_text = auxiliary::read_text_file(file_path);

	GLuint shader = glCreateShader(shader_type);
	const GLchar *tmp = shader_text.c_str();
	glShaderSource(shader, 1, &tmp, 0);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		throw std::runtime_error(file_path + " - Shader compilation failed!\n" + infoLog);
	};

	return shader;
}

template<size_t N>
GLuint opengl_gui::create_program(const std::array<GLuint, N> shaders)
{
	GLuint program = glCreateProgram();

	for (const GLuint& shader : shaders)
	{
		glAttachShader(program, shader);
	}

	glLinkProgram(program);

	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		throw std::runtime_error(std::string("Shader program linking failed!\n") + infoLog);
	}

	return program;
}

void opengl_gui::init_shaders()
{
	GLuint screen_vert_shader = load_shader("shaders/screen.vert");
	GLuint mandelbulb_frag_shader = load_shader("shaders/mandelbulb.frag");
	GLuint screen_frag_shader = load_shader("shaders/screen.frag");

	const std::array shaders_offscreen = {
		screen_vert_shader,
		mandelbulb_frag_shader
	};
	
	const std::array shaders_screen = {
		screen_vert_shader,
		screen_frag_shader
	};

	shader_program_off_screen = create_program(shaders_offscreen);
	shader_program_screen = create_program(shaders_screen);

	// Get uniform locations
	shader_params.offscreen_size_mandelbulb = uniform<glm::ivec2>(shader_program_off_screen, "offscreen_size");
	shader_params.cam_basis = uniform<glm::mat3>(shader_program_off_screen, "cam_basis");
	shader_params.cam_pos = uniform<glm::vec3>(shader_program_off_screen, "cam_pos");
	shader_params.epsilon = uniform<float>(shader_program_off_screen, "epsilon");
	shader_params.fov = uniform<float>(shader_program_off_screen, "fov");
	shader_params.mandelbulb_power = uniform<float>(shader_program_off_screen, "mandelbulb_power");
	shader_params.mandelbulb_iter_num = uniform<int>(shader_program_off_screen, "mandelbulb_iter_num");
	shader_params.view_radius = uniform<float>(shader_program_off_screen, "view_radius");
	shader_params.screen_texture = uniform<int>(shader_program_screen, "screen_texture");
	shader_params.offscreen_size = uniform<glm::ivec2>(shader_program_screen, "offscreen_size");
	shader_params.screen_size = uniform<glm::ivec2>(shader_program_screen, "screen_size");

	// Set default parameters
	if (resolution_constraint) shader_params.offscreen_size_mandelbulb.set_value(default_fbo_size);
	else shader_params.offscreen_size_mandelbulb.set_value(off_screen_fbo_size);
	shader_params.offscreen_size.set_value(shader_params.offscreen_size_mandelbulb.get_value());
	shader_params.epsilon.set_value(0.002f);
	shader_params.fov.set_value(glm::radians(45.f));
	shader_params.mandelbulb_power.set_value(8);
	shader_params.view_radius.set_value(10);
	shader_params.mandelbulb_iter_num.set_value(15);
	shader_params.screen_texture.set_value(0);
	shader_params.screen_size.set_value(default_fbo_size);

	glDeleteShader(screen_vert_shader);
	glDeleteShader(mandelbulb_frag_shader);
	glDeleteShader(screen_frag_shader);
}

void opengl_gui::init_off_screen_fbo()
{
	glGenFramebuffers(1, &off_screen_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, off_screen_fbo);

	glGenTextures(1, &off_screen_color_texture);
	glBindTexture(GL_TEXTURE_2D, off_screen_color_texture);

	glm::vec2 size = shader_params.offscreen_size.get_value();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[3] = { 1, 1, 1 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, off_screen_color_texture, 0);

}

void opengl_gui::safe_destroy() noexcept
{
	if (off_screen_color_texture) glDeleteTextures(1, &off_screen_color_texture);
	if (off_screen_fbo) glDeleteFramebuffers(1, &off_screen_fbo);
	if (shader_program_screen) glDeleteProgram(shader_program_screen);
	if (shader_program_off_screen) glDeleteProgram(shader_program_off_screen);
	if (VAO) glDeleteVertexArrays(1, &VAO);
}

opengl_gui::opengl_gui(bool resolution_constraint) :
	resolution_constraint(resolution_constraint)
{
	try
	{
#ifdef _DEBUG
		GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(&opengl_gui::gl_debug_callback_func, 0);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}
#endif // DEBUG

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		GLint m_viewport[4];
		glGetIntegerv(GL_VIEWPORT, m_viewport);
		default_fbo_size = glm::ivec2(m_viewport[2], m_viewport[3]);

		init_shaders();
		init_off_screen_fbo();
	}
	catch (const std::exception&)
	{
		safe_destroy();
		throw;
	}
}

opengl_gui::~opengl_gui()
{
	safe_destroy();
}

void opengl_gui::on_framebuffer_resize(int x, int y)
{
	default_fbo_size = glm::ivec2(x, y);
	shader_params.screen_size.set_value(default_fbo_size);

	if (resolution_constraint)
	{
		shader_params.offscreen_size.set_value(default_fbo_size);
		shader_params.offscreen_size_mandelbulb.set_value(default_fbo_size);

		glBindTexture(GL_TEXTURE_2D, off_screen_color_texture);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, default_fbo_size.x, default_fbo_size.y, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, default_fbo_size.x, default_fbo_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	}
}

void opengl_gui::render(const double delta_time)
{
	static double time = 0;
	time += delta_time;

	camera.update(delta_time);

	//animate the camera for demonstration
	//constexpr float total_time = 20;
	//constexpr float fps = 60;
	//constexpr int num_frames = total_time * fps;
	//constexpr float angle = 2 * glm::pi<float>() / num_frames;

	//static int i = 0;
	//if (!resolution_constraint && i < num_frames)
	//{
	//	const float radius = 2.5;
	//	camera.rotate_up(-angle);
	//	glm::vec3 dir = camera.get_dir();
	//	camera.set_pos(-camera.get_dir() * radius);
	//}

	/////

	glm::mat3 cam_basis_mat{ camera.get_right(), camera.get_up(), camera.get_dir() };
	shader_params.cam_basis.set_value(cam_basis_mat);
	shader_params.cam_pos.set_value(camera.get_pos());

	glUseProgram(shader_program_off_screen);
	shader_params.cam_basis.update();
	shader_params.cam_pos.update();
	shader_params.epsilon.update();
	shader_params.fov.update();
	shader_params.mandelbulb_iter_num.update();
	shader_params.mandelbulb_power.update();
	shader_params.offscreen_size_mandelbulb.update();
	shader_params.view_radius.update();

	glBindFramebuffer(GL_FRAMEBUFFER, off_screen_fbo);

	glm::vec2 size = shader_params.offscreen_size.get_value();
	glViewport(0, 0, size.x, size.y);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	//////////////////////////////////////////////////////

	glUseProgram(shader_program_screen);
	shader_params.screen_texture.update();
	shader_params.offscreen_size.update();
	shader_params.screen_size.update();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, off_screen_color_texture);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, default_fbo_size.x, default_fbo_size.y);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	//if (!resolution_constraint && i < num_frames)
	//{
	//	char path[50];
	//	sprintf_s(path, "render/%d.png", i);
	//	make_screenshot(path);
	//	i++;
	//}
}

void opengl_gui::set_resolution_constraint(bool val)
{
	if (resolution_constraint && !val)
	{
		resolution_constraint = val;

		shader_params.offscreen_size.set_value(off_screen_fbo_size);
		shader_params.offscreen_size_mandelbulb.set_value(off_screen_fbo_size);

		glBindTexture(GL_TEXTURE_2D, off_screen_color_texture);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, off_screen_fbo_size.x, off_screen_fbo_size.y, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, off_screen_fbo_size.x, off_screen_fbo_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	}
	else if (!resolution_constraint && val)
	{
		resolution_constraint = val;

		shader_params.offscreen_size.set_value(default_fbo_size);
		shader_params.offscreen_size_mandelbulb.set_value(default_fbo_size);

		glBindTexture(GL_TEXTURE_2D, off_screen_color_texture);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, default_fbo_size.x, default_fbo_size.y, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, default_fbo_size.x, default_fbo_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}
}

bool opengl_gui::get_resolution_constraint() const
{
	return resolution_constraint;
}

void opengl_gui::make_screenshot(std::filesystem::path path)
{
	auto size = shader_params.offscreen_size.get_value();

	using namespace image;

	img_info info;
	info.width = size.x;
	info.height = size.y;
	info.path = path;
	info.format.bit_depth = img_info::pixel_format::BIT_DEPTH_8;
	info.format.color_type = img_info::pixel_format::RGB;
	info.data.resize((uint64_t)size.x * size.y*3);

	glBindTexture(GL_TEXTURE_2D, off_screen_color_texture);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, info.data.data());

	img_svr.save_image(std::move(info));
}
