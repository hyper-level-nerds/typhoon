///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tpn
https://www.tpncpp.com

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

#if 0
#error THIS HEADER IS A GENERATOR. DO NOT INCLUDE.
#endif

//***************************************************************************
// THIS FILE HAS BEEN AUTO GENERATED. DO NOT EDIT THIS FILE.
//***************************************************************************

//***************************************************************************
// To generate to header file, run this at the command line.
// Note: You will need Python and COG installed.
//
// python -m cogapp -d -e -osmallest.h -DNTypes=<n> smallest_generator.h
// Where <n> is the number of types to support.
//
// e.g.
// To generate handlers for up to 16 types...
// python -m cogapp -d -e -osmallest.h -DNTypes=16 smallest_generator.h
//
// See generate.bat
//***************************************************************************

#ifndef TYPHOON_SMALLEST_HPP
#define TYPHOON_SMALLEST_HPP

#include "platform.hpp"
#include "integral_limits.hpp"

#include <stdint.h>

///\defgroup smallest smallest
///\ingroup utilities

namespace tpn
{
#if TYPHOON_USING_CPP11 && !defined(TYPHOON_SMALLEST_TYPE_FORCE_CPP03_IMPLEMENTATION)
  //***************************************************************************
  /// Template to determine the largest type and size.
  /// Defines 'value_type' which is the type of the largest parameter.
  /// Defines 'size' which is the size of the largest parameter.
  ///\ingroup largest
  //***************************************************************************
  template <typename T1, typename... TRest>
  class smallest_type
  {
  private:

    // Define 'smallest_other' as 'smallest_type' with all but the first parameter.
    using smallest_other = typename smallest_type<TRest...>::type;

  public:

    // Set 'type' to be the smallest of the first parameter and any of the others.
    // This is recursive.
    using type = typename tpn::conditional<(tpn::size_of<T1>::value < tpn::size_of<smallest_other>::value), // Boolean
                                            T1,                                                             // TrueType
                                            smallest_other>                                                 // FalseType
                                            ::type;                                                         // The smallest type of the two.

    // The size of the smallest type.
    enum
    {
      size = tpn::size_of<type>::value
    };
  };

  //***************************************************************************
  // Specialisation for one template parameter.
  //***************************************************************************
  template <typename T1>
  class smallest_type<T1>
  {
  public:

    using type = T1;

    enum
    {
      size = tpn::size_of<type>::value
    };
  };

#if TYPHOON_USING_CPP11
  template <typename... T>
  using smallest_type_t = typename smallest_type<T...>::type;
#endif

#if TYPHOON_USING_CPP17
  template <typename... T>
  constexpr size_t smallest_type_v = smallest_type<T...>::size;
#endif

#else
  //***************************************************************************
  /// Template to determine the smallest type and size.
  /// Supports up to 16 types.
  /// Defines 'value_type' which is the type of the smallest parameter.
  /// Defines 'size' which is the size of the smallest parameter.
  ///\ingroup smallest
  //***************************************************************************
  template <typename T1, typename T2 = void, typename T3 = void, typename T4 = void, 
            typename T5 = void, typename T6 = void, typename T7 = void, typename T8 = void, 
            typename T9 = void, typename T10 = void, typename T11 = void, typename T12 = void, 
            typename T13 = void, typename T14 = void, typename T15 = void, typename T16 = void>
  struct smallest_type
  {
  private:

    // Declaration.
    template <const bool Boolean, typename TrueType, typename FalseType>
    struct choose_type;

    // Specialisation for 'true'.
    // Defines 'type' as 'TrueType'.
    template <typename TrueType, typename FalseType>
    struct choose_type<true, TrueType, FalseType>
    {
      typedef TrueType type;
    };

    // Specialisation for 'false'. 
    // Defines 'type' as 'FalseType'.
    template <typename TrueType, typename FalseType>
    struct choose_type<false, TrueType, FalseType>
    {
      typedef FalseType type;
    };

  public:

    // Define 'smallest_other' as 'smallest_type' with all but the first parameter. 
    typedef typename smallest_type<T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::type smallest_other;

    // Set 'type' to be the smallest of the first parameter and any of the others.
    // This is recursive.
    typedef typename choose_type<(sizeof(T1) < sizeof(smallest_other)), // Boolean
                                  T1,                                   // TrueType
                                  smallest_other>                       // FalseType
                                  ::type type;                          // The smallest type of the two.

    // The size of the smallest type.
    enum
    {
      size = sizeof(type)
    };
  };

  //***************************************************************************
  // Specialisation for one template parameter.
  //***************************************************************************
  template <typename T1>
  struct smallest_type<T1,   void, void, void, void, void, void, void, 
                       void, void, void, void, void, void, void, void>
  {
    typedef T1 type;

    enum
    {
      size = sizeof(type)
    };
  };
#endif

  namespace private_smallest
  {
    //*************************************************************************
    // Determine the type to hold the number of bits based on the index.
    //*************************************************************************
    template <const int index>
    struct best_fit_uint_type;

    //*************************************************************************
    // Less than or equal to 8 bits.
    //*************************************************************************
    template <>
    struct best_fit_uint_type<0>
    {
      typedef uint_least8_t type;
    };

    //*************************************************************************
    // 9 to 16 bits.
    //*************************************************************************
    template <>
    struct best_fit_uint_type<1>
    {
      typedef uint_least16_t type;
    };

    //*************************************************************************
    // 17 to 31 bits.
    //*************************************************************************
    template <>
    struct best_fit_uint_type<2>
    {
      typedef uint_least32_t type;
    };

#if TYPHOON_USING_64BIT_TYPES
    //*************************************************************************
    // Greater than 32 bits.
    //*************************************************************************
    template <>
    struct best_fit_uint_type<3>
    {
      typedef uint_least64_t type;
    };
#endif

    //*************************************************************************
    // Determine the type to hold the number of bits based on the index.
    //*************************************************************************
    template <const int index>
    struct best_fit_int_type;

    //*************************************************************************
    // Less than or equal to 8 bits.
    //*************************************************************************
    template <>
    struct best_fit_int_type<0>
    {
      typedef int_least8_t type;
    };

    //*************************************************************************
    // 9 to 16 bits.
    //*************************************************************************
    template <>
    struct best_fit_int_type<1>
    {
      typedef int_least16_t type;
    };

    //*************************************************************************
    // 17 to 31 bits.
    //*************************************************************************
    template <>
    struct best_fit_int_type<2>
    {
      typedef int_least32_t type;
    };

#if TYPHOON_USING_64BIT_TYPES
    //*************************************************************************
    // Greater than 32 bits.
    //*************************************************************************
    template <>
    struct best_fit_int_type<3>
    {
      typedef int_least64_t type;
    };
#endif
  }

  //***************************************************************************
  /// Template to determine the smallest unsigned int type that can contain a
  /// value with the specified number of bits.
  /// Defines 'type' which is the type of the smallest unsigned integer.
  ///\ingroup smallest
  //***************************************************************************
  template <size_t NBITS>
  struct smallest_uint_for_bits
  {
  private:

    // Determines the index of the best unsigned type for the required number of bits.
    static TYPHOON_CONSTANT int TYPE_INDEX = ((NBITS >  8) ? 1 : 0) +
                                         ((NBITS > 16) ? 1 : 0) +
                                         ((NBITS > 32) ? 1 : 0);

  public:

    typedef typename private_smallest::best_fit_uint_type<TYPE_INDEX>::type type;
  };

#if TYPHOON_USING_CPP11
  template <size_t NBITS>
  using smallest_uint_for_bits_t = typename smallest_uint_for_bits<NBITS>::type;
#endif

  //***************************************************************************
  /// Template to determine the smallest signed int type that can contain a
  /// value with the specified number of bits.
  /// Defines 'type' which is the type of the smallest signed integer.
  ///\ingroup smallest
  //***************************************************************************
  template <size_t NBITS>
  struct smallest_int_for_bits
  {
  private:

    // Determines the index of the best unsigned type for the required number of bits.
    static TYPHOON_CONSTANT int TYPE_INDEX = ((NBITS >  8) ? 1 : 0) +
                                         ((NBITS > 16) ? 1 : 0) +
                                         ((NBITS > 32) ? 1 : 0);

  public:

    typedef typename private_smallest::best_fit_int_type<TYPE_INDEX>::type type;
  };

#if TYPHOON_USING_CPP11
  template <size_t NBITS>
  using smallest_int_for_bits_t = typename smallest_int_for_bits<NBITS>::type;
#endif

  //***************************************************************************
  /// Template to determine the smallest unsigned int type that can contain the
  /// specified unsigned value.
  /// Defines 'type' which is the type of the smallest unsigned integer.
  ///\ingroup smallest
  //***************************************************************************
  template <uintmax_t VALUE>
  struct smallest_uint_for_value
  {
  private:

    // Determines the index of the best unsigned type for the required value.
    static TYPHOON_CONSTANT int TYPE_INDEX = ((VALUE > UINT_LEAST8_MAX)  ? 1 : 0) +
                                         ((VALUE > UINT16_MAX) ? 1 : 0) +
                                         ((VALUE > UINT32_MAX) ? 1 : 0);

  public:

    typedef typename private_smallest::best_fit_uint_type<TYPE_INDEX>::type type;
  };

#if TYPHOON_USING_CPP11
  template <uintmax_t VALUE>
  using smallest_uint_for_value_t = typename smallest_uint_for_value<VALUE>::type;
#endif

  //***************************************************************************
  /// Template to determine the smallest int type that can contain the
  /// specified signed value.
  /// Defines 'type' which is the type of the smallest signed integer.
  ///\ingroup smallest
  //***************************************************************************
  template <const intmax_t VALUE>
  struct smallest_int_for_value
  {
  private:

    // Determines the index of the best signed type for the required value.
    static TYPHOON_CONSTANT int TYPE_INDEX = (((VALUE > intmax_t(INT_LEAST8_MAX)) || (VALUE < intmax_t(INT_LEAST8_MIN))) ? 1 : 0) +
                                         (((VALUE > intmax_t(INT16_MAX))      || (VALUE < intmax_t(INT16_MIN))) ? 1 : 0) +
                                         (((VALUE > intmax_t(INT32_MAX))      || (VALUE < intmax_t(INT32_MIN))) ? 1 : 0);

  public:

    typedef typename private_smallest::best_fit_int_type<TYPE_INDEX>::type type;
  };

#if TYPHOON_USING_CPP11
  template <intmax_t VALUE>
  using smallest_int_for_value_t = typename smallest_int_for_value<VALUE>::type;
#endif
}

#endif
