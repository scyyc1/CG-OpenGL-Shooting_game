#include <GLFW/glfw3.h> 

GLFWwindow* createWindow(int w, int h, const char* title)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "A triangle", NULL, NULL);
	glfwMakeContextCurrent(window);

	return window;
}