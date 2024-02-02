#include "assert.hpp"

#include <stdexcept>

void rb::core::throwAssert(char const* msg, SourceLocation const& /*location*/) {
	throw std::logic_error(msg); // FIXME
}
