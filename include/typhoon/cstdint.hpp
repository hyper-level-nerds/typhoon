//
// Copyright (c) 2016-2019 Jin (jaehwanspin@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/hyper-level-nerds/typhoon
//

#ifndef TYPHOON_CSTDINT_HPP
#define TYPHOON_CSTDINT_HPP

#include <stdint.h>

namespace tpn {

using ::int8_t;
using ::int16_t;
using ::int32_t;
using ::int64_t;

using ::int_fast8_t;
using ::int_fast16_t;
using ::int_fast32_t;
using ::int_fast64_t;

using ::int_least8_t;
using ::int_least16_t;
using ::int_least32_t;
using ::int_least64_t;

using ::intmax_t;
using ::intptr_t;
  
using ::uint8_t;
using ::uint16_t;
using ::uint32_t;
using ::uint64_t;

using ::uint_fast8_t;
using ::uint_fast16_t;
using ::uint_fast32_t;
using ::uint_fast64_t;

using ::uint_least8_t;
using ::uint_least16_t;
using ::uint_least32_t;
using ::uint_least64_t;

using ::uintmax_t;
using ::uintptr_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

}

#endif