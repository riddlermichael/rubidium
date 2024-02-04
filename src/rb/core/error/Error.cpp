#include "Error.hpp"

using namespace rb::core;
using namespace rb::core::error;

Error::Error(SourceLocation const& location) noexcept
    : loc_(location) {
}

Error::Error(LiteralString msg, SourceLocation const& location) noexcept
    : msg_(msg)
    , loc_(location) {
}

Error& Error::withLocation(SourceLocation const& location) noexcept {
	loc_ = location;
	return *this;
}

LiteralString Error::message() const noexcept {
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

std::ostream& error::operator<<(std::ostream& os, Error const& error) {
	if (error.message()) {
		os << error.message() << " at ";
	}
	return os << error.location();
}
