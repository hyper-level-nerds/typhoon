/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tpn
https://www.tpncpp.com

Copyright(c) 2017 John Wellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef TYPHOON_ATOMIC_HPP
#define TYPHOON_ATOMIC_HPP

#include "platform.hpp"

#if TYPHOON_HAS_ATOMIC
  #if (TYPHOON_USING_CPP11 && (TYPHOON_USING_STL || defined(TYPHOON_IN_UNIT_TEST))) 
    #include "atomic/atomic_std.hpp"
  #elif defined(TYPHOON_COMPILER_ARM5)
    #include "atomic/atomic_arm.hpp"
  #elif defined(TYPHOON_COMPILER_ARM6)
    #include "atomic/atomic_arm.hpp"
  #elif defined(TYPHOON_COMPILER_GCC)
    #include "atomic/atomic_gcc_sync.hpp"
  #elif defined(TYPHOON_COMPILER_CLANG)
    #include "atomic/atomic_clang_sync.hpp"
  #endif
#endif

#endif
