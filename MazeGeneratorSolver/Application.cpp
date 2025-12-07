#include "Application.h"

bool Application::spacePressed = false;

/*
PURPOSE: This function runs the main loop of the application,
	continuously rendering frames until the window is closed.

WARNING: Call this function only after Initialize() has been called.
*/
void Application::Run()
{
    while (!glfwWindowShouldClose(window))
    {
		Update();
		Render();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

/*
PURPOSE: This function initializes the application by
	setting up GLFW, creating a window, and loading OpenGL functions using GLAD.
*/
void Application::Initialize()
{
    if(glfwInit() == GLFW_FALSE) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        throw std::runtime_error("GLFW initialization failed.");
	}

	CreateWindow();

	glfwSetKeyCallback(window, &Application::KeyCallback);
}

void Application::Cleanup()
{
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
	glfwTerminate();
}

void Application::CreateWindow()
{
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw std::runtime_error("Window creation failed.");
    }

    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to load GLAD" << std::endl;
        throw std::runtime_error("GLAD loading failed.");
    }
}

void Application::Update()
{
    /* Keyboard events */
    if (spacePressed && phaseCompleted) {
        phaseCompleted = false;
        currentPhase = GetNextPhase(currentPhase);
    }

    if (!phaseCompleted) {
        switch (currentPhase)
        {
        case Utils::Phase::Idle:
			std::cout << "Phase: Idle" << std::endl;
            HandlePhaseIdle();
            break;
        case Utils::Phase::Generation:
            std::cout << "Phase: Generation" << std::endl;
            HandlePhaseGeneration();
            break;
        case Utils::Phase::Solving:
            std::cout << "Phase: Solving" << std::endl;
            HandlePhaseSolving();
            break;
        case Utils::Phase::Completed:
            std::cout << "Phase: Completed" << std::endl;
            HandlePhaseCompleted();
            break;
        default:
            break;
        }
    }

    spacePressed = false;
}

void Application::Render()
{
    /* Render frame here */
    glClear(GL_COLOR_BUFFER_BIT);

#ifdef DEBUG_CLEAR_COLOR
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Clear to red color
#else
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Clear to dark gray color
#endif
}

void Application::HandlePhaseIdle()
{
    /* Clean old maze */
    if (maze) {
        delete maze;
        maze = nullptr;
    }
	phaseCompleted = true;
}

void Application::HandlePhaseGeneration()
{
    /* Create new maze */
    if (!maze) {
        maze = new Maze(20, 20); // Example size
    }

    /* Start to generate */
	maze->GenerateMaze();

	// TODO: After generation is complete, set phaseCompleted to true
	phaseCompleted = true;
}

void Application::HandlePhaseSolving()
{
    /* Start to solve */
    if(maze)
		maze->SolveMaze();

    // TODO: After solving is complete, set phaseCompleted to true
    phaseCompleted = true;
}

void Application::HandlePhaseCompleted()
{
    /* Maze completed handling */
	phaseCompleted = true;
}

void Application::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        spacePressed = true;
}
