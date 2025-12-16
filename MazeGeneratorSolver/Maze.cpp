#include "Maze.h"

void Maze::GenerateMaze(float& cameraXAfterSet, float& cameraYAfterSet, float& cameraZoomAfterSet)
{
	/* Initialize parameters */
	generationComplete = false;
	generating = true;

	SetCameraToFitMazeIntoScreen(cameraXAfterSet, cameraYAfterSet, cameraZoomAfterSet);

	/* Generate or use a maze seed */
#ifdef MAZE_SEED
	srand(MAZE_SEED);

	std::cout << "Using pre-entered maze seed: " << MAZE_SEED << std::endl;
#else
	/* Generate 10 rigid maze seed */
	srand(static_cast<unsigned int>(time(nullptr)));

	std::string mazeSeedStr = "";

	for (int i = 0; i < MAZE_SEED_RIGID_COUNT; ++i) {
		int mazeRigid = rand() % MAZE_SEED_RIGID_COUNT;

		mazeSeedStr += std::to_string(mazeRigid);
	}

	unsigned int mazeSeed = (unsigned int)std::stoul(mazeSeedStr);

	srand(mazeSeed);

	std::cout << "Maze seed: " << mazeSeed << std::endl;
#endif

	do {
		startCell = GetCellFromXY(rand() % height, rand() % width);

	} while (startCell->x % 2 == 0 || startCell->y % 2 == 0); // Ensure start cell is odd indexed

	//GenerateStep(startCell); // This is recursive and may block the main thread
	// Instead, we will implement iterative generation in UpdateGeneration
	// by pushing the start cell onto the stack

	startCell->isWall = false; // Mark start cell as part of the maze
	generationStack.push_back(startCell);

	std::cout << "Maze Generation Started from (" << startCell->x << ", " << startCell->y << ")\n";
}

void Maze::StartSelection()
{
	selectingCells = true;
}

/* UNUSED FUNCTION */
void Maze::GenerateStep(std::shared_ptr<Utils::Cell> cell)
{
	grid[cell->y][cell->x]->isWall = false; // Mark cell as part of the maze

	for(int i = 0; i < 4; ++i) {
		int nextX = cell->x + Utils::GetDirection(i).first * 2;
		int nextY = cell->y + Utils::GetDirection(i).second * 2;

		if (nextX >= 0 && nextX < width && nextY >= 0 && nextY < height && grid[nextY][nextX]->isWall) {
			GenerateStep(grid[nextY][nextX]);
		}
	}
}

void Maze::FixNeighborJunctions()
{
	for (int i = 0; i < junctions.size(); ++i) {
		const auto j0 = junctions[i];

		for (int j = i + 1; j < junctions.size(); ++j) {
			const auto j1 = junctions[j];

			int distX = j0->x - j1->x;
			int distY = j0->y - j1->y;

			if (abs(distX) <= 2 && abs(distY) <= 2) {
				int entX = (j0->x + j1->x) / 2;
				int entY = (j0->y + j1->y) / 2;

				auto entrance = std::find_if(passedEntrances.begin(), passedEntrances.end(), [&entX, &entY](const std::shared_ptr<Utils::Entrance>& entrance) {
					return entrance->cell->x == entX && entrance->cell->y == entY;
				});

				if (entrance == passedEntrances.end())
					continue;

				if (entrance->get()->cell->isWall)
					continue;

				if (entrance->get()->passCount == 2)
					entrance->get()->passCount = 1;
			}
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

std::vector<Utils::Direction> Maze::GetMovableDirections(std::shared_ptr<Utils::Cell> cell)
{
	std::vector<Utils::Direction> movableDirections;

	for (int i = 0; i < 4; ++i) {
		std::shared_ptr<Utils::Cell> nextCell = GetCellTowardsDirection(cell, Utils::GetDirection(i));

		if (!nextCell)
			continue;
		
		if (nextCell->isWall)
			continue;

		movableDirections.push_back(Utils::GetDirection(i));
	}

	return movableDirections;
}

/*
PURPOSE: Set camera position and zoom to fit maze into screen without any navigation process
*/
void Maze::SetCameraToFitMazeIntoScreen(float& cameraXAfterSet, float& cameraYAfterSet, float& cameraZoomAfterSet)
{
	float mazeWorldWidth = cellHalfSize * 2.0f * width;
	float mazeWorldHeight = cellHalfSize * 2.0f * height;

	float zoomX = WINDOW_WIDTH / mazeWorldWidth * 0.8f;
	float zoomY = WINDOW_HEIGHT / mazeWorldHeight * 0.8f;

	cameraZoomAfterSet = (zoomX < zoomY) ? zoomX : zoomY;

	float mazeCenterX = mazeWorldWidth / 2.0f - cellHalfSize;
	float mazeCenterY = mazeWorldHeight / 2.0f - cellHalfSize;

	cameraXAfterSet = mazeCenterX;
	cameraYAfterSet = mazeCenterY;
}

std::shared_ptr<Utils::Cell> Maze::GetCellFromXY(int x, int y)
{
	if (x >= 0 && x <= width - 1 && y >= 0 && y <= height - 1) {
		return grid[y][x];
	}
	else
		return nullptr;
}

std::shared_ptr<Utils::Cell> Maze::GetCellTowardsDirection(std::shared_ptr<Utils::Cell> cell, Utils::Direction direction, int multiply)
{
	return GetCellFromXY(cell->x + direction.first * multiply, cell->y + direction.second * multiply);
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
		std::vector<std::shared_ptr<Utils::Cell>> unvisitedNeighbors;
		for (int i = 0; i < 4; ++i) {
			std::shared_ptr<Utils::Cell> nextCell = GetCellTowardsDirection(currentCell, Utils::GetDirection(i), 2);

			if (!nextCell)
				continue;

			if (!nextCell->isWall)
				continue;

			unvisitedNeighbors.push_back(nextCell);
		}

		if (!unvisitedNeighbors.empty()) {
			generationStack.push_back(currentCell); // Push current cell back to stack
			
			int selectedNeighborIndex = rand() % unvisitedNeighbors.size();
			std::shared_ptr<Utils::Cell> selectedNeighbor = unvisitedNeighbors[selectedNeighborIndex];

			// Remove wall between current cell and selected neighbor
			int wallX = (currentCell->x + selectedNeighbor->x) / 2;
			int wallY = (currentCell->y + selectedNeighbor->y) / 2;
			GetCellFromXY(wallX, wallY)->isWall = false;

			selectedNeighbor->isWall = false;
			generationStack.push_back(selectedNeighbor);
		}
	}
	else {
		generationComplete = true;
		generating = false;

		std::cout << "Maze Generation Complete!\n";

#ifdef DEBUG_PRINT_MAZE_INTO_CONSOLE
		PrintMaze();
#endif
	}
}

void Maze::UpdateSelection(int mouseX, int mouseY, float cameraX, float cameraY, float cameraZoom, bool leftMouseClicked)
{
	/* Convert mouse coordinates into our coordinate system */
	float worldMouseX = mouseX / cameraZoom + cameraX;
	float worldMouseY = mouseY / cameraZoom + cameraY;

	/* Use world mouse position to find pointed cell */
	int cellX = (int)((worldMouseX + cellHalfSize) / (cellHalfSize * 2.0f));
	int cellY = (int)((worldMouseY + cellHalfSize) / (cellHalfSize * 2.0f));

	bool mouseInsideMaze = cellX >= 0 && cellX < width && cellY >= 0 && cellY < height;

	/* Clamp cell indices to be within grid bounds */
	if (cellX < 0) cellX = 0;
	if (cellX >= width) cellX = width - 1;

	if (cellY < 0) cellY = 0;
	if (cellY >= height) cellY = height - 1;

	/* Do some checks to avoid point to the walls on inside of the map */
	bool isWall = grid[cellY][cellX]->isWall;
	bool isBound = cellX == 0 || cellX == width - 1 || cellY == 0 || cellY == height - 1;

	bool hasNeighborWall = false;

	if (cellX == 0 && grid[cellY][cellX + 1]->isWall) hasNeighborWall = true; // Left
	if (cellX == width - 1 && grid[cellY][cellX - 1]->isWall) hasNeighborWall = true; // Right
	if (cellY == 0 && grid[cellY + 1][cellX]->isWall) hasNeighborWall = true; // Down
	if (cellY == height - 1 && grid[cellY - 1][cellX]->isWall) hasNeighborWall = true; // Up

	/* Set pointed cell if there is no issue from the checks */
	if((!isWall || (isBound && !hasNeighborWall)) && mouseInsideMaze) {
		pointedCell = grid[cellY][cellX]; //Checks have already been done before
		pointing = true;
	}
	else {
		pointing = false;
	}

	/* Cell selections to choose start and end points */
	if(leftMouseClicked && pointing && !selectionComplete) {
		if(selectionPhase == Utils::SelectionPhase::SelectingStart) {
			solveStartCell = pointedCell;

			if(solveStartCell->isWall)
				solveStartCell->isWall = false;
			
			hasSolveStartCell = true;
			
			selectionPhase = Utils::SelectionPhase::SelectingEnd;
			std::cout << "Start Cell Selected at (" << solveStartCell->x << ", " << solveStartCell->y << ")\n";
		}
		else if(selectionPhase == Utils::SelectionPhase::SelectingEnd) {
			solveEndCell = pointedCell;

			if (solveEndCell->isWall)
				solveEndCell->isWall = false;

			hasSolveEndCell = true;

			selectionComplete = true;
			selectingCells = false;

			std::cout << "End Cell Selected at (" << solveEndCell->x << ", " << solveEndCell->y << ")\n";
		}
	}
}

void Maze::UpdateSolving()
{
	bool movable = true;

	/* Get movable direction of the current cell */
	std::vector<Utils::Direction> movableDirections = GetMovableDirections(currentSolveCell);

	Utils::Direction nextDirection{};

	/* Decide the direction we will move */
	if (movableDirections.size() <= 2) {
		/* We are in a passage */

		if (movableDirections.size() == 2) {
			/* Only go forward */
			std::erase(movableDirections, Utils::GetInvertedDirection(currentDirection));
		}

		nextDirection = movableDirections[0];
	}
	else {
		/* We are in a junction */
		
		/* Store this junction cell */
		auto junction = std::find(junctions.begin(), junctions.end(), currentSolveCell);

		if(junction == junctions.end())
			junctions.push_back(currentSolveCell);

		/* Pass the previous entrance */
		std::shared_ptr<Utils::Cell> previousCell = GetCellTowardsDirection(currentSolveCell, Utils::GetInvertedDirection(currentDirection));

		Utils::PassOnEntrance(passedEntrances, previousCell);

		/* Get passed entrances */
		std::vector<Utils::Direction> unpassedDirections;

		/* Set those variables */
		bool isOnlyPassedPrevious = true;
		bool isAllEntrancesPassed = true;

		for (const auto& direction : movableDirections) {
			std::shared_ptr<Utils::Cell> nextCell = GetCellTowardsDirection(currentSolveCell, direction);

			if (!Utils::IsPassedEntrance(passedEntrances, nextCell)) {
				unpassedDirections.push_back(direction);

				isAllEntrancesPassed = false;
			}
			else {
				if (nextCell != previousCell)
					isOnlyPassedPrevious = false;
			}
		}
		if (isOnlyPassedPrevious) {
			/* Select an random unpassed entrance */
			if (!unpassedDirections.empty()) {
				int selectedIndex = rand() % unpassedDirections.size();
				nextDirection = unpassedDirections[selectedIndex];
			}
		}
		else if (isAllEntrancesPassed && Utils::GetPassCount(passedEntrances, previousCell) < 2) {
			/* All entrances are passed, go back */
			Utils::Direction invDirection = Utils::GetInvertedDirection(currentDirection);
			nextDirection = invDirection;
		}
		else {
			/* Select any entrance with the fewest passed */
			int minPassCount = INT_MAX;
			std::vector<Utils::Direction> leastPassedDirections;

			for (const auto& direction : movableDirections) {
				std::shared_ptr<Utils::Cell> nextCell = GetCellTowardsDirection(currentSolveCell, direction);

				int passCount = Utils::GetPassCount(passedEntrances, nextCell);
				
				if (passCount < minPassCount) {
					minPassCount = passCount;
					leastPassedDirections.clear();
					leastPassedDirections.push_back(direction);
				}
				else if (passCount == minPassCount) {
					leastPassedDirections.push_back(direction);
				}
			}
			/* Select a random direction from least passed directions */
			if (!leastPassedDirections.empty()) {
				int selectedIndex = rand() % leastPassedDirections.size();
				nextDirection = leastPassedDirections[selectedIndex];
			}
		}

		std::shared_ptr<Utils::Cell> nextCell = GetCellTowardsDirection(currentSolveCell, nextDirection);

		Utils::PassOnEntrance(passedEntrances, nextCell);
	}

	currentDirection = nextDirection;

	/* Mode the current cell */
	if (movable) {
		/* To prevent wrong direction selection in completion phase */
		if (currentSolveCell == solveStartCell)
			startDirection = currentDirection;

		currentSolveCell = GetCellTowardsDirection(currentSolveCell, currentDirection);
	}

	/* If we reach the finish */
	if (currentSolveCell == solveEndCell) {
		FixNeighborJunctions();

		solving = false;
		solvingComplete = true;
	}
}

void Maze::UpdateCompletion()
{
	/* Get movable direction of the current cell */
	std::vector<Utils::Direction> movableDirections = GetMovableDirections(currentCompleteCell);

	Utils::Direction nextDirection{};

	/* Decide the direction we will move */
	if (movableDirections.size() <= 2) {
		/* We are in a passage */

		if (movableDirections.size() == 2) {
			/* Only go forward */
			std::erase(movableDirections, Utils::GetInvertedDirection(currentCompletionDirection));
		}

		nextDirection = movableDirections[0];
	}
	else {
		/*	
			We are in a junction
			We will move on the once passed entrance
		*/

		/* Remove our backward to make our once passed entrances only one */
		std::erase(movableDirections, Utils::GetInvertedDirection(currentCompletionDirection));

		for (const auto& direction : movableDirections) {
			/* Get next cell in the direcion */
			std::shared_ptr<Utils::Cell> nextCell = GetCellTowardsDirection(currentCompleteCell, direction);
			if (Utils::IsOncePassedEntrance(passedEntrances, nextCell)) {
				/* We will move on this direction */
				nextDirection = direction;

				break;
			}
		}
	}

	currentCompletionDirection = nextDirection;

	currentCompleteCell = GetCellTowardsDirection(currentCompleteCell, currentCompletionDirection);

	/* We have reached to end */
	if (currentCompleteCell == solveEndCell) {
		completing = false;
		completionComplete = true;

		std::cout << "Displayed solve path" << std::endl;
		return;
	}

	solvePath.push_back(currentCompleteCell);
}

void Maze::SolveMaze()
{
	solving = true;
	solvingComplete = false;

	/* Start to use Tremaux's algorithm */

	std::cout << "Maze Solving Started from (" << solveStartCell->x << ", " << solveStartCell->y << ") to (" << solveEndCell->x << ", " << solveEndCell->y << ")\n";

	currentSolveCell = solveStartCell;
	passedEntrances.clear();
}

void Maze::CompleteMaze()
{
	completing = true;
	completionComplete = false;
	solvingComplete = false;

	std::cout << "Maze Completion Started from (" << solveStartCell->x << ", " << solveStartCell->y << ") to (" << solveEndCell->x << ", " << solveEndCell->y << ")\n";

	/* Start solve path */
	currentCompletionDirection = startDirection;
	currentCompleteCell = solveStartCell;
}

void Maze::UpdateMaze(int mouseX, int mouseY, float cameraX, float cameraY, float cameraZoom, bool leftMouseClicked)
{
	if(generating && !generationComplete) {
		UpdateGeneration();
	}
	if (selectingCells) {
		UpdateSelection(mouseX, mouseY, cameraX, cameraY, cameraZoom, leftMouseClicked);
	}
	if(solving && !solvingComplete) {
		UpdateSolving();
	}
	if (completing && !completionComplete) {
		UpdateCompletion();
	}
}

void Maze::DrawMaze(unsigned int shaderProgram, float cameraX, float cameraY)
{
	for(int y = 0; y < height; ++y) {
		for(int x = 0; x < width; ++x) {
			std::shared_ptr<Utils::Cell> cell = GetCellFromXY(x, y);
			if(cell->isWall) {
				DrawCell(shaderProgram, cameraX, cameraY, 1.0f, 1.0f, 1.0f, cell);
			}
		}
	}

	if(!generationStack.empty())
		DrawCell(shaderProgram, cameraX, cameraY, 0.0f, 1.0f, 0.0f, generationStack.back());

	if(selectingCells && pointing)
		DrawCell(shaderProgram, cameraX, cameraY, 1.0f, 0.0f, 0.0f, pointedCell);

	if (solving) {
		for (const auto& passedEntrance : passedEntrances) {
			if (passedEntrance->passCount == 1)
				DrawCell(shaderProgram, cameraX, cameraY, 0.0f, 1.0f, 1.0f, passedEntrance->cell);
			else if (passedEntrance->passCount >= 2)
				DrawCell(shaderProgram, cameraX, cameraY, 0.5f, 0.5f, 0.5f, passedEntrance->cell);
		}
	}

	for (const auto& solvePathCell : solvePath) {
		DrawCell(shaderProgram, cameraX, cameraY, 0.0f, 0.7f, 0.7f, solvePathCell);
	}

	if(hasSolveStartCell)
		DrawCell(shaderProgram, cameraX, cameraY, 0.0f, 0.0f, 1.0f, solveStartCell);

	if (hasSolveEndCell)
		DrawCell(shaderProgram, cameraX, cameraY, 1.0f, 1.0f, 0.0f, solveEndCell);

	if (completing)
		DrawCell(shaderProgram, cameraX, cameraY, 0.5f, 0.0f, 0.0f, currentCompleteCell);

	if (solving)
		DrawCell(shaderProgram, cameraX, cameraY, 1.0f, 0.0f, 1.0f, currentSolveCell);
}

/*
PURPOSE: Draws a single cell at given position with specified color.
	This function sends the color of the cell to the fragment shader
	to avoid creating multiple VAOs for different colors.
*/
void Maze::DrawCell(unsigned int shaderProgram, float cameraX, float cameraY, float r, float g, float b, std::shared_ptr<Utils::Cell> cell)
{
	/* Send translation matrix */
	int translateX = cell->x * cellHalfSize * 2;
	int translateY = cell->y * cellHalfSize * 2;
	float translationMatrix[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		(float)(translateX - cameraX), (float)(translateY - cameraY), 0, 1
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
			std::cout << (grid[i][j]->isWall ? "##" : "  ");
		}
		std::cout << std::endl;
	}
}

void Maze::InitializeGrid()
{
	/* Setup the variables */
	generating = false;
	generationComplete = false;
	generationStack.clear();
	solving = false;
	solvingComplete = false;
	pointing = false;
	selectingCells = false;
	selectionComplete = false;
	selectionPhase = Utils::SelectionPhase::SelectingStart;
	hasSolveStartCell = false;
	hasSolveEndCell = false;
	passedEntrances.clear();
	solvePath.clear();
	completing = false;
	completionComplete = false;
	
	/* Make sure the maze size is odd */
	if (width % 2 == 0)
		width--;

	if (height % 2 == 0)
		height--;

	/* Allocate memory for grid */
	grid.resize(height);

	for (int y = 0; y < height; ++y) {
		grid[y].resize(width);

		for (int x = 0; x < width; ++x) {
			grid[y][x] = std::make_shared<Utils::Cell>();

			grid[y][x]->x = x;
			grid[y][x]->y = y;
		}
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
	grid.clear();

	/* Deallocate memory for grid */
	glDeleteBuffers(1, &mazeCellBuffer);
	glDeleteVertexArrays(1, &mazeCellVAO);
}
