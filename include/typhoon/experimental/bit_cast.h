#pragma once

#include "platform.h"

template <typename TDestination, typename TSource>
typename tphn::enable_if<(sizeof(TDestination) == sizeof(TSource))   && 
                         tphn::is_trivially_copyable<TSource>::value && 
                         tphn::is_trivially_copyable<TDestination>::value, TDestination>::type
  bit_cast(const TSource& source) TYPHOON_NOEXCEPT
{
  TDestination destination;

  memcpy(&destination, &source, sizeof(TDestination));

  return destination;
}

template <typename TDestination, typename TSource>
TYPHOON_CONSTEXPR
typename tphn::enable_if<(sizeof(TDestination) == sizeof(TSource))   &&
                         tphn::is_trivially_copyable<TSource>::value &&
                         tphn::is_trivially_copyable<TDestination>::value, TDestination>::type
  bit_cast(const TSource& source) TYPHOON_NOEXCEPT
{
  TDestination destination;

  __builtin_memcpy(&destination, &source, sizeof(TDestination));

  return destination;
}



