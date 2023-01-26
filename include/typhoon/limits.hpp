///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tpn
https://www.tpncpp.com

Copyright(c) 2018 John Wellbelove

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

#ifndef TYPHOON_LIMITS_HPP
#define TYPHOON_LIMITS_HPP

#include "platform.hpp"
#include "type_traits.hpp"
#include "char_traits.hpp"
#include "integral_limits.hpp"

#if TYPHOON_NOT_USING_STL && defined(TYPHOON_COMPILER_ARM5) && !defined(__USE_C99_MATH)
  // Required for nan, nanf, nanl
  #define __USE_C99_MATH
#endif

#include <limits.h>
#include <stdint.h>
#include <float.h>
#include <math.h>

#include "private/minmax_push.hpp"

#if defined(TYPHOON_COMPILER_MICROSOFT)
  #pragma warning(push)
  #pragma warning(disable : 26812)
#endif

#if TYPHOON_NOT_USING_STL
#define TYPHOON_LOG10_OF_2(x) (((x) * 301) / 1000)

#if !defined(LDBL_MIN) && defined(DBL_MIN)
  // Looks like we don't have these macros defined.
  // That probably means that 'long double' is the same size as 'double'.
  #define LDBL_MIN        DBL_MIN
  #define LDBL_MAX        DBL_MAX
  #define LDBL_EPSILON    DBL_EPSILON
  #define LDBL_MANT_DIG   DBL_MANT_DIG
  #define LDBL_DIG        DBL_DIG
  #define LDBL_MIN_EXP    DBL_MIN_EXP
  #define LDBL_MIN_10_EXP DBL_MIN_10_EXP
  #define LDBL_MAX_EXP    DBL_MAX_EXP
  #define LDBL_MAX_10_EXP DBL_MAX_10_EXP
#endif

#if !defined(HUGE_VAL)
  // Looks like we don't have these macros defined.
  // They're compiler implementation dependent, so we'll make them the same as the max values.
  #define HUGE_VALF FLT_MAX
  #define HUGE_VAL  DBL_MAX
  #define HUGE_VALL LDBL_MAX
#endif

#if defined(TYPHOON_NO_CPP_NAN_SUPPORT)
  #if defined(NAN)
    #define TYPHOON_NAN  (double)NAN
    #define TYPHOON_NANF (float)NAN
    #define TYPHOON_NANL (long double)NAN
    #define TYPHOON_HAS_NAN true
  #else
    #define TYPHOON_NAN  (double)0.0
    #define TYPHOON_NANF (float)0.0
    #define TYPHOON_NANL (long double)0.0
    #define TYPHOON_HAS_NAN false
  #endif
#else
  #define TYPHOON_NAN  nan("")
  #define TYPHOON_NANF nanf("")
  #define TYPHOON_NANL nanl("")
  #define TYPHOON_HAS_NAN true
#endif

namespace tpn
{
  enum float_round_style
  {
    round_indeterminate       = -1,
    round_toward_zero         = 0,
    round_to_nearest          = 1,
    round_toward_infinity     = 2,
    round_toward_neg_infinity = 3,
  };

  enum float_denorm_style
  {
    denorm_indeterminate = -1,
    denorm_absent        = 0,
    denorm_present       = 1
  };


  class tpn_integral_limits
  {
  public:

    static TYPHOON_CONSTANT bool is_specialized    = true;
    static TYPHOON_CONSTANT bool is_integer        = true;
    static TYPHOON_CONSTANT bool is_exact          = true;
    static TYPHOON_CONSTANT int  max_digits10      = 0;
    static TYPHOON_CONSTANT int  radix             = 2;
    static TYPHOON_CONSTANT int  min_exponent      = 0;
    static TYPHOON_CONSTANT int  min_exponent10    = 0;
    static TYPHOON_CONSTANT int  max_exponent      = 0;
    static TYPHOON_CONSTANT int  max_exponent10    = 0;
    static TYPHOON_CONSTANT bool has_infinity      = false;
    static TYPHOON_CONSTANT bool has_quiet_NaN     = false;
    static TYPHOON_CONSTANT bool has_signaling_NaN = false;
    static TYPHOON_CONSTANT bool has_denorm_loss   = false;
    static TYPHOON_CONSTANT bool is_iec559         = false;
    static TYPHOON_CONSTANT bool is_bounded        = true;
    static TYPHOON_CONSTANT bool traps             = false;
    static TYPHOON_CONSTANT bool tinyness_before   = false;
    static TYPHOON_CONSTANT float_denorm_style has_denorm = denorm_absent;
    static TYPHOON_CONSTANT float_round_style round_style = round_toward_zero;
  };

  class tpn_floating_point_limits
  {
  public:

    static TYPHOON_CONSTANT bool is_specialized    = true;
    static TYPHOON_CONSTANT bool is_signed         = true;
    static TYPHOON_CONSTANT bool is_integer        = false;
    static TYPHOON_CONSTANT bool is_exact          = false;
    static TYPHOON_CONSTANT int radix              = 2;
    static TYPHOON_CONSTANT bool has_infinity      = true;
    static TYPHOON_CONSTANT bool has_quiet_NaN     = TYPHOON_HAS_NAN;
    static TYPHOON_CONSTANT bool has_signaling_NaN = TYPHOON_HAS_NAN;
    static TYPHOON_CONSTANT bool has_denorm_loss   = false;
    static TYPHOON_CONSTANT bool is_iec559         = false;
    static TYPHOON_CONSTANT bool is_bounded        = true;
    static TYPHOON_CONSTANT bool is_modulo         = false;
    static TYPHOON_CONSTANT bool traps             = false;
    static TYPHOON_CONSTANT bool tinyness_before   = false;
    static TYPHOON_CONSTANT float_denorm_style has_denorm = denorm_indeterminate;
    static TYPHOON_CONSTANT float_round_style round_style = round_indeterminate;

    static float round_error() { return float(0.5); }
  };

  //***************************************************************************
  // Default
  template <typename T>
  class numeric_limits;

  template <typename T>
  class numeric_limits<const T> : public numeric_limits<T> { };

  template <typename T>
  class numeric_limits<volatile T> : public numeric_limits<T> { };

  template <typename T>
  class numeric_limits<const volatile T> : public numeric_limits<T> { };

  //***************************************************************************
  // bool
  template<>
  class numeric_limits<bool> : public tpn_integral_limits
  {
  public:

    static TYPHOON_CONSTANT int  digits    = 1;
    static TYPHOON_CONSTANT int  digits10  = 0;
    static TYPHOON_CONSTANT bool is_signed = false;
    static TYPHOON_CONSTANT bool is_modulo = false;

    static TYPHOON_CONSTEXPR bool min() { return false; }
    static TYPHOON_CONSTEXPR bool max() { return true; }
    static TYPHOON_CONSTEXPR bool lowest() { return false; }
    static TYPHOON_CONSTEXPR bool epsilon() { return false; }
    static TYPHOON_CONSTEXPR bool round_error() { return false; }
    static TYPHOON_CONSTEXPR bool denorm_min() { return false; }
    static TYPHOON_CONSTEXPR bool infinity() { return false; }
    static TYPHOON_CONSTEXPR bool quiet_NaN() { return false; }
    static TYPHOON_CONSTEXPR bool signaling_NaN() { return false; }
  };

  //***************************************************************************
  // char
  template<>
  class numeric_limits<char> : public tpn_integral_limits
  {
  public:

    static TYPHOON_CONSTANT int digits     = (CHAR_BIT * sizeof(char)) - (tpn::is_signed<char>::value ? 1 : 0);
    static TYPHOON_CONSTANT int digits10   = TYPHOON_LOG10_OF_2(digits);
    static TYPHOON_CONSTANT bool is_signed = tpn::is_signed<char>::value;
    static TYPHOON_CONSTANT bool is_modulo = false;

    static TYPHOON_CONSTEXPR char min() { return char(CHAR_MIN); }
    static TYPHOON_CONSTEXPR char max() { return char(CHAR_MAX); }
    static TYPHOON_CONSTEXPR char lowest() { return char(CHAR_MIN); }
    static TYPHOON_CONSTEXPR char epsilon() { return 0; }
    static TYPHOON_CONSTEXPR char round_error() { return 0; }
    static TYPHOON_CONSTEXPR char denorm_min() { return 0; }
    static TYPHOON_CONSTEXPR char infinity() { return 0; }
    static TYPHOON_CONSTEXPR char quiet_NaN() { return 0; }
    static TYPHOON_CONSTEXPR char signaling_NaN() { return 0; }
  };

  //***************************************************************************
  // unsigned char
  template<>
  class numeric_limits<unsigned char> : public tpn_integral_limits
  {
  public:

    static TYPHOON_CONSTANT int digits     = (CHAR_BIT * sizeof(unsigned char)) - (tpn::is_signed<unsigned char>::value ? 1 : 0);
    static TYPHOON_CONSTANT int digits10   = TYPHOON_LOG10_OF_2(digits);
    static TYPHOON_CONSTANT bool is_signed = false;
    static TYPHOON_CONSTANT bool is_modulo = true;

    static TYPHOON_CONSTEXPR unsigned char min() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned char max() { return UCHAR_MAX; }
    static TYPHOON_CONSTEXPR unsigned char lowest() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned char epsilon() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned char round_error() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned char denorm_min() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned char infinity() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned char quiet_NaN() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned char signaling_NaN() { return 0U; }
  };

  //***************************************************************************
  // signed char
  template<>
  class numeric_limits<signed char> : public tpn_integral_limits
  {
  public:

    static TYPHOON_CONSTANT int digits = (CHAR_BIT * sizeof(char)) - (tpn::is_signed<char>::value ? 1 : 0);
    static TYPHOON_CONSTANT int digits10 = TYPHOON_LOG10_OF_2(digits);
    static TYPHOON_CONSTANT bool is_signed = true;
    static TYPHOON_CONSTANT bool is_modulo = false;

    static TYPHOON_CONSTEXPR signed char min() { return SCHAR_MIN; }
    static TYPHOON_CONSTEXPR signed char max() { return SCHAR_MAX; }
    static TYPHOON_CONSTEXPR signed char lowest() { return SCHAR_MIN; }
    static TYPHOON_CONSTEXPR signed char epsilon() { return 0; }
    static TYPHOON_CONSTEXPR signed char round_error() { return 0; }
    static TYPHOON_CONSTEXPR signed char denorm_min() { return 0; }
    static TYPHOON_CONSTEXPR signed char infinity() { return 0; }
    static TYPHOON_CONSTEXPR signed char quiet_NaN() { return 0; }
    static TYPHOON_CONSTEXPR signed char signaling_NaN() { return 0; }
  };

#if TYPHOON_HAS_NATIVE_CHAR8_T
  //***************************************************************************
  // char8_t
  template<>
  class numeric_limits<char8_t> : public tpn_integral_limits
  {
  public:

    static TYPHOON_CONSTANT int digits = (CHAR_BIT * sizeof(char8_t)) - (tpn::is_signed<char8_t>::value ? 1 : 0);
    static TYPHOON_CONSTANT int digits10 = TYPHOON_LOG10_OF_2(digits);
    static TYPHOON_CONSTANT bool is_signed = tpn::is_signed<char8_t>::value;
    static TYPHOON_CONSTANT bool is_modulo = false;

    static TYPHOON_CONSTEXPR char8_t min() { return char8_t(CHAR_MIN); }
    static TYPHOON_CONSTEXPR char8_t max() { return char8_t(CHAR_MAX); }
    static TYPHOON_CONSTEXPR char8_t lowest() { return char8_t(CHAR_MIN); }
    static TYPHOON_CONSTEXPR char8_t epsilon() { return 0; }
    static TYPHOON_CONSTEXPR char8_t round_error() { return 0; }
    static TYPHOON_CONSTEXPR char8_t denorm_min() { return 0; }
    static TYPHOON_CONSTEXPR char8_t infinity() { return 0; }
    static TYPHOON_CONSTEXPR char8_t quiet_NaN() { return 0; }
    static TYPHOON_CONSTEXPR char8_t signaling_NaN() { return 0; }
  };
#endif

#if TYPHOON_HAS_NATIVE_CHAR16_T
  //***************************************************************************
  // char16_t
  template<>
  class numeric_limits<char16_t> : public tpn_integral_limits
  {
  public:

    static TYPHOON_CONSTANT int digits = (CHAR_BIT * sizeof(char16_t)) - (tpn::is_signed<char16_t>::value ? 1 : 0);
    static TYPHOON_CONSTANT int digits10 = TYPHOON_LOG10_OF_2(digits);
    static TYPHOON_CONSTANT bool is_signed = false;
    static TYPHOON_CONSTANT bool is_modulo = true;

    static TYPHOON_CONSTEXPR char16_t min() { return 0U; }
    static TYPHOON_CONSTEXPR char16_t max() { return UINT_LEAST16_MAX; }
    static TYPHOON_CONSTEXPR char16_t lowest() { return 0U; }
    static TYPHOON_CONSTEXPR char16_t epsilon() { return 0U; }
    static TYPHOON_CONSTEXPR char16_t round_error() { return 0U; }
    static TYPHOON_CONSTEXPR char16_t denorm_min() { return 0U; }
    static TYPHOON_CONSTEXPR char16_t infinity() { return 0U; }
    static TYPHOON_CONSTEXPR char16_t quiet_NaN() { return 0U; }
    static TYPHOON_CONSTEXPR char16_t signaling_NaN() { return 0U; }
  };
#endif

#if TYPHOON_HAS_NATIVE_CHAR32_T
  //***************************************************************************
  // char32_t
  template<>
  class numeric_limits<char32_t> : public tpn_integral_limits
  {
  public:

    static TYPHOON_CONSTANT int digits = (CHAR_BIT * sizeof(char32_t)) - (tpn::is_signed<char32_t>::value ? 1 : 0);
    static TYPHOON_CONSTANT int digits10 = TYPHOON_LOG10_OF_2(digits);
    static TYPHOON_CONSTANT bool is_signed = false;
    static TYPHOON_CONSTANT bool is_modulo = true;

    static TYPHOON_CONSTEXPR char32_t min() { return 0U; }
    static TYPHOON_CONSTEXPR char32_t max() { return UINT_LEAST32_MAX; }
    static TYPHOON_CONSTEXPR char32_t lowest() { return 0U; }
    static TYPHOON_CONSTEXPR char32_t epsilon() { return 0U; }
    static TYPHOON_CONSTEXPR char32_t round_error() { return 0U; }
    static TYPHOON_CONSTEXPR char32_t denorm_min() { return 0U; }
    static TYPHOON_CONSTEXPR char32_t infinity() { return 0U; }
    static TYPHOON_CONSTEXPR char32_t quiet_NaN() { return 0U; }
    static TYPHOON_CONSTEXPR char32_t signaling_NaN() { return 0U; }
  };
#endif

  //***************************************************************************
  // wchar_t
  template<>
  class numeric_limits<wchar_t> : public tpn_integral_limits
  {
  public:

    static TYPHOON_CONSTANT int digits = (CHAR_BIT * sizeof(wchar_t)) - (tpn::is_signed<wchar_t>::value ? 1 : 0);
    static TYPHOON_CONSTANT int digits10 = TYPHOON_LOG10_OF_2(digits);
    static TYPHOON_CONSTANT bool is_signed = tpn::is_signed<wchar_t>::value;
    static TYPHOON_CONSTANT bool is_modulo = tpn::is_unsigned<wchar_t>::value;
    static TYPHOON_CONSTEXPR wchar_t min() { return WCHAR_MIN; }
    static TYPHOON_CONSTEXPR wchar_t max() { return WCHAR_MAX; }
    static TYPHOON_CONSTEXPR wchar_t lowest() { return WCHAR_MIN; }
    static TYPHOON_CONSTEXPR wchar_t epsilon() { return wchar_t(0); }
    static TYPHOON_CONSTEXPR wchar_t round_error() { return wchar_t(0); }
    static TYPHOON_CONSTEXPR wchar_t denorm_min() { return wchar_t(0); }
    static TYPHOON_CONSTEXPR wchar_t infinity() { return wchar_t(0); }
    static TYPHOON_CONSTEXPR wchar_t quiet_NaN() { return wchar_t(0); }
    static TYPHOON_CONSTEXPR wchar_t signaling_NaN() { return wchar_t(0); }
  };

  //***************************************************************************
  // short
  template<>
  class numeric_limits<short> : public tpn_integral_limits
  {
  public:

    static TYPHOON_CONSTANT int digits = (CHAR_BIT * sizeof(short)) - (tpn::is_signed<short>::value ? 1 : 0);
    static TYPHOON_CONSTANT int digits10 = TYPHOON_LOG10_OF_2(digits);
    static TYPHOON_CONSTANT bool is_signed = true;
    static TYPHOON_CONSTANT bool is_modulo = false;

    static TYPHOON_CONSTEXPR short min() { return SHRT_MIN; }
    static TYPHOON_CONSTEXPR short max() { return SHRT_MAX; }
    static TYPHOON_CONSTEXPR short lowest() { return SHRT_MIN; }
    static TYPHOON_CONSTEXPR short epsilon() { return 0; }
    static TYPHOON_CONSTEXPR short round_error() { return 0; }
    static TYPHOON_CONSTEXPR short denorm_min() { return 0; }
    static TYPHOON_CONSTEXPR short infinity() { return 0; }
    static TYPHOON_CONSTEXPR short quiet_NaN() { return 0; }
    static TYPHOON_CONSTEXPR short signaling_NaN() { return 0; }
  };

  //***************************************************************************
  // unsigned short
  template<>
  class numeric_limits<unsigned short> : public tpn_integral_limits
  {
  public:

    static TYPHOON_CONSTANT int digits = (CHAR_BIT * sizeof(unsigned short)) - (tpn::is_signed<unsigned short>::value ? 1 : 0);
    static TYPHOON_CONSTANT int digits10 = TYPHOON_LOG10_OF_2(digits);
    static TYPHOON_CONSTANT bool is_signed = false;
    static TYPHOON_CONSTANT bool is_modulo = true;

    static TYPHOON_CONSTEXPR unsigned short min() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned short max() { return USHRT_MAX; }
    static TYPHOON_CONSTEXPR unsigned short lowest() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned short epsilon() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned short round_error() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned short denorm_min() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned short infinity() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned short quiet_NaN() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned short signaling_NaN() { return 0U; }

  };

  //***************************************************************************
  // int
  template<>
  class numeric_limits<int> : public tpn_integral_limits
  {
  public:

    static TYPHOON_CONSTANT int digits = (CHAR_BIT * sizeof(int)) - (tpn::is_signed<int>::value ? 1 : 0);
    static TYPHOON_CONSTANT int digits10 = TYPHOON_LOG10_OF_2(digits);
    static TYPHOON_CONSTANT bool is_signed = true;
    static TYPHOON_CONSTANT bool is_modulo = false;

    static TYPHOON_CONSTEXPR int min() { return INT_MIN; }
    static TYPHOON_CONSTEXPR int max() { return INT_MAX; }
    static TYPHOON_CONSTEXPR int lowest() { return INT_MIN; }
    static TYPHOON_CONSTEXPR int epsilon() { return 0; }
    static TYPHOON_CONSTEXPR int round_error() { return 0; }
    static TYPHOON_CONSTEXPR int denorm_min() { return 0; }
    static TYPHOON_CONSTEXPR int infinity() { return 0; }
    static TYPHOON_CONSTEXPR int quiet_NaN() { return 0; }
    static TYPHOON_CONSTEXPR int signaling_NaN() { return 0; }
  };

  //***************************************************************************
  // unsigned int
  template<>
  class numeric_limits<unsigned int> : public tpn_integral_limits
  {
  public:

    static TYPHOON_CONSTANT int digits = (CHAR_BIT * sizeof(unsigned int)) - (tpn::is_signed<unsigned int>::value ? 1 : 0);
    static TYPHOON_CONSTANT int digits10 = TYPHOON_LOG10_OF_2(digits);
    static TYPHOON_CONSTANT bool is_signed = false;
    static TYPHOON_CONSTANT bool is_modulo = true;

    static TYPHOON_CONSTEXPR unsigned int min() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned int max() { return UINT_MAX; }
    static TYPHOON_CONSTEXPR unsigned int lowest() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned int epsilon() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned int round_error() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned int denorm_min() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned int infinity() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned int quiet_NaN() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned int signaling_NaN() { return 0U; }
  };

  //***************************************************************************
  // long
  template<>
  class numeric_limits<long> : public tpn_integral_limits
  {
  public:

    static TYPHOON_CONSTANT int digits = (CHAR_BIT * sizeof(long)) - (tpn::is_signed<long>::value ? 1 : 0);
    static TYPHOON_CONSTANT int digits10 = TYPHOON_LOG10_OF_2(digits);
    static TYPHOON_CONSTANT bool is_signed = true;
    static TYPHOON_CONSTANT bool is_modulo = false;

    static TYPHOON_CONSTEXPR long min() { return LONG_MIN; }
    static TYPHOON_CONSTEXPR long max() { return LONG_MAX; }
    static TYPHOON_CONSTEXPR long lowest() { return LONG_MIN; }
    static TYPHOON_CONSTEXPR long epsilon() { return 0; }
    static TYPHOON_CONSTEXPR long round_error() { return 0; }
    static TYPHOON_CONSTEXPR long denorm_min() { return 0; }
    static TYPHOON_CONSTEXPR long infinity() { return 0; }
    static TYPHOON_CONSTEXPR long quiet_NaN() { return 0; }
    static TYPHOON_CONSTEXPR long signaling_NaN() { return 0; }
  };

  //***************************************************************************
  // unsigned long
  template<>
  class numeric_limits<unsigned long> : public tpn_integral_limits
  {
  public:

    static TYPHOON_CONSTANT int digits = (CHAR_BIT * sizeof(unsigned long)) - (tpn::is_signed<unsigned long>::value ? 1 : 0);
    static TYPHOON_CONSTANT int digits10 = TYPHOON_LOG10_OF_2(digits);
    static TYPHOON_CONSTANT bool is_signed = false;
    static TYPHOON_CONSTANT bool is_modulo = true;

    static TYPHOON_CONSTEXPR unsigned long min() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned long max() { return ULONG_MAX; }
    static TYPHOON_CONSTEXPR unsigned long lowest() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned long epsilon() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned long round_error() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned long denorm_min() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned long infinity() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned long quiet_NaN() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned long signaling_NaN() { return 0U; }
  };

  //***************************************************************************
  // long long
  template<>
  class numeric_limits<long long> : public tpn_integral_limits
  {
  public:

    static TYPHOON_CONSTANT int digits = (CHAR_BIT * sizeof(long long)) - (tpn::is_signed<long long>::value ? 1 : 0);
    static TYPHOON_CONSTANT int digits10 = TYPHOON_LOG10_OF_2(digits);
    static TYPHOON_CONSTANT bool is_signed = true;
    static TYPHOON_CONSTANT bool is_modulo = false;

    static TYPHOON_CONSTEXPR long long min() { return LLONG_MIN; }
    static TYPHOON_CONSTEXPR long long max() { return LLONG_MAX; }
    static TYPHOON_CONSTEXPR long long lowest() { return LLONG_MIN; }
    static TYPHOON_CONSTEXPR long long epsilon() { return 0; }
    static TYPHOON_CONSTEXPR long long round_error() { return 0; }
    static TYPHOON_CONSTEXPR long long denorm_min() { return 0; }
    static TYPHOON_CONSTEXPR long long infinity() { return 0; }
    static TYPHOON_CONSTEXPR long long quiet_NaN() { return 0; }
    static TYPHOON_CONSTEXPR long long signaling_NaN() { return 0; }
  };

  //***************************************************************************
  // unsigned long long
  template<>
  class numeric_limits<unsigned long long> : public tpn_integral_limits
  {
  public:

    static TYPHOON_CONSTANT int digits = (CHAR_BIT * sizeof(unsigned long long)) - (tpn::is_signed<unsigned long long>::value ? 1 : 0);
    static TYPHOON_CONSTANT int digits10 = TYPHOON_LOG10_OF_2(digits);
    static TYPHOON_CONSTANT bool is_signed = false;
    static TYPHOON_CONSTANT bool is_modulo = true;

    static TYPHOON_CONSTEXPR unsigned long long min() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned long long max() { return ULLONG_MAX; }
    static TYPHOON_CONSTEXPR unsigned long long lowest() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned long long epsilon() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned long long round_error() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned long long denorm_min() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned long long infinity() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned long long quiet_NaN() { return 0U; }
    static TYPHOON_CONSTEXPR unsigned long long signaling_NaN() { return 0U; }
  };

  //***************************************************************************
  // float
  template<>
  class numeric_limits<float> : public tpn_floating_point_limits
  {
  public:

    static TYPHOON_CONSTEXPR float min() { return FLT_MIN; }
    static TYPHOON_CONSTEXPR float max() { return FLT_MAX; }
    static TYPHOON_CONSTEXPR float lowest() { return -FLT_MAX; }
    static TYPHOON_CONSTEXPR float epsilon() { return FLT_EPSILON; }
    static TYPHOON_CONSTEXPR float denorm_min() { return FLT_MIN; }
    static TYPHOON_CONSTEXPR float infinity() { return HUGE_VALF; }
    static float quiet_NaN() { return TYPHOON_NANF; }
    static float signaling_NaN() { return TYPHOON_NANF; }

    static TYPHOON_CONSTANT int digits       = FLT_MANT_DIG;
    static TYPHOON_CONSTANT int digits10     = FLT_DIG;
    static TYPHOON_CONSTANT int max_digits10 = TYPHOON_LOG10_OF_2(FLT_MANT_DIG) + 2;

    static TYPHOON_CONSTANT int min_exponent   = FLT_MIN_EXP;
    static TYPHOON_CONSTANT int min_exponent10 = FLT_MIN_10_EXP;
    static TYPHOON_CONSTANT int max_exponent   = FLT_MAX_EXP;
    static TYPHOON_CONSTANT int max_exponent10 = FLT_MAX_10_EXP;
  };

  //***************************************************************************
  // double
  template<>
  class numeric_limits<double> : public tpn_floating_point_limits
  {
  public:

    static TYPHOON_CONSTEXPR double min() { return DBL_MIN; }
    static TYPHOON_CONSTEXPR double max() { return DBL_MAX; }
    static TYPHOON_CONSTEXPR double lowest() { return -DBL_MAX; }
    static TYPHOON_CONSTEXPR double epsilon() { return DBL_EPSILON; }
    static TYPHOON_CONSTEXPR double denorm_min() { return DBL_MIN; }
    static TYPHOON_CONSTEXPR double infinity() { return HUGE_VAL; }
    static double quiet_NaN() { return TYPHOON_NAN; }
    static double signaling_NaN() { return TYPHOON_NAN; }

    static TYPHOON_CONSTANT int digits       = DBL_MANT_DIG;
    static TYPHOON_CONSTANT int digits10     = DBL_DIG;
    static TYPHOON_CONSTANT int max_digits10 = TYPHOON_LOG10_OF_2(DBL_MANT_DIG) + 2;

    static TYPHOON_CONSTANT int min_exponent   = DBL_MIN_EXP;
    static TYPHOON_CONSTANT int min_exponent10 = DBL_MIN_10_EXP;
    static TYPHOON_CONSTANT int max_exponent   = DBL_MAX_EXP;
    static TYPHOON_CONSTANT int max_exponent10 = DBL_MAX_10_EXP;
  };

  //***************************************************************************
  // long double
  template<>
  class numeric_limits<long double> : public tpn_floating_point_limits
  {
  public:

    static TYPHOON_CONSTEXPR long double min() { return LDBL_MIN; }
    static TYPHOON_CONSTEXPR long double max() { return LDBL_MAX; }
    static TYPHOON_CONSTEXPR long double lowest() { return -LDBL_MAX; }
    static TYPHOON_CONSTEXPR long double epsilon() { return LDBL_EPSILON; }
    static TYPHOON_CONSTEXPR long double denorm_min() { return LDBL_MIN; }
    static TYPHOON_CONSTEXPR long double infinity() { return HUGE_VALL; }
    static long double quiet_NaN() { return TYPHOON_NANL; }
    static long double signaling_NaN() { return TYPHOON_NANL; }

    static TYPHOON_CONSTANT int digits       = LDBL_MANT_DIG;
    static TYPHOON_CONSTANT int digits10     = LDBL_DIG;
    static TYPHOON_CONSTANT int max_digits10 = TYPHOON_LOG10_OF_2(LDBL_MANT_DIG) + 2;

    static TYPHOON_CONSTANT int min_exponent   = LDBL_MIN_EXP;
    static TYPHOON_CONSTANT int min_exponent10 = LDBL_MIN_10_EXP;
    static TYPHOON_CONSTANT int max_exponent   = LDBL_MAX_EXP;
    static TYPHOON_CONSTANT int max_exponent10 = LDBL_MAX_10_EXP;
  };
}

#else

#include <limits>

namespace tpn
{
  enum float_round_style
  {
    round_indeterminate       = std::round_indeterminate,
    round_toward_zero         = std::round_toward_zero,
    round_to_nearest          = std::round_to_nearest,
    round_toward_infinity     = std::round_toward_infinity,
    round_toward_neg_infinity = std::round_toward_neg_infinity,
  };

  enum float_denorm_style
  {
    denorm_indeterminate = std::denorm_indeterminate,
    denorm_absent        = std::denorm_absent,
    denorm_present       = std::denorm_present
  };

  template <typename T>
  class numeric_limits : public std::numeric_limits<T>
  {
  };
}
#endif

#if defined(TYPHOON_COMPILER_MICROSOFT)
  #pragma warning(pop)
#endif

#include "private/minmax_pop.hpp"

#endif
