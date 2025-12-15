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
	void SetGLFWCallbacks();

	void Update();
	void Render();

	/* Phase */
	void HandlePhaseIdle();
	void HandlePhaseGeneration();
	void HandlePhaseCellSelection();
	void HandlePhaseSolving();
	void HandlePhaseCompleted();

	void UpdatePhase();
	bool IsCurrentPhaseCompleted();


	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void MousePositionCallback(GLFWwindow* window, double xpos, double ypos);

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
	static bool leftMouseClicked;
	static int mouseX;
	static int mouseY;

	float lastMazeUpdateTime = 0.0f;
	float mazeUpdateInterval = SIMULATION_INTERVAL;
};