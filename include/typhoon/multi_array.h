///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tphn
https://www.tphncpp.com

Copyright(c) 2019 John Wellbelove

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

#ifndef TYPHOON_MULTI_ARRAY_INCLUDED
#define TYPHOON_MULTI_ARRAY_INCLUDED

#include "platform.h"
#include "array.h"

///\defgroup multi_array multi_array
/// A multi dimensional array.
///\ingroup containers

namespace tphn
{
#if TYPHOON_USING_CPP11

  namespace private_multi_array
  {
    template <class T, size_t TD1, size_t... TDx>
    struct multi_array_t
    {
      using type = tphn::array<typename multi_array_t<T, TDx...>::type, TD1>;
      static constexpr size_t SIZE = TD1;
    };

    template <class T, size_t TD1>
    struct multi_array_t<T, TD1>
    {
      using type = tphn::array<T, TD1>;
      static constexpr size_t SIZE = TD1;
    };
  }

  template <typename T, const size_t... TDx>
  using multi_array = typename private_multi_array::multi_array_t<T, TDx...>::type;

#endif
}

#endif
