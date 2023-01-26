//
// Copyright (c) 2016-2019 Jin (jaehwanspin@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/hyper-level-nerds/typhoon
//

#ifndef TYPHOON_DETAIL_CONFIG_HPP
#define TYPHOON_DETAIL_CONFIG_HPP

#if __cplusplus >= 202002L
    // Zephyr RTOS
    #if defined(__ZEPHYR__)
        #define TYPHOON_RTOS_ZEPHYR 1
    #endif
#else
    #error "you are not using over C++17"
#endif

#if TYPHOON_RTOS_ZEPHYR == 1

    #if CONFIG_EXCEPTIONS
        #define TYPHOON_HAS_EXCEPTIONS 1
    #endif

#else

#endif // TYPHOON_RTOS_ZEPHYR == 1

#endif