#include "Maze.h"

void Maze::GenerateMaze()
{
	/* Initialize parameters */
	generationComplete = false;
	generating = true;

	srand(static_cast<unsigned int>(time(nullptr)));

	do {
		startCell = { rand() % width, rand() % height };

	} while (startCell.x % 2 == 0 || startCell.y % 2 == 0); // Ensure start cell is odd indexed

	//GenerateStep(startCell); // This is recursive and may block the main thread
	// Instead, we will implement iterative generation in UpdateGeneration
	// by pushing the start cell onto the stack

	grid[startCell.y][startCell.x] = true; // Mark start cell as part of the maze
	generationStack.push_back(startCell);

	std::cout << "Maze Generation Started from (" << startCell.x << ", " << startCell.y << ")\n";
}

void Maze::StartSelection()
{
	selectingCells = true;
}

void Maze::GenerateStep(Utils::Cell cell)
{
	grid[cell.y][cell.x] = true; // Mark cell as part of the maze

	for(int i = 0; i < 4; ++i) {
		int nextX = cell.x + Utils::GetDirection(i).first * 2;
		int nextY = cell.y + Utils::GetDirection(i).second * 2;

		if (nextX >= 0 && nextX < width && nextY >= 0 && nextY < height && !grid[nextY][nextX]) {
			GenerateStep(Utils::Cell{ nextX, nextY });
		}
	}
}

/*
WARNING: Use delete[] after usage to avoid memory leaks.
*/
float* Maze::GenerateGridVertices(float gridW, float gridH)
{
	float* gridVertices = new float[8];

	gridVertices[0] = -gridW; gridVertices[1] = -gridH;
	gridVertices[2] = gridW; gridVertices[3] = -gridH;
	gridVertices[4] = gridW; gridVertices[5] = gridH;
	gridVertices[6] = -gridW; gridVertices[7] = gridH;

	return gridVertices;
}

void Maze::UpdateGeneration()
{
	/*
		Here we are stepping on generation process
		each call to this function should progress the maze generation
		until it is complete.
	*/

	if (!generationStack.empty()) {
		currentCell = generationStack.back();
		generationStack.pop_back();
		
		// Find unvisited neighbors
		std::vector<Utils::Cell> unvisitedNeighbors;
		for (int i = 0; i < 4; ++i) {
			int nextX = currentCell.x + Utils::GetDirection(i).first * 2;
			int nextY = currentCell.y + Utils::GetDirection(i).second * 2;

			if(nextX >= 0 && nextX < width && nextY >= 0 && nextY < height && !grid[nextY][nextX]) {
				unvisitedNeighbors.push_back(Utils::Cell{ nextX, nextY });
			}
		}

		if (!unvisitedNeighbors.empty()) {
			generationStack.push_back(currentCell); // Push current cell back to stack
			
			int selectedNeighborIndex = rand() % unvisitedNeighbors.size();
			Utils::Cell selectedNeighbor = unvisitedNeighbors[selectedNeighborIndex];

			// Remove wall between current cell and selected neighbor
			int wallX = (currentCell.x + selectedNeighbor.x) / 2;
			int wallY = (currentCell.y + selectedNeighbor.y) / 2;
			grid[wallY][wallX] = true;

			grid[selectedNeighbor.y][selectedNeighbor.x] = true;
			generationStack.push_back(selectedNeighbor);
		}
	}
	else {
		generationComplete = true;
		generating = false;

		std::cout << "Maze Generation Complete!\n";
		PrintMaze();
	}
}

void Maze::UpdateSelection(int mouseX, int mouseY, bool leftMouseClicked)
{
	/* Use mouse position to find pointed cell */

	/* Convert mouse coordinates into our coordinate system */
	mouseY = WINDOW_HEIGHT - mouseY; // Invert Y axis
	mouseX -= WINDOW_WIDTH / 2;
	mouseY -= WINDOW_HEIGHT / 2;

	int cellX = (int)((mouseX + cellHalfSize) / (cellHalfSize * 2.0f) + width / 2.0f);
	int cellY = (int)((mouseY + cellHalfSize) / (cellHalfSize * 2.0f) + height / 2.0f);

	bool mouseInsideMaze = cellX >= 0 && cellX < width && cellY >= 0 && cellY < height;

	/* Clamp cell indices to be within grid bounds */
	if (cellX < 0) cellX = 0;
	if (cellX >= width) cellX = width - 1;

	if (cellY < 0) cellY = 0;
	if (cellY >= height) cellY = height - 1;

	/* Do some checks to avoid point to the walls on inside of the map */
	bool isWall = !grid[cellY][cellX];
	bool isBound = cellX == 0 || cellX == width - 1 || cellY == 0 || cellY == height - 1;

	bool hasNeighborWall = false;

	if (cellX == 0 && !grid[cellY][cellX + 1]) hasNeighborWall = true; // Left
	if (cellX == width - 1 && !grid[cellY][cellX - 1]) hasNeighborWall = true; // Right
	if (cellY == 0 && !grid[cellY + 1][cellX]) hasNeighborWall = true; // Down
	if (cellY == height - 1 && !grid[cellY - 1][cellX]) hasNeighborWall = true; // Up

	/* Set pointed cell if there is no issue from the checks */
	if((!isWall || (isBound && !hasNeighborWall)) && mouseInsideMaze) {
		pointedCell = { cellX, cellY };
		pointing = true;
	}
	else {
		pointing = false;
	}
}

void Maze::SolveMaze()
{
	solving = true;
	solvingComplete = false;

	/*
		TODO: We will select some pathfinding algorithm to solve the maze here
		But before that, i will add selection mechanism of start and end points for solving
	*/
}

void Maze::UpdateMaze(int mouseX, int mouseY, bool leftMouseClicked)
{
	if(generating && !generationComplete) {
		UpdateGeneration();
	}
	if (selectingCells) {
		UpdateSelection(mouseX, mouseY, leftMouseClicked);
	}
}

void Maze::DrawMaze(unsigned int shaderProgram)
{
	for(int i = 0; i < height; ++i) {
		for(int j = 0; j < width; ++j) {
			if(!grid[i][j]) {
				DrawCell(shaderProgram, 1.0f, 1.0f, 1.0f, Utils::Cell{ j, i });
			}
		}
	}

	if(!generationStack.empty())
		DrawCell(shaderProgram, 0.0f, 1.0f, 0.0f, generationStack.back());

	if(selectingCells && pointing)
		DrawCell(shaderProgram, 1.0f, 0.0f, 0.0f, pointedCell);
}

/*
PURPOSE: Draws a single cell at given position with specified color.
	This function sends the color of the cell to the fragment shader
	to avoid creating multiple VAOs for different colors.
*/
void Maze::DrawCell(unsigned int shaderProgram, float r, float g, float b, Utils::Cell cell)
{
	/* Send translation matrix */
	int translateX = cell.x * cellHalfSize * 2 - width * cellHalfSize;
	int translateY = cell.y * cellHalfSize * 2 - height * cellHalfSize;
	float translationMatrix[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		(float)translateX, (float)translateY, 0, 1
	};
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "translation"), 1, GL_FALSE, translationMatrix);

	/* Send color */
	glUniform3f(glGetUniformLocation(shaderProgram, "cellColor"), r, g, b);

	/* Render cell here */
	glBindVertexArray(mazeCellVAO); // Bind VAO
	glDrawArrays(GL_QUADS, 0, 4);
	glBindVertexArray(0); // Unbind VAO
}

void Maze::PrintMaze()
{
	for(int i = height - 1; i >= 0; --i) {
		for(int j = 0; j < width; ++j) {
			std::cout << (grid[i][j] ? "  " : "##");
		}
		std::cout << std::endl;
	}
}

void Maze::InitializeGrid()
{
	/* Setup the variables */
	generating = false;
	generationComplete = false;
	solving = false;
	solvingComplete = false;
	pointing = false;
	selectingCells = false;

	/* Allocate memory for grid */
	grid = new bool* [height];
	for (int i = 0; i < height; ++i) {
		grid[i] = new bool[width]{};
	}

	float* gridVertices = GenerateGridVertices((float)cellHalfSize, (float)cellHalfSize);

	glGenBuffers(1, &mazeCellBuffer);
	glGenVertexArrays(1, &mazeCellVAO);
	
	glBindVertexArray(mazeCellVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mazeCellBuffer);
	
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), gridVertices, GL_STATIC_DRAW);

	glEnableVertexArrayAttrib(mazeCellBuffer, 0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind buffer
	glBindVertexArray(0); // Unbind VAO
	delete[] gridVertices; // Free allocated memory
}

void Maze::CleanupGrid()
{
	/* Deallocate memory for grid */
	if (grid) {
		for (int i = 0; i < height; ++i) {
			delete[] grid[i];
		}
		delete[] grid;
		grid = nullptr;
	}

	glDeleteBuffers(1, &mazeCellBuffer);
	glDeleteVertexArrays(1, &mazeCellVAO);
}
