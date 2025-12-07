#include "Application.h"

/*
PURPOSE: This function runs the main loop of the application,
	continuously rendering frames until the window is closed.

WARNING: Call this function only after Initialize() has been called.
*/
void Application::Run()
{
    while (!glfwWindowShouldClose(window))
    {
        /* Render frame here */
		glClear(GL_COLOR_BUFFER_BIT);

#ifdef DEBUG_CLEAR_COLOR
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Clear to red color
#else
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Clear to dark gray color
#endif

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
