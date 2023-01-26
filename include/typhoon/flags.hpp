///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tpn
https://www.tpncpp.com

Copyright(c) 2020 John Wellbelove

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

#ifndef TYPHOON_FLAGS_HPP
#define TYPHOON_FLAGS_HPP

#include "platform.hpp"
#include "algorithm.hpp"
#include "type_traits.hpp"
#include "integral_limits.hpp"
#include "static_assert.hpp"
#include "initializer_list.hpp"

#include <string.h>
#include <stddef.h>
#include <stdint.h>

namespace tpn
{
  //*************************************************************************
  /// flags
  /// Flags based around an integral value.
  //*************************************************************************
  template <typename T, T MASK = tpn::integral_limits<T>::max>
  class flags
  {
  public:

    TYPHOON_STATIC_ASSERT(tpn::is_integral<T>::value && tpn::is_unsigned<T>::value, "Unsigned integral values only");

    typedef T value_type;

    static TYPHOON_CONSTANT value_type ALL_SET   = tpn::integral_limits<value_type>::max & MASK;
    static TYPHOON_CONSTANT value_type ALL_CLEAR = 0;

    static TYPHOON_CONSTANT size_t NBITS = tpn::integral_limits<value_type>::bits;

    //*************************************************************************
    /// Constructor
    //*************************************************************************
    TYPHOON_CONSTEXPR flags() TYPHOON_NOEXCEPT
      : data(value_type(0))
    {
    }

    TYPHOON_CONSTEXPR flags(value_type pattern) TYPHOON_NOEXCEPT
      : data(pattern & MASK)
    {
    }

    TYPHOON_CONSTEXPR flags(const flags<T, MASK>& pattern) TYPHOON_NOEXCEPT
      : data(pattern.value())
    {
    }

    //*************************************************************************
    /// Tests bits.
    //*************************************************************************
    template <value_type pattern>
    TYPHOON_CONSTEXPR bool test() const TYPHOON_NOEXCEPT
    {
      return (data & pattern) != value_type(0);
    }

    //*******************************************
    TYPHOON_CONSTEXPR bool test(value_type pattern) const TYPHOON_NOEXCEPT
    {
      return (data & pattern) != value_type(0);
    }

    //*************************************************************************
    /// Set the bits.
    //*************************************************************************
    template <value_type pattern, bool value>
    TYPHOON_CONSTEXPR14 flags<T, MASK>& set() TYPHOON_NOEXCEPT
    {
      value ? data |= (pattern & MASK) : data &= (~pattern & MASK);

      return *this;
    }

    //*******************************************
    template <value_type pattern>
    TYPHOON_CONSTEXPR14 flags<T, MASK>& set(bool value) TYPHOON_NOEXCEPT
    {
      value ? data |= (pattern & MASK) : data &= (~pattern & MASK);

      return *this;
    }

    //*******************************************
    template <value_type pattern>
    TYPHOON_CONSTEXPR14 flags<T, MASK>& set() TYPHOON_NOEXCEPT
    {
      data |= (pattern & MASK);

      return *this;
    }

    //*******************************************
    TYPHOON_CONSTEXPR14 flags<T, MASK>& set(value_type pattern) TYPHOON_NOEXCEPT
    {
      data |= (pattern & MASK);

      return *this;
    }

    //*******************************************
    TYPHOON_CONSTEXPR14 flags<T, MASK>& set(value_type pattern, bool value) TYPHOON_NOEXCEPT
    {
      value ? data |= (pattern & MASK) : data &= (~pattern & MASK);

      return *this;
    }

    //*************************************************************************
    /// Clear all of the flags.
    //*************************************************************************
    TYPHOON_CONSTEXPR14 flags<T, MASK>& clear() TYPHOON_NOEXCEPT
    {
      data = ALL_CLEAR;

      return *this;
    }

    //*************************************************************************
    /// Reset the bit at the pattern.
    //*************************************************************************
    template <value_type pattern>
    TYPHOON_CONSTEXPR14 flags<T, MASK>& reset() TYPHOON_NOEXCEPT
    {
      data &= ~pattern;

      return *this;
    }

    //*******************************************
    TYPHOON_CONSTEXPR14 flags<T, MASK>& reset(value_type pattern) TYPHOON_NOEXCEPT
    {
      data &= ~pattern;

      return *this;
    }

    //*************************************************************************
    /// Flip bits.
    //*************************************************************************
    TYPHOON_CONSTEXPR14 flags<T, MASK>& flip() TYPHOON_NOEXCEPT
    {
      data = (~data & MASK);

      return *this;
    }

    //*******************************************
    template <value_type pattern>
    TYPHOON_CONSTEXPR14 flags<T, MASK>& flip() TYPHOON_NOEXCEPT
    {
      data ^= pattern & MASK;

      return *this;
    }

    //*******************************************
    TYPHOON_CONSTEXPR14 flags<T, MASK>& flip(value_type pattern) TYPHOON_NOEXCEPT
    {
      data ^= pattern & MASK;

      return *this;
    }

    //*************************************************************************
    // Are all the bits sets?
    //*************************************************************************
    TYPHOON_CONSTEXPR bool all() const TYPHOON_NOEXCEPT
    {
      return data == MASK;
    }

    //*******************************************
    template <value_type pattern>
    TYPHOON_CONSTEXPR bool all_of() const TYPHOON_NOEXCEPT
    {
      return (data & (pattern & MASK)) == (pattern & MASK);
    }

    //*******************************************
    TYPHOON_CONSTEXPR bool all_of(value_type pattern) const TYPHOON_NOEXCEPT
    {
      return (data & (pattern & MASK)) == (pattern & MASK);
    }

    //*************************************************************************
    /// Are none of the bits set?
    //*************************************************************************
    TYPHOON_CONSTEXPR bool none() const TYPHOON_NOEXCEPT
    {
      return (data & MASK) == ALL_CLEAR;
    }

    //*******************************************
    template <value_type pattern>
    TYPHOON_CONSTEXPR bool none_of() const TYPHOON_NOEXCEPT
    {
      return !any_of(pattern);
    }

    //*******************************************
    TYPHOON_CONSTEXPR bool none_of(value_type pattern) const TYPHOON_NOEXCEPT
    {
      return !any_of(pattern);
    }

    //*************************************************************************
    /// Are any of the bits set?
    //*************************************************************************
    TYPHOON_CONSTEXPR bool any() const TYPHOON_NOEXCEPT
    {
      return (data & MASK) != value_type(0);
    }

    //*******************************************
    template <value_type pattern>
    TYPHOON_CONSTEXPR bool any_of() const TYPHOON_NOEXCEPT
    {
      return (data & (pattern & MASK)) != value_type(0);
    }

    //*******************************************
    TYPHOON_CONSTEXPR bool any_of(value_type pattern) const
    {
      return (data & (pattern & MASK)) != value_type(0);
    }

    //*************************************************************************
    /// Return the value of the flags.
    //*************************************************************************
    TYPHOON_CONSTEXPR value_type value() const TYPHOON_NOEXCEPT
    {
      return data;
    }

    //*************************************************************************
    /// Set the value of the flags.
    //*************************************************************************
    TYPHOON_CONSTEXPR14 flags<T, MASK>& value(value_type pattern) TYPHOON_NOEXCEPT
    {
      data = pattern & MASK;

      return *this;
    }

    //*************************************************************************
    /// Return the value of the flags.
    //*************************************************************************
    TYPHOON_CONSTEXPR operator value_type() const TYPHOON_NOEXCEPT
    {
      return data;
    }

    //*************************************************************************
    /// operator &=
    //*************************************************************************
    TYPHOON_CONSTEXPR14 flags<T, MASK>& operator &=(value_type pattern) TYPHOON_NOEXCEPT
    {
      data &= pattern;

      return *this;
    }

    //*************************************************************************
    /// operator |=
    //*************************************************************************
    TYPHOON_CONSTEXPR14 flags<T, MASK>& operator |=(value_type pattern) TYPHOON_NOEXCEPT
    {
      data |= (pattern & MASK);

      return *this;
    }

    //*************************************************************************
    /// operator ^=
    //*************************************************************************
    TYPHOON_CONSTEXPR14 flags<T, MASK>& operator ^=(value_type pattern) TYPHOON_NOEXCEPT
    {
      data ^= (pattern & MASK);

      return *this;
    }

    //*************************************************************************
    /// operator =
    //*************************************************************************
    TYPHOON_CONSTEXPR14 flags<T, MASK>& operator =(flags<T, MASK> other) TYPHOON_NOEXCEPT
    {
      data = other.data;

      return *this;
    }

    //*************************************************************************
    /// operator =
    //*************************************************************************
    TYPHOON_CONSTEXPR14 flags<T, MASK>& operator =(value_type pattern) TYPHOON_NOEXCEPT
    {
      data = (pattern & MASK);

      return *this;
    }

    //*************************************************************************
    /// swap
    //*************************************************************************
    void swap(flags<T, MASK>& other) TYPHOON_NOEXCEPT
    {
      using TYPHOON_OR_STD::swap;
      swap(data, other.data);
    }

  private:

    value_type data;
  };

  //***************************************************************************
  /// operator ==
  //***************************************************************************
  template <typename T, T MASK>
  TYPHOON_CONSTEXPR bool operator == (flags<T, MASK> lhs, flags<T, MASK> rhs) TYPHOON_NOEXCEPT
  {
    return lhs.value() == rhs.value();
  }

  //***************************************************************************
  /// operator !=
  //***************************************************************************
  template <typename T, T MASK>
  TYPHOON_CONSTEXPR bool operator != (flags<T, MASK> lhs, flags<T, MASK> rhs) TYPHOON_NOEXCEPT
  {
    return !(lhs == rhs);
  }

  //*************************************************************************
  /// swap
  //*************************************************************************
  template <typename T, T MASK>
  void swap(tpn::flags<T, MASK>& lhs, tpn::flags<T, MASK>& rhs) TYPHOON_NOEXCEPT
  {
    lhs.swap(rhs);
  }
}

#endif
