#pragma once

#include <ostream>

#include <rb/core/Option.hpp>
#include <rb/core/Span.hpp>
#include <rb/fmt/charconv.hpp>
#include <rb/rb_version.hpp>

namespace rb::core {

/// A lite and constexpr version according to [SemVer](https://semver.org) specification,
/// without pre-release and build components.
class Version final {
public:
	enum Component {
		kMajor,
		kMinor,
		kPatch,
	};

	static constexpr Option<Version> from(Span<char const> s) noexcept {
		char const* first = s.data();
		char const* last = s.data() + s.size();

		u32 components[3] = {};
		for (int c = 0; c < 3; ++c) {
			auto result = fmt::fromChars<u32>(first, last, &first);
			if (!result) {
				return kNone;
			}

			components[c] = *result; // NOLINT(*-pro-bounds-constant-array-index)
			if (first == last) {
				return Version{components[0], components[1], components[2]};
			}

			if (*first && *first != '.') {
				return kNone;
			}

			if (c < 2) {
				++first;
			}
		}
		if (*first && first != last) {
			return kNone;
		}

		return Version{components[0], components[1], components[2]};
	}

	static constexpr Version unpack(u32 value) noexcept {
		u32 const major = (value & 0xFF00'0000) >> 24;
		u32 const minor = (value & 0x00FF'0000) >> 16;
		u32 const patch = value & 0xFFFF;
		return Version{major, minor, patch};
	}

	constexpr explicit Version(u32 major = 0, u32 minor = 0, u32 patch = 0) noexcept
	    : major_(major)
	    , minor_(minor)
	    , patch_(patch) {
	}

	constexpr bool operator<(Version const& rhs) const noexcept {
		if (major_ < rhs.major_) {
			return true;
		}

		if (major_ > rhs.major_) {
			return false;
		}

		if (minor_ < rhs.minor_) {
			return true;
		}

		if (minor_ > rhs.minor_) {
			return false;
		}

		return patch_ < rhs.patch_;
	}

	constexpr bool operator==(Version const& rhs) const noexcept {
		return major_ == rhs.major_
		    && minor_ == rhs.minor_
		    && patch_ == rhs.patch_;
	}

	constexpr u32 major() const noexcept {
		return major_;
	}

	constexpr u32 minor() const noexcept {
		return minor_;
	}

	constexpr u32 patch() const noexcept {
		return patch_;
	}

	template <Component component>
	constexpr Version bump() const noexcept {
		return bump(component);
	}

	constexpr Version bump(Component component) const noexcept {
		switch (component) {
			case kMajor: return Version{major_ + 1};
			case kMinor: return Version{major_, minor_ + 1};
			case kPatch: return Version{major_, minor_, patch_ + 1};
			default    : RB_UNREACHABLE();
		}
	}

	constexpr u32 pack() const noexcept {
		return ((major_ & 0xFF) << 24) | ((minor_ & 0xFF) << 16) | (patch_ & 0xFFFF);
	}

private:
	u32 major_;
	u32 minor_;
	u32 patch_;
};

// synthesized operators

constexpr bool operator>(Version const& lhs, Version const& rhs) noexcept {
	return rhs < lhs;
}

constexpr bool operator>=(Version const& lhs, Version const& rhs) noexcept {
	return !(lhs < rhs);
}

constexpr bool operator<=(Version const& lhs, Version const& rhs) noexcept {
	return !(rhs < lhs);
}

constexpr bool operator!=(Version const& lhs, Version const& rhs) noexcept {
	return !(lhs == rhs);
}

inline std::ostream& operator<<(std::ostream& os, Version const& v) {
	return os << v.major() << '.' << v.minor() << '.' << v.patch();
}

inline constexpr Version kRbVersion{RB_VERSION_MAJOR, RB_VERSION_MINOR, RB_VERSION_PATCH};

} // namespace rb::core
