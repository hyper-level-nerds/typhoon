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

#ifndef TYPHOON_INTEGRAL_LIMITS_INCLUDED
#define TYPHOON_INTEGRAL_LIMITS_INCLUDED

#include "platform.h"
#include "type_traits.h"

#include <stddef.h>
#include <limits.h>

#include "private/minmax_push.h"

//*****************************************************************************
///\defgroup integral_limits integral_limits
/// A set of templated compile time constants that mirror some of std::numeric_limits functionality.
///\ingroup utilities
//*****************************************************************************

namespace tphn
{
  //***************************************************************************
  ///\ingroup integral_limits
  //***************************************************************************
  template <typename T>
  struct integral_limits;

  //***************************************************************************
  ///\ingroup integral_limits
  //***************************************************************************
  template <>
  struct integral_limits<void>
  {
    static TYPHOON_CONSTANT int min        = 0;
    static TYPHOON_CONSTANT int max        = 0;
    static TYPHOON_CONSTANT int bits       = 0;
    static TYPHOON_CONSTANT bool is_signed = false;
  };

  //***************************************************************************
  ///\ingroup integral_limits
  //***************************************************************************
  template <>
  struct integral_limits<signed char>
  {
    static TYPHOON_CONSTANT signed char min = SCHAR_MIN;
    static TYPHOON_CONSTANT signed char max = SCHAR_MAX;
    static TYPHOON_CONSTANT int bits        = CHAR_BIT;
    static TYPHOON_CONSTANT bool is_signed  = tphn::is_signed<signed char>::value;
  };

  //***************************************************************************
  ///\ingroup integral_limits
  //***************************************************************************
  template <>
  struct integral_limits<unsigned char>
  {
    static TYPHOON_CONSTANT unsigned char min = 0;
    static TYPHOON_CONSTANT unsigned char max = UCHAR_MAX;
    static TYPHOON_CONSTANT int bits          = CHAR_BIT;
    static TYPHOON_CONSTANT bool is_signed    = tphn::is_signed<unsigned char>::value;
  };

  //***************************************************************************
  ///\ingroup integral_limits
  //***************************************************************************
#if defined(TYPHOON_COMPILER_MICROSOFT)
  #pragma warning(push)
  #pragma warning(disable : 4309)
#endif
  template <>
  struct integral_limits<char>
  {
    static TYPHOON_CONSTANT char min       = (tphn::is_signed<char>::value) ? SCHAR_MIN : 0;
    static TYPHOON_CONSTANT char max       = (tphn::is_signed<char>::value) ? SCHAR_MAX : static_cast<char>(UCHAR_MAX);
    static TYPHOON_CONSTANT int bits       = CHAR_BIT;
    static TYPHOON_CONSTANT bool is_signed = tphn::is_signed<char>::value;
  };
#if defined(TYPHOON_COMPILER_MICROSOFT)
  #pragma warning(pop)
#endif

  //***************************************************************************
  ///\ingroup integral_limits
  //***************************************************************************
  template <>
  struct integral_limits<short>
  {
    static TYPHOON_CONSTANT short min      = SHRT_MIN;
    static TYPHOON_CONSTANT short max      = SHRT_MAX;
    static TYPHOON_CONSTANT int bits       = CHAR_BIT * (sizeof(short) / sizeof(char));
    static TYPHOON_CONSTANT bool is_signed = tphn::is_signed<short>::value;
  };

  //***************************************************************************
  ///\ingroup integral_limits
  //***************************************************************************
  template <>
  struct integral_limits<unsigned short>
  {
    static TYPHOON_CONSTANT unsigned short min = 0;
    static TYPHOON_CONSTANT unsigned short max = USHRT_MAX;
    static TYPHOON_CONSTANT int bits           = CHAR_BIT * (sizeof(unsigned short) / sizeof(char));
    static TYPHOON_CONSTANT bool is_signed     = tphn::is_signed<unsigned short>::value;
  };

  //***************************************************************************
  ///\ingroup integral_limits
  //***************************************************************************
  template <>
  struct integral_limits<int>
  {
    static TYPHOON_CONSTANT int min        = INT_MIN;
    static TYPHOON_CONSTANT int max        = INT_MAX;
    static TYPHOON_CONSTANT int bits       = CHAR_BIT * (sizeof(int) / sizeof(char));
    static TYPHOON_CONSTANT bool is_signed = tphn::is_signed<int>::value;
  };

  //***************************************************************************
  ///\ingroup integral_limits
  //***************************************************************************
  template <>
  struct integral_limits<unsigned int>
  {
    static TYPHOON_CONSTANT unsigned int min = 0;
    static TYPHOON_CONSTANT unsigned int max = UINT_MAX;
    static TYPHOON_CONSTANT int bits         = CHAR_BIT * (sizeof(unsigned int) / sizeof(char));
    static TYPHOON_CONSTANT bool is_signed   = tphn::is_signed<unsigned int>::value;
  };

  //***************************************************************************
  ///\ingroup integral_limits
  //***************************************************************************
  template <>
  struct integral_limits<long>
  {
    static TYPHOON_CONSTANT long min       = LONG_MIN;
    static TYPHOON_CONSTANT long max       = LONG_MAX;
    static TYPHOON_CONSTANT int bits       = CHAR_BIT * (sizeof(long) / sizeof(char));
    static TYPHOON_CONSTANT bool is_signed = tphn::is_signed<long>::value;
  };

  //***************************************************************************
  ///\ingroup integral_limits
  //***************************************************************************
  template <>
  struct integral_limits<unsigned long>
  {
    static TYPHOON_CONSTANT unsigned long min = 0;
    static TYPHOON_CONSTANT unsigned long max = ULONG_MAX;
    static TYPHOON_CONSTANT int bits          = CHAR_BIT * (sizeof(unsigned long) / sizeof(char));
    static TYPHOON_CONSTANT bool is_signed    = tphn::is_signed<unsigned long>::value;
  };

#ifndef LLONG_MAX
#define LLONG_MAX	9223372036854775807LL
#endif

#ifndef LLONG_MIN
#define LLONG_MIN	(-LLONG_MAX - 1LL)
#endif

#ifndef ULLONG_MAX
#define ULLONG_MAX 18446744073709551615ULL
#endif

  //***************************************************************************
  ///\ingroup integral_limits
  //***************************************************************************
  template <>
  struct integral_limits<long long>
  {
    static TYPHOON_CONSTANT long long min  = LLONG_MIN;
    static TYPHOON_CONSTANT long long max  = LLONG_MAX;
    static TYPHOON_CONSTANT int bits       = CHAR_BIT * (sizeof(long long) / sizeof(char));
    static TYPHOON_CONSTANT bool is_signed = tphn::is_signed<long long>::value;
  };

  //***************************************************************************
  ///\ingroup integral_limits
  //***************************************************************************
  template <>
  struct integral_limits<unsigned long long>
  {
    static TYPHOON_CONSTANT unsigned long long min = 0;
    static TYPHOON_CONSTANT unsigned long long max = ULLONG_MAX;
    static TYPHOON_CONSTANT int bits               = CHAR_BIT * (sizeof(unsigned long long) / sizeof(char));
    static TYPHOON_CONSTANT bool is_signed         = tphn::is_signed<unsigned long long>::value;
  };

  static TYPHOON_CONSTANT size_t npos = tphn::integral_limits<size_t>::max;
}

#include "private/minmax_pop.h"

#endif
