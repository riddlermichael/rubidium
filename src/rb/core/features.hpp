#pragma once

/**
 * The RB_ENABLED macro implements a safe compile time check for features of Rubidium.
 * Features can be in three states:
 * <ul>
 * <li> 0 or undefined: this will lead to a compile error when testing for it
 * <li> -1: the feature is not available
 * <li> 1: the feature is available
 * </ul>
 */
#define RB_ENABLED(feature) (1 / RB_FEATURE_##feature == 1)
#define RB_DISABLED(feature) (1 / RB_FEATURE_##feature == -1)
#define RB_REQUIRE_ENABLED(feature) \
	static_assert(RB_FEATURE_##feature == 1, "Required feature " #feature " for file " __FILE__ " not available.")

#define RB_HAS(feature) (1 / RB_HAS_##feature == 1)
#define RB_USE(engine) (1 / RB_USE_##engine == 1)

#include <rb/rb_features.hpp>
