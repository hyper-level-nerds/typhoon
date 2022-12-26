///\file

/******************************************************************************
The MIT License(MIT)
Embedded Template Library.
https://github.com/TYPHOONCPP/tphn
https://www.tphncpp.com
Copyright(c) 2022 John Wellbelove
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

#ifndef TYPHOON_BYTE_INCLUDED
#define TYPHOON_BYTE_INCLUDED

#include "platform.h"
#include "type_traits.h"

namespace tphn
{
#if TYPHOON_USING_CPP11 && !defined(TYPHOON_BYTE_FORCE_CPP03_IMPLEMENTATION)

  enum class byte : unsigned char {};

  //*************************************************************************
  /// To integer.
  //*************************************************************************
  template <typename TInteger>
  constexpr
    typename tphn::enable_if<tphn::is_integral<TInteger>::value, TInteger>::type
    to_integer(tphn::byte b) noexcept
  {
    return TInteger(b);
  }

  //*************************************************************************
  /// Shift left.
  //*************************************************************************
  template <typename TInteger>
  constexpr
    typename tphn::enable_if<tphn::is_integral<TInteger>::value, tphn::byte>::type
    operator <<(tphn::byte b, TInteger shift) noexcept
  {
    return  tphn::byte(static_cast<unsigned int>(b) << shift);
  }

  //*************************************************************************
  /// Shift right
  //*************************************************************************
  template <typename TInteger>
  constexpr
    typename tphn::enable_if<tphn::is_integral<TInteger>::value, tphn::byte>::type
    operator >>(tphn::byte b, TInteger shift) noexcept
  {
    return  tphn::byte(static_cast<unsigned int>(b) >> shift);
  }

  //*************************************************************************
  /// Shift left equals.
  //*************************************************************************
  template <typename TInteger>
  constexpr
    typename tphn::enable_if<tphn::is_integral<TInteger>::value, tphn::byte&>::type
    operator <<=(tphn::byte& b, TInteger shift) noexcept
  {
    return b = b << shift;;
  }

  //*************************************************************************
  /// Shift right equals.
  //*************************************************************************
  template <typename TInteger>
  constexpr
    typename tphn::enable_if<tphn::is_integral<TInteger>::value, tphn::byte&>::type
    operator >>=(tphn::byte& b, TInteger shift) noexcept
  {
    return b = b >> shift;
  }

  //*************************************************************************
  /// Or.
  //*************************************************************************
  constexpr tphn::byte operator |(tphn::byte lhs, tphn::byte rhs) noexcept
  {
    return tphn::byte(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
  }

  //*************************************************************************
  /// And.
  //*************************************************************************
  constexpr tphn::byte operator &(tphn::byte lhs, tphn::byte rhs) noexcept
  {
    return tphn::byte(static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
  }

  //*************************************************************************
  /// Exclusive Or.
  //*************************************************************************
  constexpr tphn::byte operator ^(tphn::byte lhs, tphn::byte rhs) noexcept
  {
    return tphn::byte(static_cast<unsigned int>(lhs) ^ static_cast<unsigned int>(rhs));
  }

  //*************************************************************************
  /// Or equals.
  //*************************************************************************
  TYPHOON_CONSTEXPR14 tphn::byte& operator |=(tphn::byte& lhs, tphn::byte rhs) noexcept
  {
    return lhs = lhs | rhs;
  }

  //*************************************************************************
  /// And equals
  //*************************************************************************
  TYPHOON_CONSTEXPR14 tphn::byte& operator &=(tphn::byte& lhs, tphn::byte rhs) noexcept
  {
    return lhs = lhs & rhs;
  }

  //*************************************************************************
  /// Exclusive or equals.
  //*************************************************************************
  TYPHOON_CONSTEXPR14 tphn::byte& operator ^=(tphn::byte& lhs, tphn::byte rhs) noexcept
  {
    return lhs = lhs ^ rhs;
  }

  //*************************************************************************
  /// Not.
  //*************************************************************************
  constexpr tphn::byte operator ~(tphn::byte b) noexcept
  {
    return tphn::byte(~static_cast<unsigned int>(b));
  }

#else

  //*************************************************************************
  /// The byte class.
  //*************************************************************************
  class byte
  {
  public:

    // Friend functions
    template <typename TInteger>
    friend
    typename tphn::enable_if<tphn::is_integral<TInteger>::value, TInteger>::type
    to_integer(tphn::byte b);

    friend bool operator ==(tphn::byte lhs, tphn::byte rhs);

    // Default constructor
    byte()
      : value(0U)
    {
    }

    // Construct from a value castable to unsigned char
    template <typename T>
    explicit byte(T v)
      : value(static_cast<unsigned char>(v))
    {
    }

  private:

    // Cast to a T
    template <typename T>
    operator T() const
    {
      return static_cast<T>(value);
    }

    // The byte value
    unsigned char value;
  };

  //*************************************************************************
  /// Equality test
  //*************************************************************************
  bool operator ==(tphn::byte lhs, tphn::byte rhs)
  {
    return (lhs.value == rhs.value);
  }

  //*************************************************************************
  /// Inequality test
  //*************************************************************************
  bool operator !=(tphn::byte lhs, tphn::byte rhs)
  {
    return !(lhs == rhs);
  }

  //*************************************************************************
  /// To integer.
  //*************************************************************************
  template <typename TInteger> 
  typename tphn::enable_if<tphn::is_integral<TInteger>::value, TInteger>::type
  to_integer(tphn::byte b)
  {
    return TInteger(b);
  }

  //*************************************************************************
  /// Shift left.
  //*************************************************************************
  template <typename TInteger>
  typename tphn::enable_if<tphn::is_integral<TInteger>::value, tphn::byte>::type
  operator <<(tphn::byte b, TInteger shift)
  {
    return  tphn::byte(to_integer<unsigned int>(b) << shift);
  }

  //*************************************************************************
  /// Shift right
  //*************************************************************************
  template <typename TInteger>
  typename tphn::enable_if<tphn::is_integral<TInteger>::value, tphn::byte>::type
  operator >>(tphn::byte b, TInteger shift)
  {
    return  tphn::byte(to_integer<unsigned int>(b) >> shift);
  }

  //*************************************************************************
  /// Shift left equals.
  //*************************************************************************
  template <typename TInteger>  
  typename tphn::enable_if<tphn::is_integral<TInteger>::value, tphn::byte&>::type
  operator <<=(tphn::byte& b, TInteger shift)
  {
    b = b << shift;

    return b;
  }

  //*************************************************************************
  /// Shift right equals.
  //*************************************************************************
  template <typename TInteger>
  typename tphn::enable_if<tphn::is_integral<TInteger>::value, tphn::byte&>::type
  operator >>=(tphn::byte& b, TInteger shift)
  {
    b = b >> shift;

    return b;
  }

  //*************************************************************************
  /// Or.
  //*************************************************************************
  tphn::byte operator |(tphn::byte lhs, tphn::byte rhs)
  {
    return tphn::byte(to_integer<unsigned int>(lhs) | to_integer<unsigned int>(rhs));
  }

  //*************************************************************************
  /// And.
  //*************************************************************************
  tphn::byte operator &(tphn::byte lhs, tphn::byte rhs)
  {
    return tphn::byte(to_integer<unsigned int>(lhs) & to_integer<unsigned int>(rhs));
  }

  //*************************************************************************
  /// Exclusive Or.
  //*************************************************************************
   tphn::byte operator ^(tphn::byte lhs, tphn::byte rhs)
  {
   return tphn::byte(to_integer<unsigned int>(lhs) ^ to_integer<unsigned int>(rhs));
  }

  //*************************************************************************
  /// Or equals.
  //*************************************************************************
  tphn::byte& operator |=(tphn::byte& lhs, tphn::byte rhs)
  {
    return lhs = lhs | rhs;
  }

  //*************************************************************************
  /// And equals
  //*************************************************************************
  tphn::byte& operator &=(tphn::byte& lhs, tphn::byte rhs)
  {
    return lhs = lhs & rhs;
  }

  //*************************************************************************
  /// Exclusive or equals.
  //*************************************************************************
  tphn::byte& operator ^=(tphn::byte& lhs, tphn::byte rhs)
  {
    return lhs = lhs ^ rhs;
  }

  //*************************************************************************
  /// Not.
  //*************************************************************************
   tphn::byte operator ~(tphn::byte b)
  {
    return tphn::byte(~to_integer<unsigned char>(b));
  }

#endif


}

#endif