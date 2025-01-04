#include "SteadyClock.hpp"

#include <ctime>

using namespace rb::time;

SteadyClock SteadyClock::now() noexcept {
	std::timespec ts = {};
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return SteadyClock{Duration::from(ts)};
}

std::ostream& rb::time::operator<<(std::ostream& os, SteadyClock clock) {
	return os << "SteadyClock{" << clock.rep_ << "}";
}
