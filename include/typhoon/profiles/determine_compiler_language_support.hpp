///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tpn
https://www.tpncpp.com

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

#ifndef TYPHOON_DETERMINE_COMPILER_LANGUAGE_SUPPORT_H_HPP
#define TYPHOON_DETERMINE_COMPILER_LANGUAGE_SUPPORT_H_HPP

#include <math.h>

#include "determine_compiler.hpp"

// Determine C++23 support
#if !defined(TYPHOON_CPP23_SUPPORTED)
  #define TYPHOON_CPP23_SUPPORTED 0
#endif

#if TYPHOON_CPP23_SUPPORTED
  #define TYPHOON_CPP11_SUPPORTED 1
  #define TYPHOON_CPP14_SUPPORTED 1
  #define TYPHOON_CPP17_SUPPORTED 1
  #define TYPHOON_CPP20_SUPPORTED 1
#endif

// Determine C++20 support
#if !defined(TYPHOON_CPP20_SUPPORTED)
  #if defined(__cplusplus)
    #if defined(TYPHOON_COMPILER_MICROSOFT)
      #if defined(_MSVC_LANG)
        #define TYPHOON_CPP20_SUPPORTED (_MSVC_LANG >= 202002L)
      #else
        #define TYPHOON_CPP20_SUPPORTED (_MSC_VER >= 1929)
      #endif
    #elif defined(TYPHOON_COMPILER_ARM5)
      #define TYPHOON_CPP20_SUPPORTED 0
    #else
      #define TYPHOON_CPP20_SUPPORTED (__cplusplus >= 202002L)
    #endif
  #else
    #define TYPHOON_CPP20_SUPPORTED 0
  #endif
#endif

#if TYPHOON_CPP20_SUPPORTED
  #define TYPHOON_CPP11_SUPPORTED 1
  #define TYPHOON_CPP14_SUPPORTED 1
  #define TYPHOON_CPP17_SUPPORTED 1
#endif

// Determine C++17 support
#if !defined(TYPHOON_CPP17_SUPPORTED)
  #if defined(__cplusplus)
    #if defined(TYPHOON_COMPILER_MICROSOFT)
      #if defined(_MSVC_LANG)
        #define TYPHOON_CPP17_SUPPORTED (_MSVC_LANG >= 201703L)
      #else
        #define TYPHOON_CPP17_SUPPORTED (_MSC_VER >= 1914)
      #endif
    #elif defined(TYPHOON_COMPILER_ARM5)
      #define TYPHOON_CPP17_SUPPORTED 0
    #else
      #define TYPHOON_CPP17_SUPPORTED (__cplusplus >= 201703L)
    #endif
  #else
    #define TYPHOON_CPP17_SUPPORTED 0
  #endif
#endif

#if TYPHOON_CPP17_SUPPORTED
  #define TYPHOON_CPP11_SUPPORTED 1
  #define TYPHOON_CPP14_SUPPORTED 1
#endif

// Determine C++14 support
#if !defined(TYPHOON_CPP14_SUPPORTED)
  #if defined(__cplusplus)
    #if defined(TYPHOON_COMPILER_MICROSOFT)
      #if defined(_MSVC_LANG)
        #define TYPHOON_CPP14_SUPPORTED (_MSVC_LANG >= 201402L)
      #else
        #define TYPHOON_CPP14_SUPPORTED (_MSC_VER >= 1900)
      #endif
    #elif defined(TYPHOON_COMPILER_ARM5)
      #define TYPHOON_CPP14_SUPPORTED 0
    #else
      #define TYPHOON_CPP14_SUPPORTED (__cplusplus >= 201402L)
    #endif
  #else
    #define TYPHOON_CPP14_SUPPORTED 0
  #endif
#endif

#if TYPHOON_CPP14_SUPPORTED
  #define TYPHOON_CPP11_SUPPORTED 1
#endif

// Determine C++11 support
#if !defined(TYPHOON_CPP11_SUPPORTED)
  #if defined(__cplusplus)
    #if defined(TYPHOON_COMPILER_MICROSOFT)
      #if defined(_MSVC_LANG)
        #define TYPHOON_CPP11_SUPPORTED (_MSVC_LANG >= 201103L)
      #else
        #define TYPHOON_CPP11_SUPPORTED (_MSC_VER >= 1700)
      #endif
    #elif defined(TYPHOON_COMPILER_ARM5)
      #define TYPHOON_CPP11_SUPPORTED 0
    #else
      #define TYPHOON_CPP11_SUPPORTED (__cplusplus >= 201103L)
    #endif
  #else
    #define TYPHOON_CPP11_SUPPORTED 0
  #endif
#endif

// Helper macros
#define TYPHOON_CPP11_NOT_SUPPORTED (!TYPHOON_CPP11_SUPPORTED)
#define TYPHOON_CPP14_NOT_SUPPORTED (!TYPHOON_CPP14_SUPPORTED)
#define TYPHOON_CPP17_NOT_SUPPORTED (!TYPHOON_CPP17_SUPPORTED)
#define TYPHOON_CPP20_NOT_SUPPORTED (!TYPHOON_CPP20_SUPPORTED)
#define TYPHOON_CPP23_NOT_SUPPORTED (!TYPHOON_CPP23_SUPPORTED)

#if !defined(TYPHOON_NO_NULLPTR_SUPPORT)
  #define TYPHOON_NO_NULLPTR_SUPPORT TYPHOON_CPP11_NOT_SUPPORTED
#endif

#if !defined(TYPHOON_NO_SMALL_CHAR_SUPPORT)
  #define TYPHOON_NO_SMALL_CHAR_SUPPORT TYPHOON_CPP20_NOT_SUPPORTED
#endif

#if !defined(TYPHOON_NO_LARGE_CHAR_SUPPORT)
#define TYPHOON_NO_LARGE_CHAR_SUPPORT TYPHOON_CPP11_NOT_SUPPORTED
#endif

#if !defined(TYPHOON_CPP11_TYPE_TRAITS_IS_TRIVIAL_SUPPORTED)
  #define TYPHOON_CPP11_TYPE_TRAITS_IS_TRIVIAL_SUPPORTED TYPHOON_CPP14_SUPPORTED
#endif

// 'Using' macros
#define TYPHOON_USING_CPP11 (TYPHOON_CPP11_SUPPORTED == 1)
#define TYPHOON_USING_CPP14 (TYPHOON_CPP14_SUPPORTED == 1)
#define TYPHOON_USING_CPP17 (TYPHOON_CPP17_SUPPORTED == 1)
#define TYPHOON_USING_CPP20 (TYPHOON_CPP20_SUPPORTED == 1)
#define TYPHOON_USING_CPP23 (TYPHOON_CPP23_SUPPORTED == 1)

// Language standard
#if TYPHOON_USING_CPP23
  #define TYPHOON_LANGUAGE_STANDARD 23
#elif TYPHOON_USING_CPP20
  #define TYPHOON_LANGUAGE_STANDARD 20
#elif TYPHOON_USING_CPP17
  #define TYPHOON_LANGUAGE_STANDARD 17
#elif TYPHOON_USING_CPP14
  #define TYPHOON_LANGUAGE_STANDARD 14
#elif TYPHOON_USING_CPP11
  #define TYPHOON_LANGUAGE_STANDARD 11
#else
  #define TYPHOON_LANGUAGE_STANDARD 3
#endif

// NAN not defined or Rowley CrossWorks
#if !defined(NAN) || defined(__CROSSWORKS_ARM) || defined(TYPHOON_COMPILER_ARM5) || defined(ARDUINO)
  #define TYPHOON_NO_CPP_NAN_SUPPORT
#endif

#endif
