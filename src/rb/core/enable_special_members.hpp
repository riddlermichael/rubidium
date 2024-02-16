#pragma once

#include <rb/core/traits/assignable.hpp>
#include <rb/core/traits/constructible.hpp>
#include <rb/core/traits/destructible.hpp>

namespace rb::core {

/// A mixin helper to conditionally enable or disable the default constructor.
template <bool ctor, class Tag = void>
struct EnableDefaultConstructor {
	constexpr EnableDefaultConstructor() noexcept = default;
	constexpr EnableDefaultConstructor(EnableDefaultConstructor const&) noexcept = default;
	constexpr EnableDefaultConstructor(EnableDefaultConstructor&&) noexcept = default;
	EnableDefaultConstructor& operator=(EnableDefaultConstructor const&) noexcept = default;
	EnableDefaultConstructor& operator=(EnableDefaultConstructor&&) noexcept = default;
};

template <class Tag>
struct EnableDefaultConstructor<false, Tag> {
	constexpr EnableDefaultConstructor() noexcept = delete;
	constexpr EnableDefaultConstructor(EnableDefaultConstructor const&) noexcept = default;
	constexpr EnableDefaultConstructor(EnableDefaultConstructor&&) noexcept = default;
	EnableDefaultConstructor& operator=(EnableDefaultConstructor const&) noexcept = default;
	EnableDefaultConstructor& operator=(EnableDefaultConstructor&&) noexcept = default;
};

/// A mixin helper to conditionally enable or disable the default constructor
/// taking into account the constructor's triviality.
template <bool ctor, bool isTrivial, class Tag = void>
struct EnableDefaultTrivialConstructor {
	constexpr EnableDefaultTrivialConstructor() noexcept = default;
	constexpr EnableDefaultTrivialConstructor(EnableDefaultTrivialConstructor const&) noexcept = default;
	constexpr EnableDefaultTrivialConstructor(EnableDefaultTrivialConstructor&&) noexcept = default;
	EnableDefaultTrivialConstructor& operator=(EnableDefaultTrivialConstructor const&) noexcept = default;
	EnableDefaultTrivialConstructor& operator=(EnableDefaultTrivialConstructor&&) noexcept = default;
};

template <class Tag>
struct EnableDefaultTrivialConstructor<true, false, Tag> {
	constexpr EnableDefaultTrivialConstructor() noexcept { // NOLINT(modernize-use-equals-default)
	}

	constexpr EnableDefaultTrivialConstructor(EnableDefaultTrivialConstructor const&) noexcept = default;
	constexpr EnableDefaultTrivialConstructor(EnableDefaultTrivialConstructor&&) noexcept = default;
	EnableDefaultTrivialConstructor& operator=(EnableDefaultTrivialConstructor const&) noexcept = default;
	EnableDefaultTrivialConstructor& operator=(EnableDefaultTrivialConstructor&&) noexcept = default;
};

template <bool isTrivial, class Tag>
struct EnableDefaultTrivialConstructor<false, isTrivial, Tag> {
	constexpr EnableDefaultTrivialConstructor() noexcept = delete;
	constexpr EnableDefaultTrivialConstructor(EnableDefaultTrivialConstructor const&) noexcept = default;
	constexpr EnableDefaultTrivialConstructor(EnableDefaultTrivialConstructor&&) noexcept = default;
	EnableDefaultTrivialConstructor& operator=(EnableDefaultTrivialConstructor const&) noexcept = default;
	EnableDefaultTrivialConstructor& operator=(EnableDefaultTrivialConstructor&&) noexcept = default;
};

template <class T, class Tag = void>
struct EnableDefaultTrivialConstructorFor
    : private EnableDefaultTrivialConstructor<isDefaultConstructible<T>, isTriviallyDefaultConstructible<T>> {};

/// A mixin helper to conditionally enable or disable the default destructor.
template <bool dtor, class Tag = void>
struct EnableDestructor {};

template <class Tag>
struct EnableDestructor<false, Tag> {
	~EnableDestructor() noexcept = delete;
};

/// A mixin helper to conditionally enable or disable the copy/move special members.
template <bool copy, bool copyAssignment, bool move, bool moveAssignment, class Tag = void>
struct EnableCopyMove {
};

template <class Tag>
struct EnableCopyMove<true, true, true, true, Tag> {
	constexpr EnableCopyMove() noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove const&) noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove&&) noexcept = default;
	EnableCopyMove& operator=(EnableCopyMove const&) noexcept = default;
	EnableCopyMove& operator=(EnableCopyMove&&) noexcept = default;
};

template <class Tag>
struct EnableCopyMove<true, true, true, false, Tag> {
	constexpr EnableCopyMove() noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove const&) noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove&&) noexcept = default;
	EnableCopyMove& operator=(EnableCopyMove const&) noexcept = default;
	EnableCopyMove& operator=(EnableCopyMove&&) noexcept = delete;
};

template <class Tag>
struct EnableCopyMove<true, true, false, true, Tag> {
	constexpr EnableCopyMove() noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove const&) noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove&&) noexcept = delete;
	EnableCopyMove& operator=(EnableCopyMove const&) noexcept = default;
	EnableCopyMove& operator=(EnableCopyMove&&) noexcept = default;
};

template <class Tag>
struct EnableCopyMove<true, true, false, false, Tag> {
	constexpr EnableCopyMove() noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove const&) noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove&&) noexcept = delete;
	EnableCopyMove& operator=(EnableCopyMove const&) noexcept = default;
	EnableCopyMove& operator=(EnableCopyMove&&) noexcept = delete;
};

template <class Tag>
struct EnableCopyMove<true, false, true, true, Tag> {
	constexpr EnableCopyMove() noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove const&) noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove&&) noexcept = default;
	EnableCopyMove& operator=(EnableCopyMove const&) noexcept = delete;
	EnableCopyMove& operator=(EnableCopyMove&&) noexcept = default;
};

template <class Tag>
struct EnableCopyMove<true, false, true, false, Tag> {
	constexpr EnableCopyMove() noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove const&) noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove&&) noexcept = default;
	EnableCopyMove& operator=(EnableCopyMove const&) noexcept = delete;
	EnableCopyMove& operator=(EnableCopyMove&&) noexcept = delete;
};

template <class Tag>
struct EnableCopyMove<true, false, false, true, Tag> {
	constexpr EnableCopyMove() noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove const&) noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove&&) noexcept = delete;
	EnableCopyMove& operator=(EnableCopyMove const&) noexcept = delete;
	EnableCopyMove& operator=(EnableCopyMove&&) noexcept = default;
};

template <class Tag>
struct EnableCopyMove<true, false, false, false, Tag> {
	constexpr EnableCopyMove() noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove const&) noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove&&) noexcept = delete;
	EnableCopyMove& operator=(EnableCopyMove const&) noexcept = delete;
	EnableCopyMove& operator=(EnableCopyMove&&) noexcept = delete;
};

template <class Tag>
struct EnableCopyMove<false, true, true, true, Tag> {
	constexpr EnableCopyMove() noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove const&) noexcept = delete;
	constexpr EnableCopyMove(EnableCopyMove&&) noexcept = default;
	EnableCopyMove& operator=(EnableCopyMove const&) noexcept = default;
	EnableCopyMove& operator=(EnableCopyMove&&) noexcept = default;
};

template <class Tag>
struct EnableCopyMove<false, true, true, false, Tag> {
	constexpr EnableCopyMove() noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove const&) noexcept = delete;
	constexpr EnableCopyMove(EnableCopyMove&&) noexcept = default;
	EnableCopyMove& operator=(EnableCopyMove const&) noexcept = default;
	EnableCopyMove& operator=(EnableCopyMove&&) noexcept = delete;
};

template <class Tag>
struct EnableCopyMove<false, true, false, true, Tag> {
	constexpr EnableCopyMove() noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove const&) noexcept = delete;
	constexpr EnableCopyMove(EnableCopyMove&&) noexcept = delete;
	EnableCopyMove& operator=(EnableCopyMove const&) noexcept = default;
	EnableCopyMove& operator=(EnableCopyMove&&) noexcept = default;
};

template <class Tag>
struct EnableCopyMove<false, true, false, false, Tag> {
	constexpr EnableCopyMove() noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove const&) noexcept = delete;
	constexpr EnableCopyMove(EnableCopyMove&&) noexcept = delete;
	EnableCopyMove& operator=(EnableCopyMove const&) noexcept = default;
	EnableCopyMove& operator=(EnableCopyMove&&) noexcept = delete;
};

template <class Tag>
struct EnableCopyMove<false, false, true, true, Tag> {
	constexpr EnableCopyMove() noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove const&) noexcept = delete;
	constexpr EnableCopyMove(EnableCopyMove&&) noexcept = default;
	EnableCopyMove& operator=(EnableCopyMove const&) noexcept = delete;
	EnableCopyMove& operator=(EnableCopyMove&&) noexcept = default;
};

template <class Tag>
struct EnableCopyMove<false, false, true, false, Tag> {
	constexpr EnableCopyMove() noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove const&) noexcept = delete;
	constexpr EnableCopyMove(EnableCopyMove&&) noexcept = default;
	EnableCopyMove& operator=(EnableCopyMove const&) noexcept = delete;
	EnableCopyMove& operator=(EnableCopyMove&&) noexcept = delete;
};

template <class Tag>
struct EnableCopyMove<false, false, false, true, Tag> {
	constexpr EnableCopyMove() noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove const&) noexcept = delete;
	constexpr EnableCopyMove(EnableCopyMove&&) noexcept = delete;
	EnableCopyMove& operator=(EnableCopyMove const&) noexcept = delete;
	EnableCopyMove& operator=(EnableCopyMove&&) noexcept = default;
};

template <class Tag>
struct EnableCopyMove<false, false, false, false, Tag> {
	constexpr EnableCopyMove() noexcept = default;
	constexpr EnableCopyMove(EnableCopyMove const&) noexcept = delete;
	constexpr EnableCopyMove(EnableCopyMove&&) noexcept = delete;
	EnableCopyMove& operator=(EnableCopyMove const&) noexcept = delete;
	EnableCopyMove& operator=(EnableCopyMove&&) noexcept = delete;
};

template <class T, class Tag = void>
struct EnableCopyMoveFor
    : private EnableCopyMove<
          isCopyConstructible<T>, isCopyAssignable<T>, isMoveConstructible<T>, isMoveAssignable<T>,
          Tag> {};

/// A mixin helper to conditionally enable or disable the special members.
/// The @p Tag type parameter is to make mixin bases unique and thus avoid ambiguities.
template <bool ctor, bool dtor, bool copy, bool copyAssignment, bool move, bool moveAssignment, class Tag = void>
struct EnableSpecialMembers
    : private EnableDefaultConstructor<ctor, Tag>
    , private EnableDestructor<dtor, Tag>
    , private EnableCopyMove<copy, copyAssignment, move, moveAssignment, Tag> {};

template <class T, class Tag = void>
struct EnableSpecialMembersFor
    : private EnableSpecialMembers<isDefaultConstructible<T>, isDestructible<T>,
          isCopyConstructible<T>, isCopyAssignable<T>, isMoveConstructible<T>, isMoveAssignable<T>,
          Tag> {};

} // namespace rb::core
