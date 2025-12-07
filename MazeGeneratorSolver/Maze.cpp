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
float* Maze::GenerateGridVertices(float gridW, float gridH, float r, float g, float b)
{
	float* gridVertices = new float[20];

	gridVertices[0] = -gridW; gridVertices[1] = -gridH; gridVertices[2] = r; gridVertices[3] = g; gridVertices[4] = b;
	gridVertices[5] = gridW; gridVertices[6] = -gridH; gridVertices[7] = r; gridVertices[8] = g; gridVertices[9] = b;
	gridVertices[10] = gridW; gridVertices[11] = gridH; gridVertices[12] = r; gridVertices[13] = g; gridVertices[14] = b;
	gridVertices[15] = -gridW; gridVertices[16] = gridH; gridVertices[17] = r; gridVertices[18] = g; gridVertices[19] = b;

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

void Maze::SolveMaze()
{
	/* TODO: Implement Solving */
}

void Maze::UpdateMaze()
{
	if(generating && !generationComplete) {
		UpdateGeneration();
	}
}

void Maze::DrawMaze(unsigned int shaderProgram)
{
	for(int i = 0; i < height; ++i) {
		for(int j = 0; j < width; ++j) {
			if(!grid[i][j]) {
				DrawCell(shaderProgram, mazeCellVAO, Utils::Cell{ j, i });
			}
		}
	}

	if(generationStack.empty())
		DrawCell(shaderProgram, currentCellVAO, startCell);
	else
		DrawCell(shaderProgram, currentCellVAO, generationStack.back());
}

void Maze::DrawCell(unsigned int shaderProgram, unsigned int vertexArrayToDraw, Utils::Cell cell)
{
	int translateX = cell.x * cellHalfSize * 2 - width * cellHalfSize;
	int translateY = cell.y * cellHalfSize * 2 - height * cellHalfSize;
	float translationMatrix[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		(float)translateX, (float)translateY, 0, 1
	};
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "translation"), 1, GL_FALSE, translationMatrix);

	/* Render cell here */
	glBindVertexArray(vertexArrayToDraw); // Bind VAO
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
	/* Allocate memory for grid */
	grid = new bool* [height];
	for (int i = 0; i < height; ++i) {
		grid[i] = new bool[width]{};
	}

	float* gridVertices = GenerateGridVertices((float)cellHalfSize, (float)cellHalfSize, 1.0f, 1.0f, 1.0f);

	glGenBuffers(1, &mazeCellBuffer);
	glGenVertexArrays(1, &mazeCellVAO);
	
	glBindVertexArray(mazeCellVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mazeCellBuffer);
	
	glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), gridVertices, GL_STATIC_DRAW);

	glEnableVertexArrayAttrib(mazeCellBuffer, 0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind buffer
	glBindVertexArray(0); // Unbind VAO
	delete[] gridVertices; // Free allocated memory

	float* currentCellVertices = GenerateGridVertices((float)cellHalfSize, (float)cellHalfSize, 0.0f, 1.0f, 0.0f);

	glGenBuffers(1, &currentCellBuffer);
	glGenVertexArrays(1, &currentCellVAO);
	
	glBindVertexArray(currentCellVAO);
	glBindBuffer(GL_ARRAY_BUFFER, currentCellBuffer);
	
	glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), currentCellVertices, GL_STATIC_DRAW);
	
	glEnableVertexArrayAttrib(currentCellBuffer, 0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	delete[] currentCellVertices;
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
