#pragma once

#include "OpenGLStuff.hpp"
#include <GLFW/glfw3.h>

static GLFWwindow* s_Window = nullptr;
static uint32_t	   s_Width  = 800;
static uint32_t	   s_Height = 600;

bool Init(int32_t width, int32_t height, const char* title);
void Run();

GLFWwindow* GetWindow();