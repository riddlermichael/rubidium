#pragma once

#include <rb/core/move.hpp>
#include <rb/core/SourceLocation.hpp>

namespace rb::core {
inline namespace error {

	class RB_EXPORT Error {
	public:
		/// Construct Error with an empty message and specified @p location.
		constexpr explicit Error(RB_SOURCE_LOCATION_DECL) noexcept
		    : loc_{location} {
		}

		/// Construct Error with @p msg and @p location.
		constexpr explicit Error(czstring msg, RB_SOURCE_LOCATION_DECL) noexcept
		    : msg_{msg}
		    , loc_{location} {
		}

		constexpr Error(Error const&) noexcept = default;
		constexpr Error(Error&&) noexcept = default;
		virtual ~Error() = default;

		Error& operator=(Error const&) noexcept = default;
		Error& operator=(Error&& rhs) noexcept = default;

		/// Initializes the source location of this error to the specified value.
		/// @return @c *this.
		constexpr Error& withLocation(RB_SOURCE_LOCATION_DECL) noexcept {
			loc_ = location;
			return *this;
		}

		/// Initializes the description of this error to the specified value.
		/// @return @c *this.
		constexpr Error& withMessage(czstring msg) & noexcept {
			msg_ = msg;
			return *this;
		}

		/// Initializes the description of this error to the specified value.
		/// @return @c *this.
		constexpr Error&& withMessage(czstring msg) && noexcept {
			msg_ = msg;
			return RB_MOVE(*this);
		}

		/// @return string description of @c this.
		constexpr czstring message() const noexcept {
			return msg_;
		}

		/// @return location of @c this in source code.
		constexpr SourceLocation const& location() const noexcept {
			return loc_;
		}

		/// Print @c this with all its causes to stream @p os.
		void printTo(std::ostream& os) const;

	protected:
		virtual void printMessage(std::ostream& os) const;

	private:
		friend std::ostream& operator<<(std::ostream& os, Error const& error);

		czstring msg_ = nullptr;
		SourceLocation loc_;
	};

	std::ostream& operator<<(std::ostream& os, Error const& error);

} // namespace error
} // namespace rb::core
