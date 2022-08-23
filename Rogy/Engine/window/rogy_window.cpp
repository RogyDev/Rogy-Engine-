#include "rogy_window.h"

RWindow::RWindow()
{
}

RWindow::~RWindow()
{
}

bool onWindowClose = false;
void OnWinClose(GLFWwindow* win)
{
	onWindowClose = true;
}

bool RWindow::StartWindow(int SCR_weight,int SCR_height, int Major, int Minor)
{
	scr_w = SCR_weight;
	scr_h = SCR_height;

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return false;
	}
	glfwWindowHint(GLFW_SAMPLES, 0);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, Major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, Minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // fix compilation on OS X
#endif

	active = true;

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(SCR_weight, SCR_height, "Engine", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n");
		getchar();
		glfwTerminate();
		return false;
	}

	//Switch context to use on window
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return false;
	}

	glfwSetWindowCloseCallback(window, OnWinClose);

	std::printf("GL Vendor     : %s\n", glGetString(GL_VENDOR));
	std::printf("GL Renderer   : %s\n", glGetString(GL_RENDERER));
	std::printf("GL Version    : %s\n", glGetString(GL_VERSION));
	std::printf("GLSL Version  : %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	// Make sure OpenGL version 4.5 API is available
	if (!GLEW_VERSION_4_5) {
		 std::cout << "OpenGL 4.5 API is not available.\n";
	}

	// Enable any OpenGL features we want to use
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);

	return true;
}

void RWindow::CloseWindow()
{
	active = false;
}

void RWindow::SetWindowIcon(const char* path)
{
	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);

	if (data)
	{
		GLFWimage WindowIcon;
		WindowIcon.pixels = data;
		WindowIcon.width = width;
		WindowIcon.width = height;
	//	glfwSetWindowIcon(window, 1, &WindowIcon);
	}
	else
	{
		std::cout << "Icon failed to load at path: " << path << std::endl;
	}
	stbi_image_free(data);
}

void RWindow::SetWindowTitle(const char* new_title)
{
	if (window != nullptr)
	{
		glfwSetWindowTitle(window, new_title);
	}
	title = new_title;
}

void RWindow::SetWindowSizeCallback(GLFWwindowsizefun fun)
{
	glfwSetWindowSizeCallback(window, fun);
}

void RWindow::GetFramebufferSize(int& scr_w, int& scr_h)
{
	glfwGetFramebufferSize(window, &scr_w, &scr_h);
}

void RWindow::SwapBuffersAndPollEvents()
{
	glfwSwapBuffers(window);
	glfwPollEvents();

	if (onWindowClose)
	{
		CloseWindow();
		onWindowClose = false;
	}
}

void RWindow::Clear()
{
	glfwDestroyWindow(window);
}

void RWindow::Maximize()
{
	glfwMaximizeWindow(window);
}

void RWindow::SetCursorMode(CursorMode mode)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

bool RWindow::IsFullscreen()
{
	return glfwGetWindowMonitor(window) != nullptr;
}

void RWindow::SetFullScreen(bool fullscreen)
{
	if (IsFullscreen() == fullscreen)
		return;

	if (fullscreen)
	{
		// backup window position and window size
		glfwGetWindowPos(window, &last_win_pos_x, &last_win_pos_y);
		glfwGetWindowSize(window, &scr_w, &scr_h);

		// get resolution of monitor
		const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		// switch to full screen
		//glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, 0);
		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, scr_w, scr_h, 0);
	}
	else
	{
		// restore last window size and position
		glfwSetWindowMonitor(window, nullptr, last_win_pos_x, last_win_pos_y, scr_w, scr_h, 0);
	}
}