#include "Duration.hpp"

using namespace rb::time;

std::ostream& rb::time::operator<<(std::ostream& os, Duration dur) {
	if (dur.isInf()) {
		return os << (dur.isNegative() ? "-inf" : "+inf");
	}
	return os << "Duration{seconds: " << dur.seconds_ << ", ticks: " << dur.ticks_ << "}";
}
