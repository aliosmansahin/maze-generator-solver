#pragma once

/*

Maze class that represents a maze structure and provides functionality to generate and solve the maze.

Author: Ali Osman ÞAHÝN

Date: December 7, 2025

*/

#include "Settings.h"

#include <vector>

class Maze
{
public:
    Maze() = delete;
	Maze(int w, int h) : width(w), height(h) {
		InitializeGrid();
	}

	Maze(const Maze& other) = delete;
	Maze& operator=(const Maze& other) = delete;

	~Maze() {
		CleanupGrid();
	}

	void GenerateMaze() {};
	void SolveMaze() {};

private:
	const int width;
    const int height;

	bool** grid = nullptr;

private:
	void InitializeGrid();
	void CleanupGrid();
};

