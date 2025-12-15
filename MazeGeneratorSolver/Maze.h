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
#include <algorithm>

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
	void CompleteMaze();

	void UpdateGeneration(); //	Iterative step for generation
	void UpdateSelection(int mouseX, int mouseY, float cameraX, float cameraY, float cameraZoom, bool leftMouseClicked);  // Update selection process
	void UpdateSolving();    // Iterative step for solving
	void UpdateCompletion(); // Iteratice step for completion

	void UpdateMaze(int mouseX, int mouseY, float cameraX, float cameraY, float cameraZoom, bool leftMouseClicked);
	void DrawMaze(unsigned int shaderProgram, int cameraX, int cameraY);
	void DrawCell(unsigned int shaderProgram, int cameraX, int cameraY, float r, float g, float b, std::shared_ptr<Utils::Cell> cell);
	void PrintMaze(); // For debugging purposes, It prints the maze to console

	bool IsGenerationComplete() const { return generationComplete; }
	bool IsSolvingComplete() const { return solvingComplete; }
	bool IsSelectionComplete() const { return selectionComplete; }
	bool IsCompletionComplete() const { return completionComplete; }
private:
	void InitializeGrid();
	void CleanupGrid();

private:
	void GenerateStep(std::shared_ptr<Utils::Cell> cell); // A single recursive step in maze generation

private:
	void FixNeighborJunctions();
	
private:
	float* GenerateGridVertices(float gridW, float gridH);

private:
	const int width;
    const int height;

	std::vector<std::vector<std::shared_ptr<Utils::Cell>>> grid; //Hold all grid
	std::vector<std::shared_ptr<Utils::Cell>> junctions; //Hold all junctions

private:
	/* Variables to generate the maze */
	bool generating = false;
	bool generationComplete = false;

	std::shared_ptr<Utils::Cell> startCell; // Starting point for maze generation
	std::shared_ptr<Utils::Cell> currentCell; // Current cell being processed
	std::vector<std::shared_ptr<Utils::Cell>> generationStack; // Stack for iterative generation

private:
	/* Variables to solve the maze */
	bool solving = false;
	bool solvingComplete = false;

	std::shared_ptr<Utils::Cell> currentSolveCell; // Current cell being processed in solving
	Utils::Direction currentDirection;

	std::vector<std::shared_ptr<Utils::Entrance>> passedEntrances;

private:
	/* Variables to complete the maze */
	bool completing = false;
	bool completionComplete = false;

	bool inJunction = true; //If currentCompleteCell is in a junction

	Utils::Direction startDirection;
	Utils::Direction currentCompletionDirection;

	std::shared_ptr<Utils::Cell> currentCompleteCell;
	
	std::vector<std::shared_ptr<Utils::Cell>> solvePath;
	std::vector<std::shared_ptr<Utils::Entrance>> oncePassedEntrances;

private:
	/* Those are user selected */
	std::shared_ptr<Utils::Cell> solveStartCell; // Starting point for maze solving
	bool hasSolveStartCell = false;

	std::shared_ptr<Utils::Cell> solveEndCell;   // Ending point for maze solving
	bool hasSolveEndCell = false;

	std::shared_ptr<Utils::Cell> pointedCell; // Currently pointed cell by mouse
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

