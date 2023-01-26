#ifndef TYPHOON_RATIO_HPP
#define TYPHOON_RATIO_HPP

#include "platform.hpp"

#include <stddef.h>
#include <stdint.h>

///\defgroup ratio ratio
///\ingroup maths

namespace tpn
{
  template <const size_t NUM, const size_t DEN = 1UL>
  struct ratio
  {
    static TYPHOON_CONSTANT intmax_t num = NUM;
    static TYPHOON_CONSTANT intmax_t den = DEN;
  };

  #if INT_MAX > INT32_MAX
    typedef ratio<1, 1000000000000000000000000> yocto;
    typedef ratio<1, 1000000000000000000000>    zepto;
    typedef ratio<1, 1000000000000000000>       atto;
    typedef ratio<1, 1000000000000000>          femto;
    typedef ratio<1, 1000000000000>             pico;
  #endif

  #if (INT_MAX >= INT32_MAX)
    typedef ratio<1, 1000000000>                nano;
    typedef ratio<1, 1000000>                   micro;
  #endif

  #if (INT_MAX >= INT16_MAX)
    typedef ratio<1, 1000>                      milli;
    typedef ratio<1, 100>                       centi;
    typedef ratio<1, 10>                        deci;
    typedef ratio<10, 1>                        deca;
    typedef ratio<100, 1>                       hecto;
    typedef ratio<1000, 1>                      kilo;
  #endif

  #if (INT_MAX >= INT32_MAX)
    typedef ratio<1000000, 1>                   mega;
    typedef ratio<1000000000, 1>                giga;
  #endif

  #if INT_MAX > INT32_MAX
    typedef ratio<1000000000000, 1>             tera;
    typedef ratio<1000000000000000, 1>          peta;
    typedef ratio<1000000000000000000, 1>       exa;
    typedef ratio<1000000000000000000000, 1>    zetta;
    typedef ratio<1000000000000000000000000, 1> yotta;
  #endif

  /// An approximation of PI to 6 digits.
  typedef ratio<355, 113> ratio_pi;

  /// An approximation of root 2.
  typedef ratio<239, 169> ratio_root2;

  /// An approximation of e.
  typedef ratio<326, 120> ratio_e;
}

#endif

