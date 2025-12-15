#include "Application.h"

bool Application::spacePressed = false;
bool Application::leftMouseClicked = false;
bool Application::rightMousePressed = false;
bool Application::mouseWheelUp = false;
bool Application::mouseWheelDown = false;
int Application::mouseX = 0;
int Application::mouseY = 0;

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

	SetGLFWCallbacks();

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
    /* The cellColor object is uniform variable to draw different colors with a single VAO */
    const char* vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;

        uniform mat4 projection;
        uniform mat4 translation;
        uniform mat4 scale;

        void main()
        {
            gl_Position = projection * scale * translation * vec4(aPos, 0.0, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core

        uniform vec3 cellColor;

        void main()
        {
            gl_FragColor = vec4(cellColor, 1.0);
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

void Application::SetGLFWCallbacks()
{
    glfwSetKeyCallback(window, &Application::KeyCallback);
    glfwSetMouseButtonCallback(window, &Application::MouseButtonCallback);
    glfwSetCursorPosCallback(window, &Application::MousePositionCallback);
    glfwSetScrollCallback(window, &Application::ScrollCallback);
}

void Application::Update()
{
    /* Keyboard events */
    if (spacePressed && phaseCompleted) {
        UpdatePhase();
    }
    if (rightMousePressed) {
        UpdateCameraPosition();
    }
    else
        rightFirstPress = true;
    UpdateCameraZoom();

    /* We update maze only each <mazeUpdateInterval> seconds */
    bool updateMaze = false;

    if (!phaseCompleted &&
        (currentPhase == Utils::Phase::Generation ||
            currentPhase == Utils::Phase::Solving ||
            currentPhase == Utils::Phase::CellSelection ||
            currentPhase == Utils::Phase::Completed)) {
        float currentTime = static_cast<float>(glfwGetTime());

        if (currentTime - lastMazeUpdateTime >= mazeUpdateInterval) {
            updateMaze = true;
			lastMazeUpdateTime = currentTime;
        }
    }

    /* 
        This variable stores the last true state of the mouse left button, until UpdateMaze was called
		This is done to avoid missing clicks between frames when maze update is not performed every frame
    */
	static bool leftMouseClickedLocal = false;
    if(leftMouseClicked)
		leftMouseClickedLocal = true;

    if (maze && updateMaze) {
        maze->UpdateMaze(mouseX, mouseY, cameraX, cameraY, cameraZoom, leftMouseClickedLocal);
        if (IsCurrentPhaseCompleted()) {
            if (maze->IsSolvingComplete()) {
                UpdatePhase();
            }
            else {
                phaseCompleted = true;
            }
        }

		leftMouseClickedLocal = false;
    }

    spacePressed = false;
    leftMouseClicked = false;
    mouseWheelUp = false;
    mouseWheelDown = false;
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

    float scaleMatrix[16] = {
        cameraZoom, 0.0f, 0.0f, 0.0f,
        0.0f, cameraZoom, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "scale"), 1, GL_FALSE, scaleMatrix);

    if(maze)
		maze->DrawMaze(shaderProgram, cameraX, cameraY);
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
	maze->GenerateMaze(cameraX, cameraY, cameraZoom);
}

void Application::HandlePhaseCellSelection()
{
    /* Cell selection handling */
    if (maze)
        maze->StartSelection();
}

void Application::HandlePhaseSolving()
{
    /* Start to solve */
    if(maze)
		maze->SolveMaze();
}

void Application::HandlePhaseCompleted()
{
    /* Maze completed handling */
    if (maze)
        maze->CompleteMaze();
}

/*
PURPOSE: Updates current phase by incrementing it
*/
void Application::UpdatePhase()
{
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
        HandlePhaseCellSelection();
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

bool Application::IsCurrentPhaseCompleted()
{
    return (currentPhase == Utils::Phase::Generation && maze->IsGenerationComplete()) ||
        (currentPhase == Utils::Phase::Solving && maze->IsSolvingComplete()) ||
        (currentPhase == Utils::Phase::CellSelection && maze->IsSelectionComplete()) ||
        (currentPhase == Utils::Phase::Completed && maze->IsCompletionComplete());
}

void Application::UpdateCameraPosition()
{
    if (rightFirstPress) {
        rightFirstPress = false;

        lastMouseX = mouseX;
        lastMouseY = mouseY;
    }

    int deltaMouseX = mouseX - lastMouseX;
    int deltaMouseY = mouseY - lastMouseY;

    cameraX -= deltaMouseX * CAMERA_SENSITIVITY / cameraZoom;
    cameraY -= deltaMouseY * CAMERA_SENSITIVITY / cameraZoom;

    lastMouseX = mouseX;
    lastMouseY = mouseY;
}

void Application::UpdateCameraZoom()
{
    float oldZoom = cameraZoom;

    // Update zoom
    if (mouseWheelUp) {
        cameraZoom += 0.1f;
    }
    if (mouseWheelDown) {
        cameraZoom -= 0.1f;
        if (cameraZoom <= 0.1f)
            cameraZoom = 0.1f;
    }

    // Correct the camera
    if (cameraZoom != oldZoom) {
        cameraX += (mouseX / oldZoom) - (mouseX / cameraZoom);
        cameraY += (mouseY / oldZoom) - (mouseY / cameraZoom);
    }
}

void Application::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        spacePressed = true;
}

void Application::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		leftMouseClicked = true;

    rightMousePressed = (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS);
}

void Application::MousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	mouseX = xpos - WINDOW_WIDTH / 2.0f;
	mouseY = WINDOW_HEIGHT - ypos - WINDOW_HEIGHT / 2.0f;
}

void Application::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset > 0) {
        mouseWheelUp = true;
    }
    else if (yoffset < 0) {
        mouseWheelDown = true;
    }
}
