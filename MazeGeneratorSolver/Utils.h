#pragma once

namespace Utils {
	enum class Phase
	{
		Idle,
		Generation,
		Solving,
		Completed
	};

	inline Phase GetNextPhase(Phase currentPhase) {
		return static_cast<Phase>((static_cast<int>(currentPhase) + 1) % 4);
	}
}