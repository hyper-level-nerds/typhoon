///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tphn
https://www.tphncpp.com

Copyright(c) 2014 John Wellbelove

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

#ifndef TYPHOON_ENDIAN_HPP
#define TYPHOON_ENDIAN_HPP

#include "platform.hpp"
#include "enum_type.hpp"
#include "binary.hpp"

#include <stdint.h>

#if TYPHOON_USING_CPP20 && TYPHOON_USING_STL
  #include <bit>
#endif

///\defgroup endian endian
/// Constants & utilities for endianness
///\ingroup utilities

// Have we not already defined TYPHOON_ENDIAN_NATIVE?
#if !defined(TYPHOON_ENDIAN_NATIVE)
  // Can we use the C++20 definitions?
  #if TYPHOON_USING_CPP20 && TYPHOON_USING_STL
    #define TYPHOON_ENDIAN_LITTLE std::endian::little
    #define TYPHOON_ENDIAN_BIG    std::endian::big
    #define TYPHOON_ENDIAN_NATIVE std::endian::native
  // Is this the IAR compiler?
  #elif defined(TYPHOON_COMPILER_IAR) && defined(__LITTLE_ENDIAN__)
    #define TYPHOON_ENDIAN_LITTLE 0
    #define TYPHOON_ENDIAN_BIG    1
    #if __LITTLE_ENDIAN__ == 1
      #define TYPHOON_ENDIAN_NATIVE TYPHOON_ENDIAN_LITTLE
    #elif __LITTLE_ENDIAN__ == 0
      #define TYPHOON_ENDIAN_NATIVE TYPHOON_ENDIAN_BIG
    #endif
  // If not can we use the compiler macros?
  #elif defined(__BYTE_ORDER__)
    // Test the two versions of the macro we are likely to see.
    #if defined(__ORDER_LITTLE_ENDIAN__)
      #define TYPHOON_ENDIAN_LITTLE __ORDER_LITTLE_ENDIAN__
      #define TYPHOON_ENDIAN_BIG    __ORDER_BIG_ENDIAN__
      #define TYPHOON_ENDIAN_NATIVE __BYTE_ORDER__
    #elif defined(__LITTLE_ENDIAN__)
      #define TYPHOON_ENDIAN_LITTLE __LITTLE_ENDIAN__
      #define TYPHOON_ENDIAN_BIG    __BIG_ENDIAN__
      #define TYPHOON_ENDIAN_NATIVE __BYTE_ORDER__
    #endif
  #else
    // The user needs to define TYPHOON_ENDIAN_NATIVE.
    #error Unable to determine native endianness at compile time. TYPHOON_ENDIAN_NATIVE must be defined either as 0 for 'little endian' or 1 for 'big endian'.
  #endif
#else
  // Default values for little and big endianness.
  #define TYPHOON_ENDIAN_LITTLE 0
  #define TYPHOON_ENDIAN_BIG    1
#endif

// If true, then the endianness of the platform can be constexpr.
#if (TYPHOON_USING_CPP11 && defined(TYPHOON_ENDIAN_NATIVE))
  #define TYPHOON_HAS_CONSTEXPR_ENDIANNESS 1
#else
  #define TYPHOON_HAS_CONSTEXPR_ENDIANNESS 0
#endif

namespace tphn
{
  //***************************************************************************
  /// Constants to denote endianness of operations.
  ///\ingroup endian
  //***************************************************************************
  struct endian
  {
    enum enum_type
    {
      little  = static_cast<int>(TYPHOON_ENDIAN_LITTLE),
      big     = static_cast<int>(TYPHOON_ENDIAN_BIG),
      native  = static_cast<int>(TYPHOON_ENDIAN_NATIVE),
    };

    TYPHOON_DECLARE_ENUM_TYPE(endian, int)
    TYPHOON_ENUM_TYPE(little,  "little")
    TYPHOON_ENUM_TYPE(big,     "big")
    TYPHOON_END_ENUM_TYPE
  };

  //***************************************************************************
  /// Checks the endianness of the platform.
  ///\ingroup endian
  //***************************************************************************
  struct endianness
  {
    tphn::endian operator ()() const
    {
      return tphn::endian(*this);
    }

#if TYPHOON_HAS_CONSTEXPR_ENDIANNESS
    TYPHOON_CONSTEXPR
#endif
    operator tphn::endian() const
    {
      return get();
    }

#if TYPHOON_HAS_CONSTEXPR_ENDIANNESS
    static TYPHOON_CONSTEXPR tphn::endian value()
#else
    static tphn::endian value()
#endif
    {
      return get();
    }

  private:

#if TYPHOON_HAS_CONSTEXPR_ENDIANNESS
    static TYPHOON_CONSTEXPR tphn::endian get()
    {
      return tphn::endian::native;
    }
#else
    static tphn::endian get()
    {
      static const uint32_t i = 0xFFFF0000;

      return (*reinterpret_cast<const unsigned char*>(&i) == 0) ? tphn::endian::little : tphn::endian::big;
    }
#endif
  };

  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR14
    typename tphn::enable_if<tphn::is_integral<T>::value, T>::type
    ntoh(T value)
  {
    if (endianness::value() == endian::little)
    {
      return tphn::reverse_bytes(value);
    }
    else
    {
      return value;
    }
  }

  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR14
    typename tphn::enable_if<tphn::is_integral<T>::value, T>::type
    hton(T value)
  {
    if (endianness::value() == endian::little)
    {
      return tphn::reverse_bytes(value);
    }
    else
    {
      return value;
    }
  }
}

#endif
