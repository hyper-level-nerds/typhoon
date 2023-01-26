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

#ifndef TYPHOON_ABSOLUTE_HPP
#define TYPHOON_ABSOLUTE_HPP

#include "type_traits.hpp"
#include "integral_limits.hpp"

namespace tpn
{
  //***************************************************************************
  // For signed types.
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR typename tpn::enable_if<tpn::is_signed<T>::value, T>::type
    absolute(T value)
  {
    return (value < T(0)) ? -value : value;
  }

  //***************************************************************************
  // For unsigned types.
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR typename tpn::enable_if<tpn::is_unsigned<T>::value, T>::type
    absolute(T value)
  {
    return value;
  }

  //***************************************************************************
  // For signed types.
  // Returns the result as the unsigned type.
  //***************************************************************************
#if TYPHOON_USING_CPP11
  template <typename T, typename TReturn = typename tpn::make_unsigned<T>::type>
#else
  template <typename T, typename TReturn>
  #endif
  TYPHOON_CONSTEXPR typename tpn::enable_if<tpn::is_signed<T>::value, TReturn>::type
    absolute_unsigned(T value)
  {
    return (value == tpn::integral_limits<T>::min) ? (tpn::integral_limits<TReturn>::max / 2U) + 1U
                                                   : (value < T(0)) ? TReturn(-value) : TReturn(value);
  }

  //***************************************************************************
  // For unsigned types.
  // Returns the result as the unsigned type.
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR typename tpn::enable_if<tpn::is_unsigned<T>::value, T>::type
    absolute_unsigned(T value)
  {
    return tpn::absolute(value);
  }
}

#endif

