#pragma once

/*

Application class that manages the main loop and initialization of the Maze Generator and Solver program.

Author: Ali Osman ÞAHÝN

Date: December 7, 2025

*/

#include "Settings.h"

/* GLAD and GLFW */
#include <glad/glad.h>
#include <glfw3.h>

/* STL */
#include <string>
#include <iostream>

class Application
{
public:
	/* Constructors to vary window creation process */
	Application() = default;

	/* Avoid memory leaks by disallowing copying */
	Application(const Application& other) = delete;
	Application& operator=(const Application& other) = delete;

	/* Default destructor */
	~Application() = default; 

	void Run();
	void Initialize();

private:
	void CreateWindow();

private:
	GLFWwindow* window = nullptr;

	int width = MANIPULATE_WINDOW_WIDTH;
	int height = MANIPULATE_WINDOW_HEIGHT;
	std::string title = MANIPULATE_WINDOW_TITLE;
};

