#ifndef ROGY_WINDOW
#define ROGY_WINDOW
#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <stb_image.h>

#include <iostream>
#include <string>

enum CursorMode
{
	CM_DISABLED = GLFW_CURSOR_DISABLED,
	CM_HIDDEN = GLFW_CURSOR_HIDDEN,
	CM_NORMAL = GLFW_CURSOR_NORMAL
};
class RWindow
{
public:
	RWindow();
	~RWindow();

	GLFWwindow* window = nullptr;
	const char* title;
	bool active = false;
	bool StartWindow(int SCR_weight, int SCR_height, int Major = 3, int Minor = 0);
	void CloseWindow();
	void SetWindowTitle(const char* new_title);
	void SetWindowIcon(const char* path);
	void SetWindowSizeCallback(GLFWwindowsizefun fun);
	void GetFramebufferSize(int& scr_w, int& scr_h);
	void SwapBuffersAndPollEvents();
	bool IsFullscreen();
	void SetFullScreen(bool fullscreen);
	void Clear();
	void Maximize();

	void SetCursorMode(CursorMode mode);
private:
	int last_win_pos_x, last_win_pos_y;
	int scr_w, scr_h;
	bool fullscreen = false;
};

#endif // ! ROGY_WINDOW