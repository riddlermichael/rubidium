#include "Error.hpp"

#include <rb/core/move.hpp>

using namespace rb::core;
using namespace rb::core::error;

Error::Error(SourceLocation const& location) noexcept
    : loc_(location) {
}

Error::Error(czstring msg, SourceLocation const& location) noexcept
    : msg_(msg)
    , loc_(location) {
}

Error& Error::withLocation(SourceLocation const& location) noexcept {
	loc_ = location;
	return *this;
}

Error& Error::withMessage(czstring msg) & noexcept {
	msg_ = msg;
	return *this;
}

Error&& Error::withMessage(czstring msg) && noexcept {
	msg_ = msg;
	return RB_MOVE(*this);
}

czstring Error::message() const noexcept {
	return msg_;
}

SourceLocation const& Error::location() const noexcept {
	return loc_;
}

Error const* Error::cause() const noexcept { // NOLINT(*-convert-member-functions-to-static)
	return nullptr;
}

void Error::printTo(std::ostream& os) const {
	os << *this;
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
