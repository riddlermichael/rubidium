#pragma once

#define RB_STRINGIFY_X(x) RB_STRINGIFY(x)
#define RB_STRINGIFY(x) #x

/// Used to mark (possibly) unused variables.
#define RB_UNUSED(x) static_cast<void>(x)
