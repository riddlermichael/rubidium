#pragma once

#define RB_DEFINE_FLAG(ns, type, flag)    \
	namespace ns {                        \
	enum class type : bool {              \
		kNo,                              \
		kYes                              \
	};                                    \
	}                                     \
                                          \
	namespace rb::yes {                   \
	constexpr auto flag = ns::type::kYes; \
	}                                     \
                                          \
	namespace rb::no {                    \
	constexpr auto flag = ns::type::kNo;  \
	}
