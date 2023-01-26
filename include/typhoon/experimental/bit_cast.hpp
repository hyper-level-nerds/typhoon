#pragma once

#include "platform.hpp"

template <typename TDestination, typename TSource>
typename tpn::enable_if<(sizeof(TDestination) == sizeof(TSource))   && 
                         tpn::is_trivially_copyable<TSource>::value && 
                         tpn::is_trivially_copyable<TDestination>::value, TDestination>::type
  bit_cast(const TSource& source) TYPHOON_NOEXCEPT
{
  TDestination destination;

  memcpy(&destination, &source, sizeof(TDestination));

  return destination;
}

template <typename TDestination, typename TSource>
TYPHOON_CONSTEXPR
typename tpn::enable_if<(sizeof(TDestination) == sizeof(TSource))   &&
                         tpn::is_trivially_copyable<TSource>::value &&
                         tpn::is_trivially_copyable<TDestination>::value, TDestination>::type
  bit_cast(const TSource& source) TYPHOON_NOEXCEPT
{
  TDestination destination;

  __builtin_memcpy(&destination, &source, sizeof(TDestination));

  return destination;
}



