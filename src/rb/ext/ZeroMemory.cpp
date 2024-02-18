#include "ZeroMemory.hpp"

#include <cstring>

RB_RETURNS_NONNULL void* operator new(std::size_t size, rb::ext::ZeroMemory /*kZeroMemory*/) {
	void* const ptr = ::operator new(size);
	std::memset(ptr, 0, size);
	return ptr;
}

void operator delete(void* ptr, rb::ext::ZeroMemory /*kZeroMemory*/) noexcept {
	::operator delete(ptr);
}

RB_RETURNS_NONNULL void* operator new[](std::size_t size, rb::ext::ZeroMemory /*kZeroMemory*/) {
	void* const ptr = ::operator new[](size);
	std::memset(ptr, 0, size);
	return ptr;
}

void operator delete[](void* ptr, rb::ext::ZeroMemory /*kZeroMemory*/) noexcept {
	::operator delete[](ptr);
}

void* operator new(std::size_t size, std::nothrow_t /*nothrow*/, rb::ext::ZeroMemory /*kZeroMemory*/) noexcept {
	void* const ptr = ::operator new(size, std::nothrow);
	if (ptr) {
		std::memset(ptr, 0, size);
	}
	return ptr;
}

void operator delete(void* ptr, std::nothrow_t /*nothrow*/, rb::ext::ZeroMemory /*kZeroMemory*/) noexcept {
	::operator delete(ptr, std::nothrow);
}

void* operator new[](std::size_t size, std::nothrow_t /*nothrow*/, rb::ext::ZeroMemory /*kZeroMemory*/) noexcept {
	void* const ptr = ::operator new[](size, std::nothrow);
	if (ptr) {
		std::memset(ptr, 0, size);
	}
	return ptr;
}

void operator delete[](void* ptr, std::nothrow_t /*nothrow*/, rb::ext::ZeroMemory /*kZeroMemory*/) noexcept {
	::operator delete[](ptr, std::nothrow);
}

#ifdef __cpp_aligned_new

RB_RETURNS_NONNULL void* operator new(std::size_t size, std::align_val_t align, rb::ext::ZeroMemory /*kZeroMemory*/) {
	void* const ptr = ::operator new(size, align);
	std::memset(ptr, 0, size);
	return ptr;
}

void operator delete(void* ptr, std::align_val_t align, rb::ext::ZeroMemory /*kZeroMemory*/) noexcept {
	::operator delete(ptr, align);
}

RB_RETURNS_NONNULL void* operator new[](
    std::size_t size,
    std::align_val_t align,
    rb::ext::ZeroMemory /*kZeroMemory*/) //
{
	void* const ptr = ::operator new[](size, align);
	std::memset(ptr, 0, size);
	return ptr;
}

void operator delete[](void* ptr, std::align_val_t align, rb::ext::ZeroMemory /*kZeroMemory*/) noexcept {
	::operator delete[](ptr, align);
}

void* operator new(
    std::size_t size,
    std::align_val_t align,
    std::nothrow_t /*nothrow*/,
    rb::ext::ZeroMemory /*kZeroMemory*/) noexcept //
{
	void* const ptr = ::operator new(size, align, std::nothrow);
	if (ptr) {
		std::memset(ptr, 0, size);
	}
	return ptr;
}

void operator delete(
    void* ptr,
    std::align_val_t align,
    std::nothrow_t /*nothrow*/,
    rb::ext::ZeroMemory /*kZeroMemory*/) noexcept //
{
	::operator delete(ptr, align, std::nothrow);
}

void* operator new[](
    std::size_t size,
    std::align_val_t align,
    std::nothrow_t /*nothrow*/,
    rb::ext::ZeroMemory /*kZeroMemory*/) noexcept //
{
	void* const ptr = ::operator new[](size, align, std::nothrow);
	if (ptr) {
		std::memset(ptr, 0, size);
	}
	return ptr;
}

void operator delete[](
    void* ptr,
    std::align_val_t align,
    std::nothrow_t /*nothrow*/,
    rb::ext::ZeroMemory /*kZeroMemory*/) noexcept //
{
	::operator delete[](ptr, align, std::nothrow);
}

#endif
