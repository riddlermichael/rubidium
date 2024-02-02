#include "assert.hpp"

#include <iostream>
#include <stdexcept>

#include <rb/core/error/Error.hpp>

namespace rb::core {

RB_HIDDEN void terminateHandler() {
	std::exception_ptr const exceptionPtr = std::current_exception();
	try {
		if (exceptionPtr) {
			std::rethrow_exception(exceptionPtr);
		}
	} catch (Error const& error) {
		std::cerr << "Unhandled error:\n";
		error.printTo(std::cerr);
	} catch (std::exception const& exc) {
		std::cerr << "std::exception: " << exc.what() << "\n";
	} catch (...) {
		std::cerr << "Unknown exception\n";
	}
}

TerminateHandlerSetter::TerminateHandlerSetter() noexcept {
	std::set_terminate(terminateHandler);
}

void throwAssert(char const* msg, SourceLocation const& /*location*/) {
	throw std::logic_error(msg); // FIXME
}

} // namespace rb::core
