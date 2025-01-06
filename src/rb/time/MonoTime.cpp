#include "MonoTime.hpp"

#include <ctime>

using namespace rb::time;

MonoTime MonoTime::now() noexcept {
	std::timespec ts = {};
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return MonoTime{Duration::from(ts)};
}

Duration MonoTime::elapsed() const noexcept {
	return now() - *this;
}

std::ostream& rb::time::operator<<(std::ostream& os, MonoTime clock) {
	return os << "MonoTime{" << clock.rep_ << "}";
}
