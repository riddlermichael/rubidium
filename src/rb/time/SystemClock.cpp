#include "SystemClock.hpp"

#include <ctime>

using namespace rb::time;

SystemClock SystemClock::now() noexcept {
	std::timespec ts = {};
	clock_gettime(CLOCK_REALTIME, &ts);
	return from(ts);
}

std::ostream& rb::time::operator<<(std::ostream& os, SystemClock clock) {
	return os << "SystemClock{" << clock.rep_ << "}";
}
