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

#ifndef TYPHOON_MEMORY_MODEL_HPP
#define TYPHOON_MEMORY_MODEL_HPP

#include "platform.hpp"
#include "user_type.hpp"
#include "type_lookup.hpp"

#include <stdint.h>

namespace tpn
{
  TYPHOON_DECLARE_USER_TYPE(memory_model, int)
  TYPHOON_USER_TYPE(MEMORY_MODEL_SMALL,  0)
  TYPHOON_USER_TYPE(MEMORY_MODEL_MEDIUM, 1)
  TYPHOON_USER_TYPE(MEMORY_MODEL_LARGE,  2)
  TYPHOON_USER_TYPE(MEMORY_MODEL_HUGE,   3)
  TYPHOON_END_USER_TYPE(memory_model)

  template <const size_t MEMORY_MODEL>
  struct size_type_lookup;

  template <>
  struct size_type_lookup<tpn::memory_model::MEMORY_MODEL_SMALL>
  {
    typedef uint_least8_t type;
  };

  template <>
  struct size_type_lookup<tpn::memory_model::MEMORY_MODEL_MEDIUM>
  {
    typedef uint_least16_t type;
  };

  template <>
  struct size_type_lookup<tpn::memory_model::MEMORY_MODEL_LARGE>
  {
    typedef uint_least32_t type;
  };

  template <>
  struct size_type_lookup<tpn::memory_model::MEMORY_MODEL_HUGE>
  {
    typedef uint_least64_t type;
  };
}

#endif

