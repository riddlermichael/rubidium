#pragma once

#include <rb/core/Flags.hpp>
#include <rb/core/meta/TypeSeq.hpp>
#include <rb/core/traits/Bool.hpp>

namespace rb::core {
inline namespace traits {

	enum class FunctionAttribute {
		kNone = 0x00,
		kNoexcept = 0x01,
		kMethod = 0x02,
		kMemberFunction [[maybe_unused]] = kMethod,
		kConst = 0x04,
		kVolatile = 0x08,
		kLvalueRef = 0x10,
		kRvalueRef = 0x20,
		kVariadic = 0x40
	};

	using FunctionAttributes = Flags<FunctionAttribute>;

	template <class F>
	struct FunctionTraits {
		using IsCallable = False;
	};

	// cv (4) + ref (3) + noexcept (2) + variadic == 48

	// regular functions

	template <class R, class... Args>
	struct FunctionTraits<R(Args...)> {
		using IsCallable = True;
		using Type = R(Args...);
		using FunctionType = R(Args...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kNone,
		};
	};

	template <class R, class... Args>
	struct FunctionTraits<R(Args...) noexcept> {
		using IsCallable = True;
		using Type = R(Args...) noexcept;
		using FunctionType = R(Args...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kNone,
		    FunctionAttribute::kNoexcept,
		};
	};

	template <class R, class... Args>
	struct FunctionTraits<R(Args..., ...)> {
		using IsCallable = True;
		using Type = R(Args..., ...);
		using FunctionType = R(Args..., ...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kNone,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class... Args>
	struct FunctionTraits<R(Args..., ...) noexcept> {
		using IsCallable = True;
		using Type = R(Args..., ...) noexcept;
		using FunctionType = R(Args..., ...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kNone,
		    FunctionAttribute::kNoexcept,
		    FunctionAttribute::kVariadic,
		};
	};

	// methods

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...)> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...);
		using FunctionType = R(Args...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) const> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) const;
		using FunctionType = R(Args...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) volatile> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) volatile;
		using FunctionType = R(Args...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kVolatile,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) const volatile> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) const volatile;
		using FunctionType = R(Args...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kVolatile,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...)&> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) &;
		using FunctionType = R(Args...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kLvalueRef,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) const&> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) const&;
		using FunctionType = R(Args...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kLvalueRef,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) volatile&> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) volatile&;
		using FunctionType = R(Args...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kLvalueRef,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) const volatile&> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) const volatile&;
		using FunctionType = R(Args...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kLvalueRef,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) &&> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) &&;
		using FunctionType = R(Args...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kRvalueRef,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) const&&> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) const&&;
		using FunctionType = R(Args...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kRvalueRef,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) volatile&&> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) volatile&&;
		using FunctionType = R(Args...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kRvalueRef,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) const volatile&&> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) const volatile&&;
		using FunctionType = R(Args...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kRvalueRef,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) noexcept;
		using FunctionType = R(Args...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kNoexcept,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) const noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) const noexcept;
		using FunctionType = R(Args...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kNoexcept,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) volatile noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) volatile noexcept;
		using FunctionType = R(Args...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kNoexcept,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) const volatile noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) const volatile noexcept;
		using FunctionType = R(Args...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kNoexcept,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) & noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) & noexcept;
		using FunctionType = R(Args...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kLvalueRef,
		    FunctionAttribute::kNoexcept,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) const & noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) const& noexcept;
		using FunctionType = R(Args...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kLvalueRef,
		    FunctionAttribute::kNoexcept,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) volatile & noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) volatile& noexcept;
		using FunctionType = R(Args...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kLvalueRef,
		    FunctionAttribute::kNoexcept,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) const volatile & noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) const volatile& noexcept;
		using FunctionType = R(Args...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kLvalueRef,
		    FunctionAttribute::kNoexcept,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) && noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) && noexcept;
		using FunctionType = R(Args...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kRvalueRef,
		    FunctionAttribute::kNoexcept,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) const && noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) const&& noexcept;
		using FunctionType = R(Args...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kRvalueRef,
		    FunctionAttribute::kNoexcept,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) volatile && noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) volatile&& noexcept;
		using FunctionType = R(Args...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kRvalueRef,
		    FunctionAttribute::kNoexcept,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args...) const volatile && noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args...) const volatile&& noexcept;
		using FunctionType = R(Args...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kRvalueRef,
		    FunctionAttribute::kNoexcept,
		};
	};

	// variadic methods

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...)> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...);
		using FunctionType = R(Args..., ...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) const> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) const;
		using FunctionType = R(Args..., ...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) volatile> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) volatile;
		using FunctionType = R(Args..., ...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) const volatile> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) const volatile;
		using FunctionType = R(Args..., ...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...)&> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) &;
		using FunctionType = R(Args..., ...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kLvalueRef,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) const&> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) const&;
		using FunctionType = R(Args..., ...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kLvalueRef,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) volatile&> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) volatile&;
		using FunctionType = R(Args..., ...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kLvalueRef,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) const volatile&> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) const volatile&;
		using FunctionType = R(Args..., ...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kLvalueRef,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) &&> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) &&;
		using FunctionType = R(Args..., ...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kRvalueRef,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) const&&> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) const&&;
		using FunctionType = R(Args..., ...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kRvalueRef,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) volatile&&> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) volatile&&;
		using FunctionType = R(Args..., ...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kRvalueRef,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) const volatile&&> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) const volatile&&;
		using FunctionType = R(Args..., ...);
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kRvalueRef,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) noexcept;
		using FunctionType = R(Args..., ...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kNoexcept,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) const noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) const noexcept;
		using FunctionType = R(Args..., ...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kNoexcept,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) volatile noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) volatile noexcept;
		using FunctionType = R(Args..., ...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kNoexcept,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) const volatile noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) const volatile noexcept;
		using FunctionType = R(Args..., ...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kNoexcept,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) & noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) & noexcept;
		using FunctionType = R(Args..., ...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kLvalueRef,
		    FunctionAttribute::kNoexcept,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) const & noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) const& noexcept;
		using FunctionType = R(Args..., ...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kLvalueRef,
		    FunctionAttribute::kNoexcept,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) volatile & noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) volatile& noexcept;
		using FunctionType = R(Args..., ...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kLvalueRef,
		    FunctionAttribute::kNoexcept,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) const volatile & noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) const volatile& noexcept;
		using FunctionType = R(Args..., ...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kLvalueRef,
		    FunctionAttribute::kNoexcept,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) && noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) && noexcept;
		using FunctionType = R(Args..., ...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kRvalueRef,
		    FunctionAttribute::kNoexcept,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) const && noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) const&& noexcept;
		using FunctionType = R(Args..., ...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kRvalueRef,
		    FunctionAttribute::kNoexcept,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) volatile && noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) volatile&& noexcept;
		using FunctionType = R(Args..., ...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kRvalueRef,
		    FunctionAttribute::kNoexcept,
		    FunctionAttribute::kVariadic,
		};
	};

	template <class R, class T, class... Args>
	struct FunctionTraits<R (T::*)(Args..., ...) const volatile && noexcept> {
		using IsCallable = True;
		using Class = T;
		using Type = R (T::*)(Args..., ...) const volatile&& noexcept;
		using FunctionType = R(Args..., ...) noexcept;
		using ReturnType = R;
		using Parameters = TypeSeq<Args...>;

		static constexpr FunctionAttributes kAttributes{
		    FunctionAttribute::kMethod,
		    FunctionAttribute::kConst,
		    FunctionAttribute::kVolatile,
		    FunctionAttribute::kRvalueRef,
		    FunctionAttribute::kNoexcept,
		    FunctionAttribute::kVariadic,
		};
	};

	template <auto f>
	using CallableTraits = FunctionTraits<RemovePointer<decltype(f)>>;

} // namespace traits

} // namespace rb::core
