#pragma once

#include <utility>
#include <vector>
#include <iostream>

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

	enum class SelectionPhase
	{
		SelectingStart,
		SelectingEnd
	};

	typedef std::pair<int, int> Direction; //Just more readable :)
	inline Direction GetDirection(int index) {
		Direction Directions[] = {
			{ 0, -1 }, // Up
			{ 0, 1 },  // Down
			{ -1, 0 }, // Left
			{ 1, 0 }   // Right
		};
		return Directions[index];
	}
	inline Direction GetInvertedDirection(Direction& direction) {
		return Direction{ direction.first * -1, direction.second * -1 };
	}

	struct Cell {
		int x;
		int y;

		inline Cell operator+=(const Cell& current) {
			Cell cell;
			cell.x = x + current.x;
			cell.y = y + current.y;
			return cell;
		}
		inline bool operator==(const Cell& other) const {
			return x == other.x && y == other.y;
		}
		inline bool operator!=(const Cell& other) const {
			return !(*this == other);
		}
	};

	struct Entrance {
		struct Cell cell;
		int passCount = 1;

		Entrance(Cell cell) : cell(cell) {}
	};

	inline int GetPassCount(const std::vector<Entrance>& entrances, Cell cell) {
		for (const auto& entrance : entrances) {
			if (entrance.cell == cell) {
				return entrance.passCount;
			}
		}
		return 0;
	}

	inline void PassOnEntrance(std::vector<Entrance>& entrances, Cell cell) {
		for (auto& entrance : entrances) {
			if (entrance.cell == cell) {
				entrance.passCount++;
				return;
			}
		}
		entrances.push_back(Entrance(cell));

		std::cout << "Passed on entrance at (" << cell.x << ", " << cell.y << "), total passes: " << GetPassCount(entrances, cell) << "\n";
	}

	inline bool IsPassedEntrance(const std::vector<Entrance>& entrances, Cell cell) {
		for (const auto& entrance : entrances) {
			if (entrance.cell == cell) {
				return true;
			}
		}
		return false;
	}

}