#pragma once

#include <rb/core/compiler.hpp>
#include <rb/core/strings.hpp>
#include <rb/core/Version.hpp>

namespace rb::core {

class CompilerInfo final {
public:
	enum class Id {
		kClang,
		kGcc,
		kMingw,
		kMsvc,
#if defined(RB_COMPILER_CLANG)
		kThisId = kClang
#elif defined(RB_COMPILER_GCC)
		kThisId = kGcc
#elif defined(RB_COMPILER_MINGW)
		kThisId = kMingw
#elif defined(RB_COMPILER_MSVC)
		kThisId = kMsvc
#endif
	};

	static constexpr Version kThisVersion{
	    RB_COMPILER_VERSION_MAJOR,
	    RB_COMPILER_VERSION_MINOR,
	    RB_COMPILER_VERSION_PATCH};

	static CompilerInfo const kThis;

	constexpr CompilerInfo(Id id, Version version, LiteralString desc = nullptr) noexcept
	    : id_(id)
	    , version_(version)
	    , desc_(desc) {
	}

	constexpr Id id() const noexcept {
		return id_;
	}

	constexpr Version version() const noexcept {
		return version_;
	}

	constexpr LiteralString desc() const noexcept {
		return desc_;
	}

private:
	Id id_;
	Version version_;
	LiteralString desc_;
};

constexpr CompilerInfo CompilerInfo::kThis{CompilerInfo::Id::kThisId, CompilerInfo::kThisVersion
// there is no string description for MSVC compiler except a string version
#ifndef RB_COMPILER_MSVC
    ,
    RB_COMPILER_VERSION_STRING
#endif
};

inline std::ostream& operator<<(std::ostream& os, CompilerInfo::Id id) {
	using Id = CompilerInfo::Id;
	switch (id) {
		case Id::kClang: return os << "Clang";
		case Id::kGcc  : return os << "GCC";
		case Id::kMingw: return os << "MinGW";
		case Id::kMsvc : return os << "MSVC";
		default        : return os << "C++ compiler";
	}
}

inline std::ostream& operator<<(std::ostream& os, CompilerInfo const& compiler) {
	os << compiler.id() << ' ';
	return compiler.desc()
	         ? (os << compiler.desc())
	         : (os << compiler.version());
}

} // namespace rb::core
