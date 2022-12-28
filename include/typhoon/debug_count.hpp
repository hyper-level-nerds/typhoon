///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tphn
https://www.tphncpp.com

Copyright(c) 2017 John Wellbelove

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

#ifndef TYPHOON_DEBUG_COUNT_HPP
#define TYPHOON_DEBUG_COUNT_HPP

#include "platform.hpp"
#include "atomic.hpp"

#include <assert.h>
#include <stdint.h>

///\defgroup debug_count debug count
///\ingroup utilities

#if defined(TYPHOON_DEBUG_COUNT)

  #define TYPHOON_DECLARE_DEBUG_COUNT              tphn::debug_count tphn_debug_count;
  #define TYPHOON_SET_DEBUG_COUNT(n)               tphn_debug_count.set(n)
  #define TYPHOON_GET_DEBUG_COUNT                  tphn_debug_count.get()
  #define TYPHOON_INCREMENT_DEBUG_COUNT            ++tphn_debug_count;
  #define TYPHOON_DECREMENT_DEBUG_COUNT            --tphn_debug_count;
  #define TYPHOON_ADD_DEBUG_COUNT(n)               tphn_debug_count += (n);
  #define TYPHOON_SUBTRACT_DEBUG_COUNT(n)          tphn_debug_count -= (n);
  #define TYPHOON_RESET_DEBUG_COUNT                tphn_debug_count.clear();
  #define TYPHOON_OBJECT_RESET_DEBUG_COUNT(object) object.tphn_debug_count.clear();
  #define TYPHOON_OBJECT_GET_DEBUG_COUNT(object)   object.tphn_debug_count.get()

namespace tphn
{
  //***************************************************************************
  /// Used to count instances.
  /// Asserts if the count is decremented below zero.
  /// Asserts if the count is not zero when destructed.
  /// Does nothing in a non-debug build.
  ///\ingroup reference
  //***************************************************************************

  
  class debug_count
  {
  public:
    debug_count()
      : count(0)
    {
    }

    ~debug_count()
    {
      assert(count == 0);
    }

    debug_count& operator++()
    {
      ++count;
      return *this;
    }

    debug_count& operator--()
    {
      --count;
      assert(count >= 0);
      return *this;
    }

    template <typename T>
    debug_count& operator+=(T n)
    {
      count += int32_t(n);
      return *this;
    }

    template <typename T>
    debug_count& operator-=(T n)
    {
      count -= int32_t(n);
      return *this;
    }

    debug_count& operator=(const debug_count& other)
    {
      count.store(other.count.load());

      return *this;
    }

  #if TYPHOON_HAS_ATOMIC
    void swap(debug_count& other) TYPHOON_NOEXCEPT  // NOT ATOMIC
    {
      int32_t temp = other.count.load();
      other.count.store(count.load());
      count.store(temp);
    }
  #else
    void swap(debug_count& other) TYPHOON_NOEXCEPT
    {
      swap(count, other.count);
    }
  #endif

    operator int32_t() const
    {
      return count;
    }

    int32_t get() const
    {
      return int32_t(count);
    }

    void set(int32_t n)
    {
      count = n;
    }

    void clear()
    {
      count = 0;
    }

  private:
  #if TYPHOON_HAS_ATOMIC
    tphn::atomic_int32_t count;
  #else
    int32_t count;
  #endif
  };
}  // namespace tphn


inline void swap(tphn::debug_count& lhs, tphn::debug_count& rhs)
{
  lhs.swap(rhs);
}

#else
  #define TYPHOON_DECLARE_DEBUG_COUNT
  #define TYPHOON_SET_DEBUG_COUNT(n)
  #define TYPHOON_GET_DEBUG_COUNT
  #define TYPHOON_INCREMENT_DEBUG_COUNT
  #define TYPHOON_DECREMENT_DEBUG_COUNT
  #define TYPHOON_ADD_DEBUG_COUNT(n)
  #define TYPHOON_SUBTRACT_DEBUG_COUNT(n)
  #define TYPHOON_RESET_DEBUG_COUNT
  #define TYPHOON_OBJECT_RESET_DEBUG_COUNT(object)
  #define TYPHOON_OBJECT_GET_DEBUG_COUNT(object)
#endif  // TYPHOON_DEBUG_COUNT

#endif
