#pragma once

#include <rb/core/traits/IsConvertible.hpp>
#include <rb/core/traits/IsSame.hpp>
#include <rb/core/traits/None.hpp>
#include <rb/core/traits/Void.hpp>

namespace rb::core {
inline namespace traits {

	template <class Default, class AlwaysVoid, template <class...> class Op, class... Args>
	struct Detector {
		using ValueType = False;
		using Type = Default;
	};

	template <class Default, template <class...> class Op, class... Args>
	struct Detector<Default, Void<Op<Args...>>, Op, Args...> {
		using ValueType = True;
		using Type = Op<Args...>;
	};

	template <template <class...> class Op, class... Args>
	using IsDetected = typename Detector<None, void, Op, Args...>::ValueType;

	template <template <class...> class Op, class... Args>
	inline constexpr bool isDetected = IsDetected<Op, Args...>::value;

	template <template <class...> class Op, class... Args>
	using DetectedType = typename Detector<None, void, Op, Args...>::Type;

	template <class Default, template <class...> class Op, class... Args>
	using DetectedOr = Detector<Default, void, Op, Args...>;

	template <class Default, template <class...> class Op, class... Args>
	using DetectedOrType = typename Detector<Default, void, Op, Args...>::Type;

	template <class Expected, template <class...> class Op, class... Args>
	using IsDetectedExact = IsSame<Expected, DetectedType<Op, Args...>>;

	template <class Expected, template <class...> class Op, class... Args>
	inline constexpr bool isDetectedExact = IsDetectedExact<Expected, Op, Args...>::value;

	template <class To, template <class...> class Op, class... Args>
	using IsDetectedConvertible = IsConvertible<DetectedType<Op, Args...>, To>;

	template <class To, template <class...> class Op, class... Args>
	inline constexpr bool isDetectedConvertible = IsDetectedConvertible<To, Op, Args...>::value;

} // namespace traits
} // namespace rb::core

#define RB_METHOD_DETECTOR_NAME(method, name)                                   \
	template <class T, class... Args>                                           \
	using name##Detector = decltype(RB_DECLVAL(T).method(RB_DECLVAL(Args)...)); \
                                                                                \
	template <class T, class... Args>                                           \
	using Has##name = rb::core::IsDetected<name##Detector, T, Args...>;

#define RB_METHOD_DETECTOR(method) RB_METHOD_DETECTOR_NAME(method, method)

#define RB_STATIC_METHOD_DETECTOR(method) \
	template <class T, class... Args>     \
	using method##StaticDetector = decltype(T::method(RB_DECLVAL(Args)...));

#define RB_FUNCTION_DETECTOR(func, name)                        \
	template <class... Args>                                    \
	using name##Detector = decltype(func(RB_DECLVAL(Args)...)); \
                                                                \
	template <class... Args>                                    \
	using Has##name = rb::core::IsDetected<name##Detector, Args...>;

#define RB_TYPE_DETECTOR_NAME(type, name) \
	template <class T>                    \
	using name##Detector = typename T::type;

#define RB_TYPE_DETECTOR(type) RB_TYPE_DETECTOR_NAME(type, type)
