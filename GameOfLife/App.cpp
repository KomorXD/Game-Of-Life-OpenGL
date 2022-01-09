#include "App.hpp"
#include "GameScene.hpp"

#include <iostream>

bool Init(int32_t width, int32_t height, const char* title)
{
	s_Width  = width;
	s_Height = height;

	if(!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW\n";

		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	s_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);

	if(!s_Window)
	{
		glfwTerminate();

		std::cerr << "Failed to create window\n";

		return false;
	}

	glfwMakeContextCurrent(s_Window);
	glfwSwapInterval(0);

	if(!gladLoadGL())
	{
		glfwTerminate();

		std::cerr << "Failed to create OpenGL context\n";

		return false;
	}

	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glDepthFunc(GL_LESS));
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	return true;
}

void Run()
{
	GameScene* scene = new GameScene(s_Width, s_Height);

	float prevFrame = 0.0f;
	float currFrame = 0.0f;
	float dt = 0.0f;

	while(!glfwWindowShouldClose(s_Window))
	{
		currFrame = (float)glfwGetTime();
		dt = std::min(currFrame - prevFrame, 1 / 30.0f);
		prevFrame = currFrame;

		scene->Input();
		scene->Update(dt);
		scene->Render();

		glfwSwapBuffers(s_Window);
		glfwPollEvents();
	}

	delete scene;

	glfwTerminate();
}

GLFWwindow* GetWindow()
{
	return s_Window;
}
