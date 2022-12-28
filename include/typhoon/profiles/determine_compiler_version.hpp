///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tphn
https://www.tphncpp.com

Copyright(c) 2019 John Wellbelove

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

#ifndef TYPHOON_DETERMINE_COMPILER_VERSION_H_HPP
#define TYPHOON_DETERMINE_COMPILER_VERSION_H_HPP

#include "determine_compiler.hpp"

#if !defined(TYPHOON_COMPILER_VERSION) && !defined(TYPHOON_COMPILER_FULL_VERSION)
  #if defined(TYPHOON_COMPILER_GCC)
    #define TYPHOON_COMPILER_VERSION      __GNUC__
    #define TYPHOON_COMPILER_FULL_VERSION ((__GNUC__ * 10000) + (__GNUC_MINOR__ * 100) + __GNUC_PATCHLEVEL__)
  #elif defined(TYPHOON_COMPILER_MICROSOFT)
    #define TYPHOON_COMPILER_VERSION      _MSC_VER
    #define TYPHOON_COMPILER_FULL_VERSION _MSC_FULL_VER
  #elif defined(TYPHOON_COMPILER_ARM5)
    #define TYPHOON_COMPILER_VERSION      __ARMCC_VERSION
    #define TYPHOON_COMPILER_FULL_VERSION __ARMCC_VERSION
  #elif defined(TYPHOON_COMPILER_ARM6)
    #define TYPHOON_COMPILER_VERSION      __ARMCOMPILER_VERSION
    #define TYPHOON_COMPILER_FULL_VERSION __ARMCOMPILER_VERSION
  #elif defined(TYPHOON_COMPILER_ARM7)
    #define TYPHOON_COMPILER_VERSION      __ARMCOMPILER_VERSION
    #define TYPHOON_COMPILER_FULL_VERSION __ARMCOMPILER_VERSION
  #elif defined(TYPHOON_COMPILER_CLANG)
    #define TYPHOON_COMPILER_VERSION      __clang_version__
    #define TYPHOON_COMPILER_FULL_VERSION ((__clang_major__ * 10000) + (__clang_minor__ * 100) + __clang_patchlevel__)
  #elif defined(TYPHOON_COMPILER_GREEN_HILLS)
    #define TYPHOON_COMPILER_VERSION      __GHS_VERSION_NUMBER__
    #define TYPHOON_COMPILER_FULL_VERSION __GHS_VERSION_NUMBER__
  #elif defined(TYPHOON_COMPILER_IAR)
    #define TYPHOON_COMPILER_VERSION      __VER__
    #define TYPHOON_COMPILER_FULL_VERSION __VER__
  #elif defined(TYPHOON_COMPILER_INTEL)
    #define TYPHOON_COMPILER_VERSION      __INTEL_COMPILER
    #define TYPHOON_COMPILER_FULL_VERSION __INTEL_COMPILER
  #elif defined(TYPHOON_COMPILER_TEXAS_INSTRUMENTS)
    #define TYPHOON_COMPILER_VERSION      __TI_COMPILER_VERSION__
    #define TYPHOON_COMPILER_FULL_VERSION __TI_COMPILER_VERSION__
  #elif defined(TYPHOON_COMPILER_TASKING)
    #define TYPHOON_COMPILER_VERSION      __REVISION__
    #define TYPHOON_COMPILER_FULL_VERSION __VERSION__
  #else
    #define TYPHOON_COMPILER_VERSION      0
    #define TYPHOON_COMPILER_FULL_VERSION 0
  #endif
#endif

#endif
