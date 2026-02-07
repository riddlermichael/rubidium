#pragma once

#include <rb/core/attributes.hpp>

#define RB_DEFINE_FLAG(ns, type, flag)      \
	namespace ns {                          \
	enum class RB_CLOSED_ENUM type : bool { \
		kNo,                                \
		kYes                                \
	};                                      \
	}                                       \
                                            \
	namespace rb::yes {                     \
	constexpr auto flag = ns::type::kYes;   \
	}                                       \
                                            \
	namespace rb::no {                      \
	constexpr auto flag = ns::type::kNo;    \
	}
