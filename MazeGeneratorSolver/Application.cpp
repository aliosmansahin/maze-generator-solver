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

    InitializeShaders();
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

void Application::InitializeShaders()
{
    const char* vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec3 aColor;
        out vec4 outColor;
        uniform mat4 projection;
        uniform mat4 translation;

        void main()
        {
            gl_Position = projection * translation * vec4(aPos, 0.0, 1.0);
            outColor = vec4(aColor, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec4 outColor;
        void main()
        {
            gl_FragColor = outColor;
        }
    )";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Application::Update()
{
    /* Keyboard events */
    if (spacePressed && phaseCompleted) {
        phaseCompleted = false;
        currentPhase = GetNextPhase(currentPhase);

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
		case Utils::Phase::CellSelection:
            std::cout << "Phase: Cell Selection" << std::endl;
            HandlePhaseCellSelection(); // Not implemented yet
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

    /* We update maze only each <mazeUpdateInterval> seconds */
    bool updateMaze = false;

    if (!phaseCompleted && (currentPhase == Utils::Phase::Generation || currentPhase == Utils::Phase::Solving || currentPhase == Utils::Phase::CellSelection)) {
        float currentTime = static_cast<float>(glfwGetTime());

        if (currentTime - lastMazeUpdateTime >= mazeUpdateInterval) {
            updateMaze = true;
			lastMazeUpdateTime = currentTime;
        }
    }

    if (maze && updateMaze) {
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

        maze->UpdateMaze(mouseX, mouseY);
        if (currentPhase == Utils::Phase::Generation && maze->IsGenerationComplete()) {
            phaseCompleted = true;
        }
    }
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

    float orthoMatrix[16] = {
        2.0f / (float)width,     0,              0,  0,
        0,             2.0f / (float)height,     0,  0,
        0,             0,             -2,  0,
        0,             0,              0,  1
    };

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, orthoMatrix);

    if(maze)
		maze->DrawMaze(shaderProgram);
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
        maze = new Maze(21, 21); // Example size
    }

    /* Start to generate */
	maze->GenerateMaze();
}

void Application::HandlePhaseCellSelection()
{
    /* Cell selection handling */
    if (maze)
        maze->StartSelection();

	//phaseCompleted = true;
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
