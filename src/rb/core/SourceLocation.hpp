#pragma once

#include <cstring>
#include <ostream>

#include <rb/core/builtins.hpp>
#include <rb/core/export.hpp>
#include <rb/core/features.hpp>
#include <rb/core/keywords.hpp>
#include <rb/core/types.hpp>

namespace rb::core {

class RB_EXPORT SourceLocation final {
public:
	// intentionally `constexpr` for using in `constexpr` context
	static constexpr SourceLocation here(
	    czstring file = RB_BUILTIN_FILE,
	    unsigned line = RB_BUILTIN_LINE,
	    czstring func = RB_BUILTIN_FUNCTION,
	    unsigned column = RB_BUILTIN_COLUMN) noexcept {
		SourceLocation location;
		location.file_ = file ? file : "";
		location.line_ = line;
		location.func_ = func ? func : "";
		location.column_ = column;
		return location;
	}

	static RB_CONSTEVAL SourceLocation current(
	    czstring file = RB_BUILTIN_FILE,
	    unsigned line = RB_BUILTIN_LINE,
	    czstring func = RB_BUILTIN_FUNCTION,
	    unsigned column = RB_BUILTIN_COLUMN) noexcept {
		return here(file, line, func, column);
	}

	constexpr SourceLocation() noexcept = default;

	bool operator==(SourceLocation rhs) const noexcept {
		return std::strcmp(file_, rhs.file_) == 0
		    && line_ == rhs.line_
#if RB_HAS(BUILTIN_COLUMN)
		    && column_ == rhs.column_
#endif
		    ;
	}

	bool operator!=(SourceLocation rhs) const noexcept {
		return !(*this == rhs);
	}

	constexpr czstring file() const noexcept {
		return file_;
	}

	constexpr unsigned line() const noexcept {
		return line_;
	}

	constexpr czstring func() const noexcept {
		return func_;
	}

	constexpr unsigned column() const noexcept {
		return column_;
	}

private:
	czstring file_ = "";
	czstring func_ = ""; // unnatural order here due to alignment
	unsigned line_ = 0;
	unsigned column_ = 0;
};

inline std::ostream& operator<<(std::ostream& os, SourceLocation loc) {
	os << loc.file() << ":" << loc.line();
#if RB_ENABLED(FULL_SOURCE_LOCATION_INFO)
	#if RB_HAS(BUILTIN_COLUMN) // NOLINT(*-redundant-preprocessor)
	os << ":" << loc.column();
    #endif
	czstring func = loc.func();
	if (func && func[0]) {
		os << " (" << func << ")";
	}
#endif
	return os;
}

} // namespace rb::core

#if RB_HAS(BUILTIN_SOURCE_LOCATION)
	#define RB_SOURCE_LOCATION rb::core::SourceLocation::current()
#else
	#define RB_SOURCE_LOCATION \
		rb::core::SourceLocation::current(__FILE__, __LINE__, RB_BUILTIN_FUNCSIG, RB_BUILTIN_COLUMN)
#endif

#define RB_SOURCE_LOCATION_DECL \
	rb::core::SourceLocation const& location = rb::core::SourceLocation::current()
