#pragma once

#include <rb/core/processor.hpp>

// NOLINTBEGIN(*-macro-to-enum)

#define RB_I8_MIN (-127 - 1)
#define RB_I8_MAX 127
#define RB_I16_MIN (-32'767 - 1)
#define RB_I16_MAX 32'767
#define RB_I32_MIN (-2'147'483'647 - 1)
#define RB_I32_MAX 2'147'483'647
#define RB_I64_MAX 9'223'372'036'854'775'807LL
#define RB_I64_MIN (-RB_I64_MAX - 1LL)

#define RB_U8_MIN 0U
#define RB_U8_MAX 255U
#define RB_U16_MIN 0U
#define RB_U16_MAX 65'535U
#define RB_U32_MIN 0U
#define RB_U32_MAX 4'294'967'295U
#define RB_U64_MIN 0U
#define RB_U64_MAX 18'446'744'073'709'551'615ULL

#if RB_IS_64BIT
	#define RB_ISIZE_MIN RB_I64_MIN
	#define RB_ISIZE_MAX RB_I64_MAX
	#define RB_USIZE_MIN RB_U64_MIN
	#define RB_USIZE_MAX RB_U64_MAX
#elif RB_IS_32BIT
	#define RB_ISIZE_MIN RB_I32_MIN
	#define RB_ISIZE_MAX RB_I32_MAX
	#define RB_USIZE_MIN RB_U32_MIN
	#define RB_USIZE_MAX RB_U32_MAX
#endif

#define RB_F32_MIN 0x1p-126F
#define RB_F32_MAX 0x1.fffffep127F
#define RB_F32_INF __builtin_huge_valf()
#define RB_F64_MIN 0x1p-1022
#define RB_F64_MAX 0x1.fffffffffffffp1023
#define RB_F64_INF __builtin_huge_val()

// NOLINTEND(*-macro-to-enum)
