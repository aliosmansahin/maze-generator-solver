#pragma once

/*

Maze class that represents a maze structure and provides functionality to generate and solve the maze.

Author: Ali Osman ÞAHÝN

Date: December 7, 2025

*/

#include "Settings.h"
#include "Utils.h"

#include <vector>
#include <stdlib.h>
#include <time.h>
#include <iostream>

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

	void GenerateMaze();
	void UpdateGeneration(); //	Iterative step for generation

	void SolveMaze();

	void UpdateMaze();
	void DrawMaze();
	void PrintMaze(); // For debugging purposes, It prints the maze to console

	bool IsGenerationComplete() const { return generationComplete; }

private:
	void InitializeGrid();
	void CleanupGrid();

private:
	void GenerateStep(Utils::Cell cell); // A single recursive step in maze generation

private:
	const int width;
    const int height;

	bool** grid = nullptr;

private:
	bool generating = false;
	bool generationComplete = false;

	Utils::Cell startCell; // Starting point for maze generation
	Utils::Cell currentCell; // Current cell being processed
	std::vector<Utils::Cell> generationStack; // Stack for iterative generation
};

