#pragma once

#include <new>

#include <rb/core/attributes.hpp>

namespace rb::ext {

struct ZeroMemory {};

inline constexpr ZeroMemory kZeroMemory;

} // namespace rb::ext

// the position of attrs is important because MSVC recognizes only attributes which placed before function

[[nodiscard]] RB_ALLOCATOR RB_RETURNS_NONNULL void* operator new(
    std::size_t size,
    rb::ext::ZeroMemory) RB_ALLOC_SIZE(1);

void operator delete(void* ptr, rb::ext::ZeroMemory) noexcept;

[[nodiscard]] RB_ALLOCATOR RB_RETURNS_NONNULL void* operator new[](
    std::size_t size,
    rb::ext::ZeroMemory) RB_ALLOC_SIZE(1);

void operator delete[](void* ptr, rb::ext::ZeroMemory) noexcept;

[[nodiscard]] RB_ALLOCATOR void* operator new(
    std::size_t size,
    std::nothrow_t,
    rb::ext::ZeroMemory) noexcept RB_ALLOC_SIZE(1);

void operator delete(void* ptr, std::nothrow_t, rb::ext::ZeroMemory) noexcept;

[[nodiscard]] RB_ALLOCATOR void* operator new[](
    std::size_t size,
    std::nothrow_t,
    rb::ext::ZeroMemory) noexcept RB_ALLOC_SIZE(1);

void operator delete[](void* ptr, std::nothrow_t, rb::ext::ZeroMemory) noexcept;

#ifdef __cpp_aligned_new

[[nodiscard]] RB_ALLOCATOR RB_RETURNS_NONNULL void* operator new(
    std::size_t size,
    std::align_val_t align,
    rb::ext::ZeroMemory) RB_ALLOC_SIZE(1);

void operator delete(void* ptr, std::align_val_t align, rb::ext::ZeroMemory) noexcept;

[[nodiscard]] RB_ALLOCATOR RB_RETURNS_NONNULL void* operator new[](
    std::size_t size,
    std::align_val_t align,
    rb::ext::ZeroMemory) RB_ALLOC_SIZE(1);

void operator delete[](void* ptr, std::align_val_t align, rb::ext::ZeroMemory) noexcept;

[[nodiscard]] RB_ALLOCATOR void* operator new(
    std::size_t size,
    std::align_val_t align,
    std::nothrow_t,
    rb::ext::ZeroMemory) noexcept RB_ALLOC_SIZE(1);

void operator delete(void* ptr, std::align_val_t align, std::nothrow_t, rb::ext::ZeroMemory) noexcept;

[[nodiscard]] RB_ALLOCATOR void* operator new[](
    std::size_t size,
    std::align_val_t align,
    std::nothrow_t,
    rb::ext::ZeroMemory) noexcept RB_ALLOC_SIZE(1);

void operator delete[](void* ptr, std::align_val_t align, std::nothrow_t, rb::ext::ZeroMemory) noexcept;

#endif
