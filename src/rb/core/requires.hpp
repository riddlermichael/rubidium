#pragma once

#include <rb/core/traits/EnableIf.hpp>

namespace rb::core {

/// Useful replacement for <tt>std::enable_if_t&lt;cond, bool&gt; = true</tt>.
#define RB_REQUIRES(...) rb::core::EnableIf<(__VA_ARGS__), bool> = true
#define RB_REQUIRES_T(...) rb::core::EnableIf<__VA_ARGS__::value, bool> = true

#define RB_REQUIRES_RETURN(T, ...) rb::core::EnableIf<(__VA_ARGS__), T>
#define RB_REQUIRES_RETURN_T(T, ...) rb::core::EnableIf<__VA_ARGS__::value, T>

} // namespace rb::core
