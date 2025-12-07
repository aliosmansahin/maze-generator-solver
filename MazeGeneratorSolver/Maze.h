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
	void UpdateGeneration(); //	Iterative step for generation

	void SolveMaze();

	void UpdateMaze();
	void DrawMaze(unsigned int shaderProgram);
	void DrawCell(unsigned int shaderProgram, unsigned int vertexArrayToDraw, Utils::Cell cell);
	void PrintMaze(); // For debugging purposes, It prints the maze to console

	bool IsGenerationComplete() const { return generationComplete; }

private:
	void InitializeGrid();
	void CleanupGrid();

private:
	void GenerateStep(Utils::Cell cell); // A single recursive step in maze generation

private:
	float* GenerateGridVertices(float gridW, float gridH, float r, float g, float b);

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

	int cellHalfSize = 10; // Size of each cell in pixels

private:
	unsigned int mazeCellBuffer = 0; // OpenGL buffer for maze cells
	unsigned int mazeCellVAO = 0;    // OpenGL Vertex Array Object for maze cells

	unsigned int currentCellBuffer = 0; // OpenGL buffer for current cell
	unsigned int currentCellVAO = 0;    // OpenGL Vertex Array Object for current cell
};

