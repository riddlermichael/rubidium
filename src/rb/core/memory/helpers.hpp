#pragma once

#include <rb/core/traits/builtins.hpp>
#include <rb/core/traits/destructible.hpp>
#include <rb/core/traits/enums.hpp>
#include <rb/core/traits/IsArithmetic.hpp>
#include <rb/core/traits/ops.hpp>

namespace rb::core::impl {

template <class T>
using SafeDelete = Or<IsArithmetic<T>, IsEnum<T>, IsUnion<T>,
    If<IsComplete<T>, IsFinal<T>, HasVirtualDestructor<T>>>;

} // namespace rb::core::impl
