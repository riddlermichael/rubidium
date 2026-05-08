#pragma once

namespace rb::interval {

struct RoundSaver {
	int roundMode;

	RoundSaver() noexcept;
	~RoundSaver();
};

} // namespace rb::interval
