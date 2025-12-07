#include "Maze.h"

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
