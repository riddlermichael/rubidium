#pragma once

#include <rb/time/Duration.hpp>

namespace rb::time {

class SteadyClock final {
public:
	static constexpr bool kIsSteady = true;

	static SteadyClock now() noexcept;

	constexpr SteadyClock() noexcept = default;

	constexpr Duration operator-(SteadyClock rhs) const noexcept;

	constexpr Duration since(SteadyClock rhs) const noexcept;

private:
	friend std::ostream& operator<<(std::ostream& os, SteadyClock clock);

	constexpr explicit SteadyClock(Duration rep) noexcept
	    : rep_{rep} {
	}

	Duration rep_;
};

constexpr Duration SteadyClock::operator-(SteadyClock rhs) const noexcept {
	return rep_ - rhs.rep_;
}

constexpr Duration SteadyClock::since(SteadyClock rhs) const noexcept {
	return rhs.rep_ - rep_;
}

std::ostream& operator<<(std::ostream& os, SteadyClock clock);

} // namespace rb::time
