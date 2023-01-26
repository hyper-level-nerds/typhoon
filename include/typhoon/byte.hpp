///\file

//
// Copyright (c) 2022-2022 Juan Jin (Jin at https://github.com/hyper-level-nerds)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef TYPHOON_BYTE_HPP
#define TYPHOON_BYTE_HPP

#include "platform.hpp"
#include "type_traits.hpp"

namespace tpn
{
#if TYPHOON_USING_CPP11 && !defined(TYPHOON_BYTE_FORCE_CPP03_IMPLEMENTATION)

  enum class byte : unsigned char {};

  //*************************************************************************
  /// To integer.
  //*************************************************************************
  template <typename TInteger>
  constexpr
    typename tpn::enable_if<tpn::is_integral<TInteger>::value, TInteger>::type
    to_integer(tpn::byte b) noexcept
  {
    return TInteger(b);
  }

  //*************************************************************************
  /// Shift left.
  //*************************************************************************
  template <typename TInteger>
  constexpr
    typename tpn::enable_if<tpn::is_integral<TInteger>::value, tpn::byte>::type
    operator <<(tpn::byte b, TInteger shift) noexcept
  {
    return  tpn::byte(static_cast<unsigned int>(b) << shift);
  }

  //*************************************************************************
  /// Shift right
  //*************************************************************************
  template <typename TInteger>
  constexpr
    typename tpn::enable_if<tpn::is_integral<TInteger>::value, tpn::byte>::type
    operator >>(tpn::byte b, TInteger shift) noexcept
  {
    return  tpn::byte(static_cast<unsigned int>(b) >> shift);
  }

  //*************************************************************************
  /// Shift left equals.
  //*************************************************************************
  template <typename TInteger>
  constexpr
    typename tpn::enable_if<tpn::is_integral<TInteger>::value, tpn::byte&>::type
    operator <<=(tpn::byte& b, TInteger shift) noexcept
  {
    return b = b << shift;;
  }

  //*************************************************************************
  /// Shift right equals.
  //*************************************************************************
  template <typename TInteger>
  constexpr
    typename tpn::enable_if<tpn::is_integral<TInteger>::value, tpn::byte&>::type
    operator >>=(tpn::byte& b, TInteger shift) noexcept
  {
    return b = b >> shift;
  }

  //*************************************************************************
  /// Or.
  //*************************************************************************
  constexpr tpn::byte operator |(tpn::byte lhs, tpn::byte rhs) noexcept
  {
    return tpn::byte(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
  }

  //*************************************************************************
  /// And.
  //*************************************************************************
  constexpr tpn::byte operator &(tpn::byte lhs, tpn::byte rhs) noexcept
  {
    return tpn::byte(static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
  }

  //*************************************************************************
  /// Exclusive Or.
  //*************************************************************************
  constexpr tpn::byte operator ^(tpn::byte lhs, tpn::byte rhs) noexcept
  {
    return tpn::byte(static_cast<unsigned int>(lhs) ^ static_cast<unsigned int>(rhs));
  }

  //*************************************************************************
  /// Or equals.
  //*************************************************************************
  TYPHOON_CONSTEXPR14 tpn::byte& operator |=(tpn::byte& lhs, tpn::byte rhs) noexcept
  {
    return lhs = lhs | rhs;
  }

  //*************************************************************************
  /// And equals
  //*************************************************************************
  TYPHOON_CONSTEXPR14 tpn::byte& operator &=(tpn::byte& lhs, tpn::byte rhs) noexcept
  {
    return lhs = lhs & rhs;
  }

  //*************************************************************************
  /// Exclusive or equals.
  //*************************************************************************
  TYPHOON_CONSTEXPR14 tpn::byte& operator ^=(tpn::byte& lhs, tpn::byte rhs) noexcept
  {
    return lhs = lhs ^ rhs;
  }

  //*************************************************************************
  /// Not.
  //*************************************************************************
  constexpr tpn::byte operator ~(tpn::byte b) noexcept
  {
    return tpn::byte(~static_cast<unsigned int>(b));
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
    typename tpn::enable_if<tpn::is_integral<TInteger>::value, TInteger>::type
    to_integer(tpn::byte b);

    friend bool operator ==(tpn::byte lhs, tpn::byte rhs);

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
  bool operator ==(tpn::byte lhs, tpn::byte rhs)
  {
    return (lhs.value == rhs.value);
  }

  //*************************************************************************
  /// Inequality test
  //*************************************************************************
  bool operator !=(tpn::byte lhs, tpn::byte rhs)
  {
    return !(lhs == rhs);
  }

  //*************************************************************************
  /// To integer.
  //*************************************************************************
  template <typename TInteger> 
  typename tpn::enable_if<tpn::is_integral<TInteger>::value, TInteger>::type
  to_integer(tpn::byte b)
  {
    return TInteger(b);
  }

  //*************************************************************************
  /// Shift left.
  //*************************************************************************
  template <typename TInteger>
  typename tpn::enable_if<tpn::is_integral<TInteger>::value, tpn::byte>::type
  operator <<(tpn::byte b, TInteger shift)
  {
    return  tpn::byte(to_integer<unsigned int>(b) << shift);
  }

  //*************************************************************************
  /// Shift right
  //*************************************************************************
  template <typename TInteger>
  typename tpn::enable_if<tpn::is_integral<TInteger>::value, tpn::byte>::type
  operator >>(tpn::byte b, TInteger shift)
  {
    return  tpn::byte(to_integer<unsigned int>(b) >> shift);
  }

  //*************************************************************************
  /// Shift left equals.
  //*************************************************************************
  template <typename TInteger>  
  typename tpn::enable_if<tpn::is_integral<TInteger>::value, tpn::byte&>::type
  operator <<=(tpn::byte& b, TInteger shift)
  {
    b = b << shift;

    return b;
  }

  //*************************************************************************
  /// Shift right equals.
  //*************************************************************************
  template <typename TInteger>
  typename tpn::enable_if<tpn::is_integral<TInteger>::value, tpn::byte&>::type
  operator >>=(tpn::byte& b, TInteger shift)
  {
    b = b >> shift;

    return b;
  }

  //*************************************************************************
  /// Or.
  //*************************************************************************
  tpn::byte operator |(tpn::byte lhs, tpn::byte rhs)
  {
    return tpn::byte(to_integer<unsigned int>(lhs) | to_integer<unsigned int>(rhs));
  }

  //*************************************************************************
  /// And.
  //*************************************************************************
  tpn::byte operator &(tpn::byte lhs, tpn::byte rhs)
  {
    return tpn::byte(to_integer<unsigned int>(lhs) & to_integer<unsigned int>(rhs));
  }

  //*************************************************************************
  /// Exclusive Or.
  //*************************************************************************
   tpn::byte operator ^(tpn::byte lhs, tpn::byte rhs)
  {
   return tpn::byte(to_integer<unsigned int>(lhs) ^ to_integer<unsigned int>(rhs));
  }

  //*************************************************************************
  /// Or equals.
  //*************************************************************************
  tpn::byte& operator |=(tpn::byte& lhs, tpn::byte rhs)
  {
    return lhs = lhs | rhs;
  }

  //*************************************************************************
  /// And equals
  //*************************************************************************
  tpn::byte& operator &=(tpn::byte& lhs, tpn::byte rhs)
  {
    return lhs = lhs & rhs;
  }

  //*************************************************************************
  /// Exclusive or equals.
  //*************************************************************************
  tpn::byte& operator ^=(tpn::byte& lhs, tpn::byte rhs)
  {
    return lhs = lhs ^ rhs;
  }

  //*************************************************************************
  /// Not.
  //*************************************************************************
   tpn::byte operator ~(tpn::byte b)
  {
    return tpn::byte(~to_integer<unsigned char>(b));
  }

#endif


}

#endif