
#include <iostream>

#define GLEW_STATIC
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLFW
#include <DearImGUI/imgui.h>
#include <DearImGUI/imgui_impl_glfw.h>
#include <DearImGUI/imgui_impl_opengl3.h>

#include "../include/chat-app.hpp"

int main(void) {

	// Initialise GLFW

	if (!glfwInit()) {
		std::cout << "Fatal Error: " << "Failed to initialise GLFW" << '\n';
		exit(EXIT_FAILURE);
	}

	// Create the window

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(640, 480, "Window", nullptr, nullptr);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(2);

	// Initialise GLEW

	if (glewInit() != GLEW_OK) {
		std::cout << "Fatal Error: " << "Failed to initialise GLEW" << '\n';
		exit(EXIT_FAILURE);
	}

	// Initialise IMGUI

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(640, 480);
	io.Fonts->AddFontDefault();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Intialise WinSock

	WinSockInit();

	// Setup Chat App

	ChatApp chat;

	if (!chat.Start()) {
		std::cout << "Fatal Error: " << "Failed to start Chat App" << '\n';
		exit(EXIT_FAILURE);
	}

	chat.SendMessage("Hello!");

	// Window loop

	while (!glfwWindowShouldClose(window)) {
		// Events

		glfwPollEvents();

		// New Frame

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// GUI - Chat box

		ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.75f, io.DisplaySize.y * 0.9f));
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::Begin("Chat", nullptr, ImGuiWindowFlags_NoMove);

		ImGui::End();

		// GUI - User list

		ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.25f, io.DisplaySize.y * 1.0f));
		ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.75f, 0.0f));
		ImGui::Begin("Users", nullptr, ImGuiWindowFlags_NoMove);

		ImGui::End();

		// GUI - Input box

		static char chat_input[256] = { 0 };

		ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.75f, io.DisplaySize.y * 0.1f));
		ImGui::SetNextWindowPos(ImVec2(0.0f, io.DisplaySize.y * 0.9f));
		ImGui::Begin("Input", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
		ImGui::InputText(" ", chat_input, sizeof(chat_input));
		ImGui::SameLine();
		ImGui::SetScrollY(ImGui::GetScrollMaxY());

		if (ImGui::Button("Send")) {

		}

		ImGui::End();

		// Render

		ImGui::Render();
		glViewport(0, 0, 640, 480);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap buffers

		glfwSwapBuffers(window);
	}

	// Cleanup

	ImGui::DestroyContext();

	glfwTerminate();
}
