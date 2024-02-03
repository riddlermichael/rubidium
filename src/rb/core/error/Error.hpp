#pragma once

#include <ostream>

#include <rb/core/SourceLocation.hpp>

namespace rb::core {
inline namespace error {

	class RB_EXPORT Error {
	public:
		/// Construct Error with empty message and specified @p location.
		explicit Error(RB_SOURCE_LOCATION_DECL) noexcept;

		/// Construct Error with @p msg and @p location.
		explicit Error(LiteralString msg, RB_SOURCE_LOCATION_DECL);

		Error(Error const&) noexcept = default;
		Error(Error&&) noexcept = default;
		virtual ~Error() = default;

		Error& operator=(Error const& rhs) noexcept = default;
		Error& operator=(Error&& rhs) noexcept = default;

		/// Initializes the source location of this error to the specified value.
		/// @return @c *this.
		Error& withLocation(RB_SOURCE_LOCATION_DECL) noexcept;

		/// @return string description of @c this.
		LiteralString message() const noexcept;

		/// @return location of @c this in source code.
		SourceLocation const& location() const noexcept;

		/// @return the cause of @c this or `nullptr` if the cause is nonexistent or unknown.
		Error const* cause() const noexcept;

		/// Print @c this with all its causes to stream @p os.
		void printTo(std::ostream& os) const;

	private:
		LiteralString msg_ = nullptr;
		SourceLocation loc_;
	};

	std::ostream& operator<<(std::ostream& os, Error const& error);

} // namespace error
} // namespace rb::core