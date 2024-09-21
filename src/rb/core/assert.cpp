#include "assert.hpp"

#include <iostream>

#include <rb/core/error/Error.hpp>

namespace rb::core {

RB_HIDDEN void terminateHandler() {
	std::exception_ptr const exceptionPtr = std::current_exception();
	try {
		if (exceptionPtr) {
			std::rethrow_exception(exceptionPtr);
		}
	} catch (AssertError const& error) {
		std::cerr << "Assertion failed: ";
		error.printTo(std::cerr);
	} catch (Error const& error) {
		std::cerr << "Unhandled error: ";
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

void throwAssert(czstring msg, SourceLocation const& location) {
	throw AssertError(msg, location);
}

} // namespace rb::core
