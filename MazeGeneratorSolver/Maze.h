#pragma once

/*

Maze class that represents a maze structure and provides functionality to generate and solve the maze.

Author: Ali Osman ÞAHÝN

Date: December 7, 2025

*/

#include "Settings.h"
#include "Utils.h"

#include <glad/glad.h>

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
	void StartSelection();
	void SolveMaze();

	void UpdateGeneration(); //	Iterative step for generation
	void UpdateSelection(int mouseX, int mouseY, bool leftMouseClicked);  // Update selection process

	void UpdateMaze(int mouseX, int mouseY, bool leftMouseClicked);
	void DrawMaze(unsigned int shaderProgram);
	void DrawCell(unsigned int shaderProgram, float r, float g, float b, Utils::Cell cell);
	void PrintMaze(); // For debugging purposes, It prints the maze to console

	bool IsGenerationComplete() const { return generationComplete; }
	bool IsSolvingComplete() const { return solvingComplete; }
	bool IsSelectionComplete() const { return selectionComplete; }

private:
	void InitializeGrid();
	void CleanupGrid();

private:
	void GenerateStep(Utils::Cell cell); // A single recursive step in maze generation

private:
	float* GenerateGridVertices(float gridW, float gridH);

private:
	const int width;
    const int height;

	bool** grid = nullptr;

private:
	/* Variables to generate the maze */
	bool generating = false;
	bool generationComplete = false;

	Utils::Cell startCell; // Starting point for maze generation
	Utils::Cell currentCell; // Current cell being processed
	std::vector<Utils::Cell> generationStack; // Stack for iterative generation

private:
	/* Variables to solve the maze */
	bool solving = false;
	bool solvingComplete = false;

private:
	/* Those are user selected */
	Utils::Cell solveStartCell; // Starting point for maze solving
	bool hasSolveStartCell = false;

	Utils::Cell solveEndCell;   // Ending point for maze solving
	bool hasSolveEndCell = false;

	Utils::Cell pointedCell; // Currently pointed cell by mouse
	bool pointing = false;

	bool selectingCells = false; // Flag to indicate if we are in cell selection mode
	bool selectionComplete = false;
	Utils::SelectionPhase selectionPhase = Utils::SelectionPhase::SelectingStart;

private:
	int cellHalfSize = 10; // Size of each cell in pixels

private:
	unsigned int mazeCellBuffer = 0; // OpenGL buffer for maze cells
	unsigned int mazeCellVAO = 0;    // OpenGL Vertex Array Object for maze cells
};

