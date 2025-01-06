#include "SystemTime.hpp"

#include <ctime>

using namespace rb::time;

SystemTime SystemTime::now() noexcept {
	std::timespec ts = {};
	clock_gettime(CLOCK_REALTIME, &ts);
	return from(ts);
}

SystemTime::Result SystemTime::elapsed() const noexcept {
	return now().since(*this);
}

std::ostream& rb::time::operator<<(std::ostream& os, SystemTime clock) {
	return os << "SystemTime{" << clock.rep_ << "}";
}
