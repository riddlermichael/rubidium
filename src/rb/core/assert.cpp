#include "assert.hpp"

#include <stdexcept>

void rb::core::throwAssert(char const* msg, SourceLocation /*location*/) {
	throw std::logic_error(msg); // FIXME
}
