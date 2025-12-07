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

void Maze::DrawMaze()
{
	/* Render maze here */
}

void Maze::PrintMaze()
{
	for(int i = 0; i < height; ++i) {
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
}
