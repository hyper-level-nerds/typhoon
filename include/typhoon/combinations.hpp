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

#ifndef TYPHOON_COMBINATIONS_HPP
#define TYPHOON_COMBINATIONS_HPP

#include "platform.hpp"
#include "permutations.hpp"
#include "factorial.hpp"

///\defgroup combinations combinations
/// combinations<N, K> : Calculates K combinations from N.
///\ingroup maths

namespace tpn
{
  //***************************************************************************
  ///\ingroup combinations
  /// Calculates combinations.
  //***************************************************************************
  template <const size_t NV, const size_t KV>
  struct combinations
  {
    static TYPHOON_CONSTANT size_t value = tpn::permutations<NV, KV>::value / tpn::factorial<KV>::value;
  };

#if TYPHOON_USING_CPP17
  template <size_t NV, size_t KV>
  inline constexpr size_t combinations_v = combinations<NV, KV>::value;
#endif
}

#endif
