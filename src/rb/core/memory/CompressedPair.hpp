#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <tuple>

#include <rb/core/in_place.hpp>
#include <rb/core/memory/EmptyBase.hpp>
#include <rb/core/swap.hpp>

namespace rb::core {
inline namespace memory {

	template <class T, class U>
	class CompressedPair final : EmptyBase<U> {
		using Super = EmptyBase<U>;

	public:
		template <class... Args>
		constexpr explicit CompressedPair(InPlaceIndex<0> /*kInPlaceIndex*/, Args&&... args)
		    noexcept(isNothrowDefaultConstructible<U> && isNothrowConstructible<T, Args...>)
		    : first_(RB_FWD(args)...) {
		}

		template <class Arg, class... Args>
		constexpr CompressedPair(InPlaceIndex<1> /*kInPlaceIndex*/, Arg&& arg, Args&&... args)
		    noexcept(isNothrowConstructible<U, Arg> && isNothrowConstructible<T, Args...>)
		    : Super(RB_FWD(arg))
		    , first_(RB_FWD(args)...) {
		}

		constexpr T const& first() const& noexcept {
			return first_;
		}

		constexpr T& first() & noexcept {
			return first_;
		}

		constexpr T const&& first() const&& noexcept {
			return RB_MOVE(first_);
		}

		constexpr T&& first() && noexcept {
			return RB_MOVE(first_);
		}

		constexpr U const& second() const& noexcept {
			return Super::get();
		}

		constexpr U& second() & noexcept {
			return Super::get();
		}

		constexpr U const&& second() const&& noexcept {
			return RB_MOVE(Super::get());
		}

		constexpr U&& second() && noexcept {
			return RB_MOVE(Super::get());
		}

		constexpr void swap(CompressedPair& rhs) noexcept {
			using core::swap;

			swap(first(), rhs.first());
			swap(second(), rhs.second());
		}

	private:
		T first_;
	};

} // namespace memory

namespace impl {

	template <usize>
	struct CompressedPairGet;

	template <>
	struct CompressedPairGet<0> {
		template <class T, class U>
		static constexpr T& get(CompressedPair<T, U>& p) noexcept {
			return p.first();
		}

		template <class T, class U>
		static constexpr T const& get(CompressedPair<T, U> const& p) noexcept {
			return p.first();
		}

		template <class T, class U>
		static constexpr T&& get(CompressedPair<T, U>&& p) noexcept {
			return RB_MOVE(p).first();
		}

		template <class T, class U>
		static constexpr T const&& get(CompressedPair<T, U> const&& p) noexcept {
			return RB_MOVE(p).first();
		}
	};

	template <>
	struct CompressedPairGet<1> {
		template <class T, class U>
		static constexpr U& get(CompressedPair<T, U>& p) noexcept {
			return p.second();
		}

		template <class T, class U>
		static constexpr U const& get(CompressedPair<T, U> const& p) noexcept {
			return p.second();
		}

		template <class T, class U>
		static constexpr U&& get(CompressedPair<T, U>&& p) noexcept {
			return RB_MOVE(p).second();
		}

		template <class T, class U>
		static constexpr U const&& get(CompressedPair<T, U> const&& p) noexcept {
			return RB_MOVE(p).second();
		}
	};

} // namespace impl

inline namespace memory {

	template <usize idx, class T, class E>
	constexpr std::tuple_element_t<idx, CompressedPair<T, E>>&
	get(CompressedPair<T, E>& p) noexcept {
		return impl::CompressedPairGet<idx>::get(p);
	}

	template <usize idx, class T, class E>
	constexpr std::tuple_element_t<idx, CompressedPair<T, E>> const&
	get(CompressedPair<T, E> const& p) noexcept {
		return impl::CompressedPairGet<idx>::get(p);
	}

	template <usize idx, class T, class E>
	constexpr std::tuple_element_t<idx, CompressedPair<T, E>>&&
	get(CompressedPair<T, E>&& p) noexcept {
		return impl::CompressedPairGet<idx>::get(RB_MOVE(p));
	}

	template <usize idx, class T, class E>
	constexpr std::tuple_element_t<idx, CompressedPair<T, E>> const&&
	get(CompressedPair<T, E> const&& p) noexcept {
		return impl::CompressedPairGet<idx>::get(RB_MOVE(p));
	}

} // namespace memory

} // namespace rb::core

// NOLINTBEGIN(cert-dcl58-cpp)

namespace std {

template <class T, class E>
struct tuple_size<rb::core::CompressedPair<T, E>> : rb::core::Constant<usize{2}> {};

template <class T, class E>
struct tuple_element<0, rb::core::CompressedPair<T, E>> {
	using type = T; // NOLINT(readability-identifier-naming)
};

template <class T, class E>
struct tuple_element<1, rb::core::CompressedPair<T, E>> {
	using type = E; // NOLINT(readability-identifier-naming)
};

} // namespace std

// NOLINTEND(cert-dcl58-cpp)
