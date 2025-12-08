#pragma once

#include <utility>

namespace Utils {
	enum class Phase
	{
		Idle,
		Generation,
		CellSelection,
		Solving,
		Completed
	};

	inline Phase GetNextPhase(Phase currentPhase) {
		return static_cast<Phase>((static_cast<int>(currentPhase) + 1) % 4);
	}

	inline std::pair<int, int> GetDirection(int index) {
		std::pair<int, int> Directions[] = {
			{ 0, -1 }, // Up
			{ 0, 1 },  // Down
			{ -1, 0 }, // Left
			{ 1, 0 }   // Right
		};
		return Directions[index];
	}	

	struct Cell {
		int x;
		int y;
	};

}