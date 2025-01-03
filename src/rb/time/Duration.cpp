#include "Duration.hpp"

using namespace rb::time;

std::ostream& rb::time::operator<<(std::ostream& os, Duration dur) {
	if (dur.isNaN()) {
		return os << "NaN";
	}

	if (dur.isInf()) {
		return os << (dur.isNegative() ? "-inf" : "+inf");
	}

	if (dur.isZero()) {
		return os << "0s";
	}

	if (dur == Duration::lowest()) { // avoid negation
		return os << "-2562047788015215h30m8s";
	}

	if (dur.isNegative()) {
		dur = -dur;
		os << "-";
	}

	if (dur >= Duration::kSecond) {
		{
			auto const [quo, rem] = *dur.quorem(Duration::kHour);
			if (quo) {
				os << quo << "h";
			}
			dur = rem;
		}
		{
			auto const [quo, rem] = *dur.quorem(Duration::kMinute);
			if (quo) {
				os << quo << "m";
			}
			dur = rem;
		}
		auto t = dur.div(Duration::kSecond);
		return os << dur.div(Duration::kSecond) << "s";
	}

	if (dur < Duration::kMicrosecond) {
		return os << dur.div(Duration::kNanosecond) << "ns";
	}

	if (dur < Duration::kMillisecond) {
		return os << dur.div(Duration::kMicrosecond) << "us";
	}

	return os << dur.div(Duration::kMillisecond) << "ms";
}
