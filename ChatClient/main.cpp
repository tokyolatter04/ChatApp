
#include <iostream>

#define GLEW_STATIC
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

int main(void) {

	// Initialise GLFW

	glfwInit();

	// Create the window

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(640, 480, "Window", nullptr, nullptr);

	glfwSwapInterval(2);

	// Initialise GLEW

	if (!glewInit()) {

	}

	// Window loop

	while (!glfwWindowShouldClose(window)) {
		// Events

		glfwPollEvents();

		// Swap buffers

		glfwSwapBuffers(window);
	}

}
