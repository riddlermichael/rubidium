#include "Error.hpp"

using namespace rb::core;
using namespace rb::core::error;

void Error::printTo(std::ostream& os) const {
	os << *this;
	/*
	int level = 1;
	Error const* cause = this->cause();
	while (cause) {
		os << "\n";
		for (int i = 0; i < level; ++i) {
			os << "  ";
		}
		os << "caused by " << *cause;
		cause = cause->cause();
		++level;
	}
	*/
	os << std::endl;
}

void Error::printMessage(std::ostream& os) const {
	if (message()) {
		os << message();
	}
}

std::ostream& error::operator<<(std::ostream& os, Error const& error) {
	if (error.message()) {
		error.printMessage(os);
		os << " at ";
	}
	return os << error.location();
}
