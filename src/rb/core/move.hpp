#pragma once

#include <rb/core/traits/remove.hpp>

/// Lightweight alternative to `std::move`.
#define RB_MOVE(...) static_cast<rb::core::RemoveRef<decltype(__VA_ARGS__)>&&>(__VA_ARGS__)

/// Lightweight alternative to `std::forward`.
#define RB_FWD(...) static_cast<decltype(__VA_ARGS__)&&>(__VA_ARGS__)
