#pragma once

/*

Application class that manages the main loop and initialization of the Maze Generator and Solver program.

Author: Ali Osman ÞAHÝN

Date: December 7, 2025

*/

/* Helpers */
#include "Settings.h"
#include "Utils.h"

/* Maze class */
#include "Maze.h"

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
	~Application() 
	{
		Cleanup();
	}

	void Run();
	void Initialize();
	void Cleanup();

private:
	void CreateWindow();

	void InitializeShaders();

	void Update();
	void Render();

	void HandlePhaseIdle();
	void HandlePhaseGeneration();
	void HandlePhaseSolving();
	void HandlePhaseCompleted();

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
	GLFWwindow* window = nullptr;

	const int width = WINDOW_WIDTH;
	const int height = WINDOW_HEIGHT;
	const std::string title = WINDOW_TITLE;

	unsigned int shaderProgram = 0;

	Maze* maze = nullptr;

	Utils::Phase currentPhase = Utils::Phase::Idle;
	bool phaseCompleted = true;

	static bool spacePressed;

	float lastMazeUpdateTime = 0.0f;
	float mazeUpdateInterval = 0.1f; // Update maze every 0.1 seconds
};