#pragma once

namespace rb::core {
inline namespace traits {

	/// None is a class type used by rb::core::traits::IsDetected to indicate detection failure.
	struct None {
		None(None const&) = delete;
		~None() = delete;

		void operator=(None const&) = delete;
	};

} // namespace traits
} // namespace rb::core
