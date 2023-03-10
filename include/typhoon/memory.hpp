///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tpn
https://www.tpncpp.com

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

#ifndef TYPHOON_MEMORY_HPP
#define TYPHOON_MEMORY_HPP

#include "platform.hpp"
#include "algorithm.hpp"
#include "type_traits.hpp"
#include "iterator.hpp"
#include "utility.hpp"
#include "nullptr.hpp"
#include "alignment.hpp"
#include "placement_new.hpp"

#include "private/addressof.hpp"

#include <assert.h>
#include <string.h>

#if defined(TYPHOON_IN_UNIT_TEST) || TYPHOON_USING_STL
  #include <memory>
#endif

///\defgroup memory memory
///\ingroup tpn

namespace tpn
{
#if TYPHOON_NOT_USING_STL
  //*****************************************************************************
  /// Fills uninitialised memory range with a value.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_fill
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename T>
  typename tpn::enable_if<tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, TOutputIterator>::type
    uninitialized_fill(TOutputIterator o_begin, TOutputIterator o_end, const T& value)
  {
    tpn::fill(o_begin, o_end, value);

    return o_end;
  }

  //*****************************************************************************
  /// Fills uninitialised memory range with a value.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_fill
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename T>
  typename tpn::enable_if<!tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, TOutputIterator>::type
    uninitialized_fill(TOutputIterator o_begin, TOutputIterator o_end, const T& value)
  {
    typedef typename tpn::iterator_traits<TOutputIterator>::value_type value_type;

    while (o_begin != o_end)
    {
      ::new (static_cast<void*>(tpn::addressof(*o_begin))) value_type(value);
      ++o_begin;
    }

    return o_end;
  }

  //*****************************************************************************
  /// Fills uninitialised memory range with a value.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_fill
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename T, typename TCounter>
  typename tpn::enable_if<tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, TOutputIterator>::type
    uninitialized_fill(TOutputIterator o_begin, TOutputIterator o_end, const T& value, TCounter& count)
  {
    count += int32_t(tpn::distance(o_begin, o_end));

    tpn::fill(o_begin, o_end, value);

    return o_end;
  }

  //*****************************************************************************
  /// Fills uninitialised memory range with a value.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_fill
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename T, typename TCounter>
  typename tpn::enable_if<!tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, TOutputIterator>::type
    uninitialized_fill(TOutputIterator o_begin, TOutputIterator o_end, const T& value, TCounter& count)
  {
    count += int32_t(tpn::distance(o_begin, o_end));

    tpn::uninitialized_fill(o_begin, o_end, value);

    return o_end;
  }
#else
  //*****************************************************************************
  /// Fills uninitialised memory range with a value.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_fill
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename T>
  TOutputIterator uninitialized_fill(TOutputIterator o_begin, TOutputIterator o_end, const T& value)
  {
    std::uninitialized_fill(o_begin, o_end, value);

    return o_end;
  }

  //*****************************************************************************
  /// Fills uninitialised memory range with a value.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_fill
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename T, typename TCounter>
  TOutputIterator  uninitialized_fill(TOutputIterator o_begin, TOutputIterator o_end, const T& value, TCounter& count)
  {
    count += int32_t(tpn::distance(o_begin, o_end));

    std::uninitialized_fill(o_begin, o_end, value);

    return o_end;
  }
#endif

#if TYPHOON_NOT_USING_STL || TYPHOON_CPP11_NOT_SUPPORTED
  //*****************************************************************************
  /// Fills uninitialised memory with N values.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_fill_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename TSize, typename T>
  TOutputIterator uninitialized_fill_n(TOutputIterator o_begin, TSize n, const T& value)
  {
    return tpn::uninitialized_fill(o_begin, o_begin + n, value);
  }

  //*****************************************************************************
  /// Fills uninitialised memory with N values.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_fill_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename TSize, typename T, typename TCounter>
  TOutputIterator uninitialized_fill_n(TOutputIterator o_begin, TSize n, const T& value, TCounter& count)
  {
    count += n;

    return tpn::uninitialized_fill(o_begin, o_begin + n, value);
  }
#else
  //*****************************************************************************
  /// Fills uninitialised memory with N values.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_fill_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename TSize, typename T>
  TOutputIterator uninitialized_fill_n(TOutputIterator o_begin, TSize n, const T& value)
  {
    return std::uninitialized_fill_n(o_begin, n, value);
  }

  //*****************************************************************************
  /// Fills uninitialised memory with N values.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_fill_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename TSize, typename T, typename TCounter>
  TOutputIterator uninitialized_fill_n(TOutputIterator o_begin, TSize n, const T& value, TCounter& count)
  {
    count += n;

    return std::uninitialized_fill_n(o_begin, n, value);
  }
#endif

#if TYPHOON_NOT_USING_STL
  //*****************************************************************************
  /// Copies a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_copy
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TOutputIterator>
  typename tpn::enable_if<tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, TOutputIterator>::type
    uninitialized_copy(TInputIterator i_begin, TInputIterator i_end, TOutputIterator o_begin)
  {
    return tpn::copy(i_begin, i_end, o_begin);
  }

  //*****************************************************************************
  /// Copies a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_copy
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TOutputIterator>
  typename tpn::enable_if<!tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, TOutputIterator>::type
    uninitialized_copy(TInputIterator i_begin, TInputIterator i_end, TOutputIterator o_begin)
  {
    typedef typename tpn::iterator_traits<TOutputIterator>::value_type value_type;

    TOutputIterator o_end = o_begin;

    while (i_begin != i_end)
    {
      ::new (static_cast<void*>(tpn::addressof(*o_end))) value_type(*i_begin);
      ++i_begin;
      ++o_end;
    }

    return o_end;
  }

  //*****************************************************************************
  /// Copies a range of objects to uninitialised memory.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_copy
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TOutputIterator, typename TCounter>
  typename tpn::enable_if<tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, TOutputIterator>::type
    uninitialized_copy(TInputIterator i_begin, TInputIterator i_end, TOutputIterator o_begin, TCounter& count)
  {
    TOutputIterator o_end = tpn::copy(i_begin, i_end, o_begin);
    count += int32_t(tpn::distance(i_begin, i_end));

    return o_end;
  }

  //*****************************************************************************
  /// Copies a range of objects to uninitialised memory.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_copy
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TOutputIterator, typename TCounter>
  typename tpn::enable_if<!tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, TOutputIterator>::type
    uninitialized_copy(TInputIterator i_begin, TInputIterator i_end, TOutputIterator o_begin, TCounter& count)
  {
    TOutputIterator o_end = tpn::uninitialized_copy(i_begin, i_end, o_begin);

    count += int32_t(tpn::distance(i_begin, i_end));

    return o_end;
  }
#else
  //*****************************************************************************
  /// Copies a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_copy
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TOutputIterator>
  TOutputIterator  uninitialized_copy(TInputIterator i_begin, TInputIterator i_end, TOutputIterator o_begin)
  {
    return std::uninitialized_copy(i_begin, i_end, o_begin);
  }

  //*****************************************************************************
  /// Copies a range of objects to uninitialised memory.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_copy
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TOutputIterator, typename TCounter>
  TOutputIterator uninitialized_copy(TInputIterator i_begin, TInputIterator i_end, TOutputIterator o_begin, TCounter& count)
  {
    count += int32_t(tpn::distance(i_begin, i_end));

    return std::uninitialized_copy(i_begin, i_end, o_begin);
  }
#endif

#if TYPHOON_NOT_USING_STL || TYPHOON_CPP11_NOT_SUPPORTED
  //*****************************************************************************
  /// Copies N objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_copy_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TSize, typename TOutputIterator>
  TOutputIterator uninitialized_copy_n(TInputIterator i_begin, TSize n, TOutputIterator o_begin)
  {
    return tpn::uninitialized_copy(i_begin, i_begin + n, o_begin);
  }

  //*****************************************************************************
  /// Copies N objects to uninitialised memory.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_copy_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TSize, typename TOutputIterator, typename TCounter>
  TOutputIterator uninitialized_copy_n(TInputIterator i_begin, TSize n, TOutputIterator o_begin, TCounter& count)
  {
    count += n;

    return tpn::uninitialized_copy(i_begin, i_begin + n, o_begin);
  }
#else
  //*****************************************************************************
  /// Copies N objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_copy_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TSize, typename TOutputIterator>
  TOutputIterator uninitialized_copy_n(TInputIterator i_begin, TSize n, TOutputIterator o_begin)
  {
    return std::uninitialized_copy_n(i_begin, n, o_begin);
  }

  //*****************************************************************************
  /// Copies N objects to uninitialised memory.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_copy_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TSize, typename TOutputIterator, typename TCounter>
  TOutputIterator uninitialized_copy_n(TInputIterator i_begin, TSize n, TOutputIterator o_begin, TCounter& count)
  {
    count += n;

    return std::uninitialized_copy_n(i_begin, n, o_begin);
}
#endif

#if TYPHOON_USING_CPP11
#if TYPHOON_NOT_USING_STL || TYPHOON_CPP17_NOT_SUPPORTED
  //*****************************************************************************
  /// Moves a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_move
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TOutputIterator>
  typename tpn::enable_if<tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, TOutputIterator>::type
    uninitialized_move(TInputIterator i_begin, TInputIterator i_end, TOutputIterator o_begin)
  {
    return tpn::move(i_begin, i_end, o_begin);
  }

  //*****************************************************************************
  /// Moves a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_move
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TOutputIterator>
  typename tpn::enable_if<!tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, TOutputIterator>::type
    uninitialized_move(TInputIterator i_begin, TInputIterator i_end, TOutputIterator o_begin)
  {
    typedef typename tpn::iterator_traits<TOutputIterator>::value_type value_type;

    TOutputIterator o_end = o_begin;

    while (i_begin != i_end)
    {
      ::new (static_cast<void*>(tpn::addressof(*o_end))) value_type(tpn::move(*i_begin));
      ++i_begin;
      ++o_end;
    }

    return o_end;
  }

  //*****************************************************************************
  /// Moves a range of objects to uninitialised memory.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_move
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TOutputIterator, typename TCounter>
  typename tpn::enable_if<tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, TOutputIterator>::type
    uninitialized_move(TInputIterator i_begin, TInputIterator i_end, TOutputIterator o_begin, TCounter& count)
  {
    TOutputIterator o_end = tpn::move(i_begin, i_end, o_begin);
    count += int32_t(tpn::distance(i_begin, i_end));

    return o_end;
  }

  //*****************************************************************************
  /// Moves a range of objects to uninitialised memory.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_move
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TOutputIterator, typename TCounter>
  typename tpn::enable_if<!tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, TOutputIterator>::type
    uninitialized_move(TInputIterator i_begin, TInputIterator i_end, TOutputIterator o_begin, TCounter& count)
  {
    TOutputIterator o_end = tpn::uninitialized_move(i_begin, i_end, o_begin);

    count += int32_t(tpn::distance(i_begin, i_end));

    return o_end;
  }
#else
  //*****************************************************************************
  /// Moves a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_move
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TOutputIterator>
  TOutputIterator  uninitialized_move(TInputIterator i_begin, TInputIterator i_end, TOutputIterator o_begin)
  {
    return std::uninitialized_move(i_begin, i_end, o_begin);
  }

  //*****************************************************************************
  /// Moves a range of objects to uninitialised memory.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_move
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TOutputIterator, typename TCounter>
  TOutputIterator uninitialized_move(TInputIterator i_begin, TInputIterator i_end, TOutputIterator o_begin, TCounter& count)
  {
    count += int32_t(tpn::distance(i_begin, i_end));

    return std::uninitialized_move(i_begin, i_end, o_begin);
  }
#endif
#else
  // C++03
  //*****************************************************************************
  /// Moves a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_move
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TOutputIterator>
  TOutputIterator  uninitialized_move(TInputIterator i_begin, TInputIterator i_end, TOutputIterator o_begin)
  {
    // Move not supported. Defer to copy.
    return TYPHOON_OR_STD::uninitialized_copy(i_begin, i_end, o_begin);
  }

  //*****************************************************************************
  /// Moves a range of objects to uninitialised memory.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_move
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TOutputIterator, typename TCounter>
  TOutputIterator uninitialized_move(TInputIterator i_begin, TInputIterator i_end, TOutputIterator o_begin, TCounter& count)
  {
    count += int32_t(tpn::distance(i_begin, i_end));

    // Move not supported. Defer to copy.
    return TYPHOON_OR_STD::uninitialized_copy(i_begin, i_end, o_begin);
  }
#endif

#if TYPHOON_USING_CPP11
#if TYPHOON_NOT_USING_STL || TYPHOON_CPP17_NOT_SUPPORTED
  //*****************************************************************************
  /// Moves a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_move_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TSize, typename TOutputIterator>
  typename tpn::enable_if<tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, TOutputIterator>::type
    uninitialized_move_n(TInputIterator i_begin, TSize n, TOutputIterator o_begin)
  {
    return tpn::move(i_begin, i_begin + n, o_begin);
  }

  //*****************************************************************************
  /// Moves a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_move_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TSize, typename TOutputIterator>
  typename tpn::enable_if<!tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, TOutputIterator>::type
    uninitialized_move_n(TInputIterator i_begin, TSize n, TOutputIterator o_begin)
  {
    typedef typename tpn::iterator_traits<TOutputIterator>::value_type value_type;

    TOutputIterator o_end = o_begin;

    while (n-- != 0)
    {
      ::new (static_cast<void*>(tpn::addressof(*o_end))) value_type(tpn::move(*i_begin));
      ++i_begin;
      ++o_end;
    }

    return o_end;
  }

  //*****************************************************************************
  /// Moves a range of objects to uninitialised memory.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_move_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TSize, typename TOutputIterator, typename TCounter>
  typename tpn::enable_if<tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, TOutputIterator>::type
    uninitialized_move_n(TInputIterator i_begin, TSize n, TOutputIterator o_begin, TCounter& count)
  {
    TOutputIterator o_end = tpn::move(i_begin, i_begin + n, o_begin);
    count += TCounter(n);

    return o_end;
  }

  //*****************************************************************************
  /// Moves a range of objects to uninitialised memory.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_move_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TSize, typename TOutputIterator, typename TCounter>
  typename tpn::enable_if<!tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, TOutputIterator>::type
    uninitialized_move_n(TInputIterator i_begin, TSize n, TOutputIterator o_begin, TCounter& count)
  {
    TOutputIterator o_end = tpn::uninitialized_move(i_begin, i_begin + n, o_begin);

    count += TCounter(n);

    return o_end;
  }
#else
  //*****************************************************************************
  /// Moves a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_move_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TSize, typename TOutputIterator>
  TOutputIterator  uninitialized_move_n(TInputIterator i_begin, TSize n, TOutputIterator o_begin)
  {
    return std::uninitialized_move(i_begin, i_begin + n, o_begin);
  }

  //*****************************************************************************
  /// Moves a range of objects to uninitialised memory.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_move_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TSize, typename TOutputIterator, typename TCounter>
  TOutputIterator uninitialized_move_n(TInputIterator i_begin, TSize n, TOutputIterator o_begin, TCounter& count)
  {
    count += TCounter(n);

    return std::uninitialized_move(i_begin, i_begin + n, o_begin);
  }
#endif
#else
  // C++03
  //*****************************************************************************
  /// Moves a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_move_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TSize, typename TOutputIterator>
  TOutputIterator  uninitialized_move_n(TInputIterator i_begin, TSize n, TOutputIterator o_begin)
  {
    // Move not supported. Defer to copy.
#if TYPHOON_USING_CPP11
    return std::uninitialized_copy_n(i_begin, n, o_begin);
#else
    return tpn::uninitialized_copy_n(i_begin, n, o_begin);
#endif
  }

  //*****************************************************************************
  /// Moves a range of objects to uninitialised memory.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_move
  ///\ingroup memory
  //*****************************************************************************
  template <typename TInputIterator, typename TSize, typename TOutputIterator, typename TCounter>
  TOutputIterator uninitialized_move_n(TInputIterator i_begin, TSize n, TOutputIterator o_begin, TCounter& count)
  {
    count += TCounter(n);

    // Move not supported. Defer to copy.
#if TYPHOON_USING_CPP11
    return std::uninitialized_copy_n(i_begin, n, o_begin);
#else
    return tpn::uninitialized_copy_n(i_begin, n, o_begin);
#endif
  }
#endif

#if TYPHOON_NOT_USING_STL || TYPHOON_CPP17_NOT_SUPPORTED
  //*****************************************************************************
  /// Default initialises a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_default_construct
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator>
  typename tpn::enable_if<tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, void>::type
    uninitialized_default_construct(TOutputIterator /*o_begin*/, TOutputIterator /*o_end*/)
  {
    // Do nothing
  }

  //*****************************************************************************
  /// Default initialises a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_default_construct
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator>
  typename tpn::enable_if<!tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, void>::type
    uninitialized_default_construct(TOutputIterator o_begin, TOutputIterator o_end)
  {

    typedef typename tpn::iterator_traits<TOutputIterator>::value_type value_type;

    while (o_begin != o_end)
    {
      ::new (static_cast<void*>(tpn::addressof(*o_begin))) value_type;
      ++o_begin;
    }
  }

  //*****************************************************************************
  /// Default initialises a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_default_construct
  /// Debug counter version.
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename TCounter>
  typename tpn::enable_if<tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, void>::type
    uninitialized_default_construct(TOutputIterator o_begin, TOutputIterator o_end, TCounter& count)
  {
    count = int32_t(tpn::distance(o_begin, o_end));
  }

  //*****************************************************************************
  /// Default initialises a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_default_construct
  /// Debug counter version.
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename TCounter>
  typename tpn::enable_if<!tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, void>::type
    uninitialized_default_construct(TOutputIterator o_begin, TOutputIterator o_end, TCounter& count)
  {
    count += int32_t(tpn::distance(o_begin, o_end));

    tpn::uninitialized_default_construct(o_begin, o_end);
  }
#else
  //*****************************************************************************
  /// Default initialises a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_default_construct
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator>
  typename tpn::enable_if<!tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, void>::type
  uninitialized_default_construct(TOutputIterator o_begin, TOutputIterator o_end)
  {
    std::uninitialized_default_construct(o_begin, o_end);
  }

  //*****************************************************************************
  /// Default initialises a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_default_construct
  /// Debug counter version.
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename TCounter>
  typename tpn::enable_if<tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, void>::type
    uninitialized_default_construct(TOutputIterator o_begin, TOutputIterator o_end, TCounter& count)
  {
    count = int32_t(tpn::distance(o_begin, o_end));

    std::uninitialized_default_construct(o_begin, o_end);
  }
#endif

#if TYPHOON_NOT_USING_STL || TYPHOON_CPP17_NOT_SUPPORTED
  //*****************************************************************************
  /// Default initialises N objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_default_construct_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename TSize>
  typename tpn::enable_if<tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, TOutputIterator>::type
    uninitialized_default_construct_n(TOutputIterator o_begin, TSize n)
  {
    TOutputIterator o_end = o_begin + n;
    return o_end;
  }

  //*****************************************************************************
  /// Default initialises N objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_default_construct_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename TSize>
  typename tpn::enable_if<!tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, TOutputIterator>::type
    uninitialized_default_construct_n(TOutputIterator o_begin, TSize n)
  {
    TOutputIterator o_end = o_begin + n;

    tpn::uninitialized_default_construct(o_begin, o_end);

    return o_end;
  }

  //*****************************************************************************
  /// Default initialises N objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_default_construct_n
  /// Debug counter version.
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename TSize, typename TCounter>
  typename tpn::enable_if<tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, TOutputIterator>::type
    uninitialized_default_construct_n(TOutputIterator o_begin, TSize n, TCounter& count)
  {
    TOutputIterator o_end = o_begin + n;

    count += n;

    return o_end;
  }

  //*****************************************************************************
  /// Default initialises N objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_default_construct_n
  /// Debug counter version.
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename TSize, typename TCounter>
  typename tpn::enable_if<!tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, TOutputIterator>::type
    uninitialized_default_construct_n(TOutputIterator o_begin, TSize n, TCounter& count)
  {
    TOutputIterator o_end = o_begin + n;

    tpn::uninitialized_default_construct(o_begin, o_end);

    count += n;

    return o_end;
  }
#else
  //*****************************************************************************
  /// Default initialises N objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_default_construct_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename TSize>
  TOutputIterator uninitialized_default_construct_n(TOutputIterator o_begin, TSize n)
  {
    return std::uninitialized_default_construct_n(o_begin, n);
  }

  //*****************************************************************************
  /// Default initialises N objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_default_construct_n
  /// Debug counter version.
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename TSize, typename TCounter>
  TOutputIterator uninitialized_default_construct_n(TOutputIterator o_begin, TSize n, TCounter& count)
  {
    count += n;

    return std::uninitialized_default_construct_n(o_begin, n);
  }
#endif

#if TYPHOON_NOT_USING_STL || TYPHOON_CPP17_NOT_SUPPORTED
  //*****************************************************************************
  /// Default initialises a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_value_construct
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator>
  typename tpn::enable_if<tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, void>::type
    uninitialized_value_construct(TOutputIterator o_begin, TOutputIterator o_end)
  {
    typedef typename tpn::iterator_traits<TOutputIterator>::value_type value_type;

    tpn::fill(o_begin, o_end, value_type());
  }

  //*****************************************************************************
  /// Default initialises a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_value_construct
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator>
  typename tpn::enable_if<!tpn::is_trivially_constructible<typename tpn::iterator_traits<TOutputIterator>::value_type>::value, void>::type
    uninitialized_value_construct(TOutputIterator o_begin, TOutputIterator o_end)
  {
    typedef typename tpn::iterator_traits<TOutputIterator>::value_type value_type;

    while (o_begin != o_end)
    {
      ::new (static_cast<void*>(tpn::addressof(*o_begin))) value_type();
      ++o_begin;
    }
  }

  //*****************************************************************************
  /// Default initialises a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_value_construct
  /// Debug counter version.
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename TCounter>
  void uninitialized_value_construct(TOutputIterator o_begin, TOutputIterator o_end, TCounter& count)
  {
    count += int32_t(tpn::distance(o_begin, o_end));

    tpn::uninitialized_value_construct(o_begin, o_end);
  }
#else
  //*****************************************************************************
  /// Default initialises a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_value_construct
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator>
  void uninitialized_value_construct(TOutputIterator o_begin, TOutputIterator o_end)
  {
    std::uninitialized_value_construct(o_begin, o_end);
  }

  //*****************************************************************************
  /// Default initialises a range of objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_value_construct
  /// Debug counter version.
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename TCounter>
  void uninitialized_value_construct(TOutputIterator o_begin, TOutputIterator o_end, TCounter& count)
  {
    count += int32_t(tpn::distance(o_begin, o_end));

    std::uninitialized_value_construct(o_begin, o_end);
  }

#endif

#if TYPHOON_NOT_USING_STL || TYPHOON_CPP17_NOT_SUPPORTED
  //*****************************************************************************
  /// Default initialises N objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_value_construct_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename TSize>
  TOutputIterator uninitialized_value_construct_n(TOutputIterator o_begin, TSize n)
  {
    TOutputIterator o_end = o_begin + n;

    tpn::uninitialized_value_construct(o_begin, o_end);

    return o_end;
  }

  //*****************************************************************************
  /// Default initialises N objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_value_construct_n
  /// Debug counter version.
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename TSize, typename TCounter>
  TOutputIterator uninitialized_value_construct_n(TOutputIterator o_begin, TSize n, TCounter& count)
  {
    TOutputIterator o_end = o_begin + n;

    tpn::uninitialized_value_construct(o_begin, o_end);

    count += n;

    return o_end;
  }
#else
  //*****************************************************************************
  /// Default initialises N objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_value_construct_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename TSize>
  TOutputIterator uninitialized_value_construct_n(TOutputIterator o_begin, TSize n)
  {
    return std::uninitialized_value_construct_n(o_begin, n);
  }

  //*****************************************************************************
  /// Default initialises N objects to uninitialised memory.
  /// https://en.cppreference.com/w/cpp/memory/uninitialized_value_construct_n
  /// Debug counter version.
  ///\ingroup memory
  //*****************************************************************************
  template <typename TOutputIterator, typename TSize, typename TCounter>
  TOutputIterator uninitialized_value_construct_n(TOutputIterator o_begin, TSize n, TCounter& count)
  {
    count += n;

    return std::uninitialized_value_construct_n(o_begin, n);
  }
#endif

#if TYPHOON_NOT_USING_STL || TYPHOON_CPP20_NOT_SUPPORTED
#if TYPHOON_USING_CPP11
  //*****************************************************************************
  /// Constructs an item at address p with value constructed from 'args'.
  /// https://en.cppreference.com/w/cpp/memory/construct_at
  ///\ingroup memory
  //*****************************************************************************
  template <typename T, typename... TArgs>
  TYPHOON_CONSTEXPR T* construct_at(T* p, TArgs&&... args)
  {
    return ::new (const_cast<void*>(static_cast<const volatile void*>(p))) T(tpn::forward<TArgs>(args)...);
  }
#else
  //*****************************************************************************
  /// Constructs an item at address p.
  /// https://en.cppreference.com/w/cpp/memory/construct_at
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR T* construct_at(T* p)
  {
    return ::new (const_cast<void*>(static_cast<const volatile void*>(p))) T();
  }
  //*****************************************************************************
  /// Constructs an item at address p with value 'arg'.
  /// https://en.cppreference.com/w/cpp/memory/construct_at
  ///\ingroup memory
  //*****************************************************************************
  template <typename T, typename TArg>
  TYPHOON_CONSTEXPR T* construct_at(T* p, const TArg& arg)
  {
    return ::new (const_cast<void*>(static_cast<const volatile void*>(p))) T(arg);
  }
#endif
#else
  template <typename T, typename... TArgs>
  TYPHOON_CONSTEXPR20 T* construct_at(T* p, TArgs&&... args)
  {
    return std::construct_at(p, tpn::forward<TArgs>(args)...);
  }
#endif

#if TYPHOON_NOT_USING_STL || TYPHOON_CPP17_NOT_SUPPORTED
  //*****************************************************************************
  /// Destroys an item at address p.
  /// https://en.cppreference.com/w/cpp/memory/destroy_at
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
  typename tpn::enable_if<tpn::is_trivially_destructible<T>::value, void>::type
    destroy_at(T* /*p*/)
  {
  }

  //*****************************************************************************
  /// Destroys an item at address p.
  /// https://en.cppreference.com/w/cpp/memory/destroy_at
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
  typename tpn::enable_if<!tpn::is_trivially_destructible<T>::value, void>::type
    destroy_at(T* p)
  {
    p->~T();
  }

  //*****************************************************************************
  /// Destroys an item at address p.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/destroy_at
  ///\ingroup memory
  //*****************************************************************************
  template <typename T, typename TCounter>
  typename tpn::enable_if<tpn::is_trivially_destructible<T>::value, void>::type
    destroy_at(T* /*p*/, TCounter& count)
  {
    --count;
  }

  //*****************************************************************************
  /// Destroys an item at address p.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/destroy_at
  ///\ingroup memory
  //*****************************************************************************
  template <typename T, typename TCounter>
  typename tpn::enable_if<!tpn::is_trivially_destructible<T>::value, void>::type
    destroy_at(T* p, TCounter& count)
  {
    p->~T();
    --count;
  }
#else
  //*****************************************************************************
  /// Destroys an item at address p.
  /// https://en.cppreference.com/w/cpp/memory/destroy_at
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
  void destroy_at(T* p)
  {
    std::destroy_at(p);
  }

  //*****************************************************************************
  /// Destroys an item at address p.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/destroy_at
  ///\ingroup memory
  //*****************************************************************************
  template <typename T, typename TCounter>
  void destroy_at(T* p, TCounter& count)
  {
    --count;
    std::destroy_at(p);
  }
#endif

#if TYPHOON_NOT_USING_STL || TYPHOON_CPP17_NOT_SUPPORTED
  //*****************************************************************************
  /// Destroys a range of items.
  /// https://en.cppreference.com/w/cpp/memory/destroy
  ///\ingroup memory
  //*****************************************************************************
  template <typename TIterator>
  typename tpn::enable_if<tpn::is_trivially_destructible<typename tpn::iterator_traits<TIterator>::value_type>::value, void>::type
    destroy(TIterator /*i_begin*/, TIterator /*i_end*/)
  {
  }

  //*****************************************************************************
  /// Destroys a range of items.
  /// https://en.cppreference.com/w/cpp/memory/destroy
  ///\ingroup memory
  //*****************************************************************************
  template <typename TIterator>
  typename tpn::enable_if<!tpn::is_trivially_destructible<typename tpn::iterator_traits<TIterator>::value_type>::value, void>::type
    destroy(TIterator i_begin, TIterator i_end)
  {
    while (i_begin != i_end)
    {
      tpn::destroy_at(tpn::addressof(*i_begin));
      ++i_begin;
    }
  }

  //*****************************************************************************
  /// Destroys a range of items.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/destroy
  ///\ingroup memory
  //*****************************************************************************
  template <typename TIterator, typename TCounter>
  typename tpn::enable_if<tpn::is_trivially_destructible<typename tpn::iterator_traits<TIterator>::value_type>::value, void>::type
    destroy(TIterator i_begin, TIterator i_end, TCounter& count)
  {
    count -= int32_t(tpn::distance(i_begin, i_end));
  }

  //*****************************************************************************
  /// Destroys a range of items.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/destroy
  ///\ingroup memory
  //*****************************************************************************
  template <typename TIterator, typename TCounter>
  typename tpn::enable_if<!tpn::is_trivially_destructible<typename tpn::iterator_traits<TIterator>::value_type>::value, void>::type
    destroy(TIterator i_begin, TIterator i_end, TCounter& count)
  {
    count -= int32_t(tpn::distance(i_begin, i_end));

    while (i_begin != i_end)
    {
      tpn::destroy_at(tpn::addressof(*i_begin));
      ++i_begin;
    }
  }
#else
  //*****************************************************************************
  /// Destroys a range of items.
  /// https://en.cppreference.com/w/cpp/memory/destroy
  ///\ingroup memory
  //*****************************************************************************
  template <typename TIterator>
  void destroy(TIterator i_begin, TIterator i_end)
  {
    std::destroy(i_begin, i_end);
  }

  //*****************************************************************************
  /// Destroys a range of items.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/destroy
  ///\ingroup memory
  //*****************************************************************************
  template <typename TIterator, typename TCounter>
  void destroy(TIterator i_begin, TIterator i_end, TCounter& count)
  {
    count -= int32_t(tpn::distance(i_begin, i_end));

    std::destroy(i_begin, i_end);
  }
#endif

#if TYPHOON_NOT_USING_STL || TYPHOON_CPP17_NOT_SUPPORTED
  //*****************************************************************************
  /// Destroys a number of items.
  /// https://en.cppreference.com/w/cpp/memory/destroy_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TIterator, typename TSize>
  typename tpn::enable_if<tpn::is_trivially_destructible<typename tpn::iterator_traits<TIterator>::value_type>::value, TIterator>::type
    destroy_n(TIterator i_begin, TSize n)
  {
    return i_begin + n;
  }

  //*****************************************************************************
  /// Destroys a number of items.
  /// https://en.cppreference.com/w/cpp/memory/destroy_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TIterator, typename TSize>
  typename tpn::enable_if<!tpn::is_trivially_destructible<typename tpn::iterator_traits<TIterator>::value_type>::value, TIterator>::type
    destroy_n(TIterator i_begin, TSize n)
  {
    while (n > 0)
    {
      tpn::destroy_at(tpn::addressof(*i_begin));
      ++i_begin;
      --n;
    }

    return i_begin;
  }

  //*****************************************************************************
  /// Destroys a number of items.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/destroy_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TIterator, typename TSize, typename TCounter>
  typename tpn::enable_if<tpn::is_trivially_destructible<typename tpn::iterator_traits<TIterator>::value_type>::value, TIterator>::type
    destroy_n(TIterator i_begin, TSize n, TCounter& count)
  {
    count -= n;
    return i_begin + n;
  }

  //*****************************************************************************
  /// Destroys a number of items.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/destroy_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TIterator, typename TSize, typename TCounter>
  typename tpn::enable_if<!tpn::is_trivially_destructible<typename tpn::iterator_traits<TIterator>::value_type>::value, TIterator>::type
    destroy_n(TIterator i_begin, TSize n, TCounter& count)
  {
    count -= n;

    while (n > 0)
    {
      tpn::destroy_at(tpn::addressof(*i_begin));
      ++i_begin;
      --n;
    }

    return i_begin;
  }
#else
  //*****************************************************************************
  /// Destroys a number of items.
  /// https://en.cppreference.com/w/cpp/memory/destroy_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TIterator, typename TSize>
  TIterator destroy_n(TIterator i_begin, TSize n)
  {
    return std::destroy_n(i_begin, n);
  }

  //*****************************************************************************
  /// Destroys a number of items.
  /// Debug counter version.
  /// https://en.cppreference.com/w/cpp/memory/destroy_n
  ///\ingroup memory
  //*****************************************************************************
  template <typename TIterator, typename TSize, typename TCounter>
  TIterator destroy_n(TIterator i_begin, TSize n, TCounter& count)
  {
    count -= n;

    return std::destroy_n(i_begin, n);
  }
#endif

  //*****************************************************************************
  /// Default deleter.
  ///\tparam T The pointed to type type.
  /// https://en.cppreference.com/w/cpp/memory/default_delete
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
  struct default_delete
  {
    //*********************************
    TYPHOON_CONSTEXPR default_delete() TYPHOON_NOEXCEPT
    {
    }

    //*********************************
    template <typename U>
    default_delete(const default_delete<U>&) TYPHOON_NOEXCEPT 
    {
    }

    //*********************************
    void operator()(T * p) const TYPHOON_NOEXCEPT
    {
      delete p;
    }
  };

  //*****************************************************************************
  /// Default deleter for arrays.
  ///\tparam T The pointed to type type.
  /// https://en.cppreference.com/w/cpp/memory/default_delete
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
  struct default_delete<T[]>
  {
    //*********************************
    TYPHOON_CONSTEXPR default_delete() TYPHOON_NOEXCEPT
    {
    }

    //*********************************
    template <typename U>
    default_delete(const default_delete<U>&) TYPHOON_NOEXCEPT
    {
    }

    //*********************************
    template <class U>
    void operator()(U* p) const
    {
      delete[] p;
    }
  };

  //*****************************************************************************
  /// Unique pointer.
  ///\tparam T The pointed to type type.
  /// https://en.cppreference.com/w/cpp/memory/unique_ptr
  ///\ingroup memory
  //*****************************************************************************
  template <typename T, typename TDeleter = tpn::default_delete<T> >
  class unique_ptr
  {
  public:

    typedef T  element_type;
    typedef T* pointer;
    typedef T& reference;

    //*********************************
    TYPHOON_CONSTEXPR unique_ptr() TYPHOON_NOEXCEPT
      : p(TYPHOON_NULLPTR)
    {
    }

    //*********************************
    TYPHOON_CONSTEXPR explicit unique_ptr(pointer p_) TYPHOON_NOEXCEPT
      : p(p_)
    {
    }

#if TYPHOON_USING_CPP11
    //*********************************
    unique_ptr(unique_ptr&& other) TYPHOON_NOEXCEPT
    {
      if (&other != this)
      {
        p = other.release();
        deleter = tpn::move(other.deleter);
      }
    }
#else
    //*********************************
    unique_ptr(unique_ptr& other) TYPHOON_NOEXCEPT
    {
      if (&other != this)
      {
        p = other.release();
        deleter = other.deleter;
      }
    }
#endif

    //*********************************
    unique_ptr(pointer p_, typename tpn::conditional<tpn::is_reference<TDeleter>::value, 
                                                     TDeleter, 
                                                     typename tpn::add_lvalue_reference<const TDeleter>::type>::type deleter_) TYPHOON_NOEXCEPT
      : p(p_)
      , deleter(deleter_)
    {
    }

#if TYPHOON_USING_CPP11
    //*********************************
    unique_ptr(pointer p_, typename tpn::remove_reference<TDeleter>::type&& deleter_) TYPHOON_NOEXCEPT
      : p(p_)
      , deleter(tpn::move(deleter_))
    {
    }

    template <typename U, typename E>
    unique_ptr(unique_ptr<U, E>&& u) TYPHOON_NOEXCEPT
      : p(u.release())
      , deleter(tpn::forward<E>(u.get_deleter()))
    {
    }
#endif

    //*********************************
    ~unique_ptr()
    {
      if (p != TYPHOON_NULLPTR)
      {
        deleter(p);
      }
    }

    //*********************************
    TYPHOON_CONSTEXPR pointer	get() const TYPHOON_NOEXCEPT
    {
      return p;
    }

    //*********************************
    TDeleter& get_deleter() TYPHOON_NOEXCEPT
    {
      return deleter;
    }

    //*********************************
    const TDeleter& get_deleter() const TYPHOON_NOEXCEPT
    {
      return deleter;
    }

    //*********************************
    pointer	release() TYPHOON_NOEXCEPT
    {
      pointer value = p;
      p = TYPHOON_NULLPTR;

      return value;
    }

    //*********************************
    void reset(pointer p_ = pointer()) TYPHOON_NOEXCEPT
    {
      assert(p_ != p);

      pointer value = p;
      p = p_;
      deleter(value);
    }

    //*********************************
    void swap(unique_ptr& value) TYPHOON_NOEXCEPT
    {
      using TYPHOON_OR_STD::swap;

      swap(p, value.p);
    }

    //*********************************
    TYPHOON_CONSTEXPR operator bool() const TYPHOON_NOEXCEPT
    {
      return (p != TYPHOON_NULLPTR);
    }

#if TYPHOON_USING_STL && TYPHOON_USING_CPP11
    //*********************************
    unique_ptr&	operator =(std::nullptr_t) TYPHOON_NOEXCEPT
    {
      reset(nullptr);

      return *this;
    }
#else
    //*********************************
    unique_ptr&	operator =(void*) TYPHOON_NOEXCEPT
    {
      reset(NULL);

      return *this;
    }
#endif

#if TYPHOON_USING_CPP11
    //*********************************
    unique_ptr&	operator =(unique_ptr&& other) TYPHOON_NOEXCEPT
    {
      if (&other != this)
      {
        reset(other.release());
        deleter = tpn::move(other.deleter);
      }

      return *this;
    }
#else
    //*********************************
    unique_ptr& operator =(unique_ptr& other) TYPHOON_NOEXCEPT
    {
      if (&other != this)
      {
        reset(other.release());
        deleter = other.deleter;
      }

      return *this;
    }
#endif

    //*********************************
    TYPHOON_CONSTEXPR reference	operator *() const
    {
      return *get();
    }

    //*********************************
    TYPHOON_CONSTEXPR pointer	operator ->() const TYPHOON_NOEXCEPT
    {
      return get();
    }

    //*********************************
    TYPHOON_CONSTEXPR reference	operator [](size_t i) const
    {
      return p[i];
    }

  private:

    // Deleted.
    unique_ptr(const unique_ptr&) TYPHOON_DELETE;
    unique_ptr&	operator =(const unique_ptr&) TYPHOON_DELETE;

    pointer	 p;
    TDeleter deleter;
  };

  //*****************************************************************************
  /// Unique pointer for arrays.
  ///\tparam T The pointed to type type.
  /// https://en.cppreference.com/w/cpp/memory/unique_ptr
  ///\ingroup memory
  //*****************************************************************************
  template<typename T, typename TDeleter>
  class unique_ptr<T[], TDeleter>
  {
  public:

    typedef T  element_type;
    typedef T* pointer;
    typedef T& reference;

    //*********************************
    TYPHOON_CONSTEXPR	unique_ptr() TYPHOON_NOEXCEPT
      : p(TYPHOON_NULLPTR)
    {
    }

    //*********************************
    TYPHOON_CONSTEXPR explicit unique_ptr(pointer p_) TYPHOON_NOEXCEPT
      : p(p_)
    {
    }

#if TYPHOON_USING_CPP11
    //*********************************
    unique_ptr(unique_ptr&& other) TYPHOON_NOEXCEPT
    {
      if (&other != this)
      {
        p = other.release();
        deleter = tpn::move(other.deleter);
      }
    }
#else
    //*********************************
    unique_ptr(unique_ptr& other) TYPHOON_NOEXCEPT
    {
      if (&other != this)
      {
        p = other.release();
        deleter = other.deleter;
      }
    }
#endif

    //*********************************
    unique_ptr(pointer p_, 
               typename tpn::conditional<tpn::is_reference<TDeleter>::value,
                                         TDeleter,
                                         typename tpn::add_lvalue_reference<const TDeleter>::type>::type deleter_) TYPHOON_NOEXCEPT
      : p(p_)
      , deleter(deleter_)
    {
    }

#if TYPHOON_USING_CPP11
    //*********************************
    unique_ptr(pointer p_, typename tpn::remove_reference<TDeleter>::type&& deleter_) TYPHOON_NOEXCEPT
      : p(p_)
      , deleter(tpn::move(deleter_))
    {
    }

    template <typename U, typename E>
    unique_ptr(unique_ptr<U, E>&& u) TYPHOON_NOEXCEPT
      : p(u.release())
      , deleter(tpn::forward<E>(u.get_deleter()))
    {
    }
#endif

    //*********************************
    ~unique_ptr()
    {
      if (p != TYPHOON_NULLPTR)
      {
        deleter(p);
      }
    }

    //*********************************
    TYPHOON_CONSTEXPR pointer	get() const TYPHOON_NOEXCEPT
    {
      return p;
    }

    //*********************************
    TDeleter& get_deleter() TYPHOON_NOEXCEPT
    {
      return deleter;
    }

    //*********************************
    const TDeleter& get_deleter() const TYPHOON_NOEXCEPT
    {
      return deleter;
    }

    //*********************************
    pointer	release() TYPHOON_NOEXCEPT
    {
      pointer value = p;
      p = TYPHOON_NULLPTR;
      return value;
    }

    //*********************************
    void reset(pointer p_) TYPHOON_NOEXCEPT
    {
      assert(p_ != p);

      pointer value = p;
      p = p_;
      delete[] value;
    }

    //*********************************
    void swap(unique_ptr& v) TYPHOON_NOEXCEPT
    {
      using TYPHOON_OR_STD::swap;

      swap(p, v.p);
    }

    //*********************************
    TYPHOON_CONSTEXPR operator bool() const TYPHOON_NOEXCEPT
    {
      return (p != TYPHOON_NULLPTR);
    }

#if TYPHOON_USING_STL && TYPHOON_USING_CPP11
    //*********************************
    unique_ptr& operator =(std::nullptr_t) TYPHOON_NOEXCEPT
    {
      reset(nullptr);

      return *this;
    }
#else
    //*********************************
    unique_ptr& operator =(void*) TYPHOON_NOEXCEPT
    {
      reset(NULL);

      return *this;
    }
#endif

#if TYPHOON_USING_CPP11
    //*********************************
    unique_ptr& operator =(unique_ptr&& other) TYPHOON_NOEXCEPT
    {
      if (&other != this)
      {
        reset(other.release());
        deleter = tpn::move(other.deleter);
      }

      return *this;
    }
#else
    //*********************************
    unique_ptr& operator =(unique_ptr& other) TYPHOON_NOEXCEPT
    {
      if (&other != this)
      {
        reset(other.release());
        deleter = other.deleter;
      }

      return *this;
    }
#endif

    //*********************************
    TYPHOON_CONSTEXPR reference	operator *() const
    {
      return *p;
    }

    //*********************************
    TYPHOON_CONSTEXPR pointer	operator ->() const TYPHOON_NOEXCEPT
    {
      return p;
    }

    //*********************************
    TYPHOON_CONSTEXPR reference	operator [](size_t i) const
    {
      return p[i];
    }

  private:

    // Deleted.
    unique_ptr(const unique_ptr&) TYPHOON_DELETE;
    unique_ptr&	operator =(const unique_ptr&) TYPHOON_DELETE;

    pointer	p; 
    TDeleter deleter;
  };
}

//*****************************************************************************
// Global functions for unique_ptr
//*****************************************************************************
template<typename T1, typename TD1, typename T2, typename TD2>
bool operator ==(const tpn::unique_ptr<T1, TD1>&lhs, const tpn::unique_ptr<T2, TD2>& rhs)
{
  return lhs.get() == rhs.get();
}

//*********************************
template<typename T1, typename TD1, typename T2, typename TD2>
bool operator <(const tpn::unique_ptr<T1, TD1>&lhs, const tpn::unique_ptr<T2, TD2>& rhs)
{
  return reinterpret_cast<char*>(lhs.get()) < reinterpret_cast<char*>(rhs.get());
}

//*********************************
template<typename T1, typename TD1, typename T2, typename TD2>
bool operator <=(const tpn::unique_ptr<T1, TD1>&lhs, const tpn::unique_ptr<T2, TD2>& rhs)
{
  return !(rhs < lhs);
}

//*********************************
template<typename T1, typename TD1, typename T2, typename TD2>
bool operator >(const tpn::unique_ptr<T1, TD1>&lhs, const tpn::unique_ptr<T2, TD2>& rhs)
{
  return (rhs < lhs);
}

//*********************************
template<typename T1, typename TD1, typename T2, typename TD2>
bool operator >=(const tpn::unique_ptr<T1, TD1>&lhs, const tpn::unique_ptr<T2, TD2>& rhs)
{
  return !(lhs < rhs);
}

namespace tpn
{
  //*****************************************************************************
  /// Default construct an item at address p.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
  typename tpn::enable_if<tpn::is_trivially_constructible<T>::value, void>::type
   create_default_at(T* /*p*/)
  {
  }

  //*****************************************************************************
  /// Default construct an item at address p.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T, typename TCounter>
  typename tpn::enable_if<tpn::is_trivially_constructible<T>::value, void>::type
   create_default_at(T* /*p*/, TCounter& count)
  {
    ++count;
  }

  //*****************************************************************************
  /// Default construct an item at address p.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
  typename tpn::enable_if<!tpn::is_trivially_constructible<T>::value, void>::type
   create_default_at(T* p)
  {
    ::new (p) T;
  }

  //*****************************************************************************
  /// Default construct an item at address p.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T, typename TCounter>
  typename tpn::enable_if<!tpn::is_trivially_constructible<T>::value, void>::type
   create_default_at(T* p, TCounter& count)
  {
    ::new (p) T;
    ++count;
  }

  //*****************************************************************************
  /// Value construct an item at address p.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
  void create_value_at(T* p)
  {
    ::new (p) T();
  }

  //*****************************************************************************
  /// Value construct an item at address p.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T, typename TCounter>
  void create_value_at(T* p, TCounter& count)
  {
    ::new (p) T();
    ++count;
  }

  //*****************************************************************************
  /// Copy construct an item at address p.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
  void create_copy_at(T* p, const T& value)
  {
    ::new (p) T(value);
  }

#if TYPHOON_USING_CPP11
  //*****************************************************************************
  /// Copy construct an item at address p.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
  void create_copy_at(T* p, T&& value)
  {
    ::new (p) T(tpn::move(value));
  }
#endif

  //*****************************************************************************
  /// Copy construct an item at address p.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T, typename TCounter>
   void create_copy_at(T* p, const T& value, TCounter& count)
  {
    ::new (p) T(value);
    ++count;
  }

  //*****************************************************************************
  /// Construct an item at address p.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
   T& make_default_at(T* p)
  {
    ::new (p) T();
    return *reinterpret_cast<T*>(p);
  }

  //*****************************************************************************
  /// Construct an item at address p.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T, typename TCounter>
   T& make_default_at(T* p, TCounter& count)
  {
    ::new (p) T();
    ++count;
    return *reinterpret_cast<T*>(p);
  }

  //*****************************************************************************
  /// Construct an item at address p.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
   T& make_copy_at(T* p, const T& other)
  {
    ::new (p) T(other);
    return *reinterpret_cast<T*>(p);
  }

#if TYPHOON_USING_CPP11
  //*****************************************************************************
  /// Construct an item at address p.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
  T& make_copy_at(T* p, T&& other)
  {
    ::new (p) T(tpn::move(other));
    return *reinterpret_cast<T*>(p);
  }
#endif

  //*****************************************************************************
  /// Construct an item at address p.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T, typename TCounter>
   T& make_copy_at(T* p, const T& other, TCounter& count)
  {
    ::new (p) T(other);
    ++count;
    return *reinterpret_cast<T*>(p);
  }

  //*****************************************************************************
  /// Construct an item at address p.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T, typename TParameter>
   T& make_value_at(T* p, const TParameter& value)
  {
    ::new (p) T(value);
    return *reinterpret_cast<T*>(p);
  }

#if TYPHOON_USING_CPP11
  //*****************************************************************************
  /// Construct an item at address p.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T, typename TParameter>
  T& make_value_at(T* p, TParameter&& value)
  {
    ::new (p) T(tpn::move(value));
    return *reinterpret_cast<T*>(p);
  }
#endif

  //*****************************************************************************
  /// Construct an item at address p.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T, typename TParameter, typename TCounter>
   T& make_value_at(T* p, const TParameter& value, TCounter& count)
  {
    ::new (p) T(value);
    ++count;
    return *reinterpret_cast<T*>(p);
  }

  //*****************************************************************************
  /// Copy constructs a derived class to an address.
  ///\tparam T The derived type.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
  struct create_copy
  {
    void create_copy_at(void* p)
    {
      new (p) T(static_cast<const T&>(*this));
    }

    template <typename TCounter>
    void create_copy_at(void* p, TCounter& count)
    {
      new (p) T(static_cast<const T&>(*this));
      ++count;
    }

    T& make_copy_at(void* p)
    {
      new (p) T(static_cast<const T&>(*this));
      return *reinterpret_cast<T*>(p);
    }

    template <typename TCounter>
    T& make_copy_at(void* p, TCounter& count)
    {
      new (p) T(static_cast<const T&>(*this));
      ++count;
      return *reinterpret_cast<T*>(p);
    }
  };

  //*****************************************************************************
  /// A low level function that clears an object's memory to zero.
  ///\param p Pointer to the memory.
  ///\param n Size of the memory.
  ///\ingroup memory
  //*****************************************************************************
  inline void memory_clear(volatile char* p, size_t n)
  {
    while (n--)
    {
      *p++ = 0;
    }
  }

  //*****************************************************************************
  /// A low level function that clears an object's memory to zero.
  ///\tparam T     The type.
  ///\param object The object to clear.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
  void memory_clear(volatile T &object)
  {
    memory_clear(reinterpret_cast<volatile char*>(&object), sizeof(T));
  }

  //*****************************************************************************
  /// A low level function that clears a range to zero.
  ///\tparam T    The type.
  ///\param begin The first object in the range.
  ///\param n     The number of objects.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
  void memory_clear_range(volatile T* begin, size_t n)
  {
    memory_clear(reinterpret_cast<volatile char*>(begin), n * sizeof(T));
  }

  //*****************************************************************************
  /// A low level function that clears a range to zero.
  ///\tparam T    The type.
  ///\param begin The first object in the range.
  ///\param end   One past the last object in the range.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
  void memory_clear_range(volatile T* begin, volatile T* end)
  {
    const size_t n = static_cast<size_t>(tpn::distance(begin, end));

    memory_clear_range(begin, n);
  }

  //*****************************************************************************
  /// A low level function that clears an object's memory to zero.
  ///\param p     Pointer to the memory.
  ///\param n     Size of the memory.
  ///\param value The value to set.
  ///\ingroup memory
  //*****************************************************************************
  inline void memory_set(volatile char* p, size_t n, char value)
  {
    while (n--)
    {
      *p++ = value;
    }
  }

  //*****************************************************************************
  /// A low level function that sets an object's memory to a value.
  ///\tparam T The type.
  ///\param object The object to set.
  ///\param value  The value to set the object's memory to.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
  void memory_set(volatile T &object, const char value)
  {
    memory_set(reinterpret_cast<volatile char*>(&object), sizeof(T), value);
  }

  //*****************************************************************************
  /// A low level function that clears a range to zero.
  ///\tparam T    The type.
  ///\param begin The first object in the range.
  ///\param n     The number of objects.
  ///\param value The value to set the object's memory to.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
  void memory_set_range(volatile T* begin, size_t n, const char value)
  {
    memory_set(reinterpret_cast<volatile char*>(begin), n * sizeof(T), value);
  }

  //*****************************************************************************
  /// A low level function that clears a range to zero.
  ///\tparam T    The type.
  ///\param begin The first object in the range.
  ///\param end   One past the last object in the range.
  ///\param value The value to set the object's memory to.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
  void memory_set_range(volatile T* begin, volatile T* end, const char value)
  {
    const size_t n = static_cast<size_t>(tpn::distance(begin, end));

    memory_set_range(begin, n, value);
  }

  //*****************************************************************************
  /// Base class for objects that require their memory to be wiped after use.
  /// Erases the object's memory to zero.
  /// Note: This may not work for multiply inherited objects.
  ///\tparam T The derived type.
  ///\ingroup memory
  //*****************************************************************************
  template <typename T>
  struct wipe_on_destruct
  {
    ~wipe_on_destruct()
    {
      memory_clear(static_cast<volatile T&>(*this));
    }
  };

  //***************************************************************************
  /// Declares an aligned buffer of N_Objects x of size Object_Size at alignment Alignment.
  ///\ingroup alignment
  //***************************************************************************
  template <size_t VObject_Size, size_t VN_Objects, size_t VAlignment>
  class uninitialized_buffer
  {
  public:

    static TYPHOON_CONSTANT size_t Object_Size = VObject_Size;
    static TYPHOON_CONSTANT size_t N_Objects   = VN_Objects;
    static TYPHOON_CONSTANT size_t Alignment   = VAlignment;

    /// Convert to T reference.
    template <typename T>
    operator T& ()
    {
      TYPHOON_STATIC_ASSERT((tpn::is_same<T*, void*>::value || ((Alignment % tpn::alignment_of<T>::value) == 0)), "Incompatible alignment");
      return *reinterpret_cast<T*>(raw);
    }

    /// Convert to const T reference.
    template <typename T>
    operator const T& () const
    {
      TYPHOON_STATIC_ASSERT((tpn::is_same<T*, void*>::value || ((Alignment % tpn::alignment_of<T>::value) == 0)), "Incompatible alignment");
      return *reinterpret_cast<const T*>(raw);
    }

    /// Convert to T pointer.
    template <typename T>
    operator T* ()
    {
      TYPHOON_STATIC_ASSERT((tpn::is_same<T*, void*>::value || ((Alignment % tpn::alignment_of<T>::value) == 0)), "Incompatible alignment");
      return reinterpret_cast<T*>(raw);
    }

    /// Convert to const T pointer.
    template <typename T>
    operator const T* () const
    {
      TYPHOON_STATIC_ASSERT((tpn::is_same<T*, void*>::value || ((Alignment % tpn::alignment_of<T>::value) == 0)), "Incompatible alignment");
      return reinterpret_cast<const T*>(raw);
    }

#if TYPHOON_USING_CPP11 && !defined(TYPHOON_COMPILER_ARM5) && !defined(TYPHOON_UNINITIALIZED_BUFFER_FORCE_CPP03_IMPLEMENTATION)
    alignas(VAlignment) char raw[Object_Size * N_Objects];
#else
    union
    {
      char raw[VObject_Size * VN_Objects];
      typename tpn::type_with_alignment<Alignment>::type tpn_alignment_type; // A POD type that has the same alignment as VAlignment.
    };
#endif
  };

  //***************************************************************************
  /// Declares an aligned buffer of VN_Objects as if they were type T.
  ///\ingroup alignment
  //***************************************************************************
  template <typename T, size_t VN_Objects>
  class uninitialized_buffer_of
  {
  public:

    typedef T        value_type;
    typedef T&       reference;
    typedef const T& const_reference;
    typedef T*       pointer;
    typedef const T* const_pointer;
    typedef T*       iterator;
    typedef const T* const_iterator;

    static TYPHOON_CONSTANT size_t Object_Size = sizeof(T);
    static TYPHOON_CONSTANT size_t N_Objects   = VN_Objects;
    static TYPHOON_CONSTANT size_t Alignment   = tpn::alignment_of<T>::value;

    /// Index operator.
    T& operator [](int i)
    {
      return reinterpret_cast<T*>(this->raw)[i];
    }

    /// Index operator.
    const T& operator [](int i) const
    {
      return reinterpret_cast<const T*>(this->raw)[i];
    }

    /// Convert to T reference.
    operator T& ()
    {
      return *reinterpret_cast<T*>(raw);
    }

    /// Convert to const T reference.
    operator const T& () const
    {
      return *reinterpret_cast<const T*>(raw);
    }

    /// Convert to T pointer.
    operator T* ()

    {
      return reinterpret_cast<T*>(raw);
    }

    /// Convert to const T pointer.
    operator const T* () const
    {
      return reinterpret_cast<const T*>(raw);
    }

    T* begin()
    {
      return reinterpret_cast<T*>(raw);
    }

    const T* begin() const
    {
      return reinterpret_cast<const T*>(raw);
    }

    T* end()
    {
      return reinterpret_cast<T*>(raw + (sizeof(T) * N_Objects));
    }

    const T* end() const
    {
      return reinterpret_cast<const T*>(raw + (sizeof(T) * N_Objects));
    }

#if TYPHOON_USING_CPP11 && !defined(TYPHOON_COMPILER_ARM5) && !defined(TYPHOON_UNINITIALIZED_BUFFER_FORCE_CPP03_IMPLEMENTATION)
    alignas(Alignment) char raw[sizeof(T) * N_Objects];
#else
    union
    {
      char raw[sizeof(T) * N_Objects];
      typename tpn::type_with_alignment<Alignment>::type tpn_alignment_type; // A POD type that has the same alignment as Alignment.
    };
#endif
  };

#if TYPHOON_USING_CPP11
  template <typename T, size_t N_Objects>
  using uninitialized_buffer_of_t = typename uninitialized_buffer_of<T, N_Objects>::buffer;
#endif

  //***************************************************************************
/// Template wrapper for memcpy.
/// Type must be trivially copyable.
/// \param source begin
/// \param source end
/// \param destination begin
/// \return A pointer to the destination.
//***************************************************************************
  template <typename TPointer>
  typename tpn::enable_if<tpn::is_trivially_copyable<typename tpn::iterator_traits<TPointer>::value_type>::value, TPointer>::type
    mem_copy(const TPointer sb, const TPointer se, TPointer db) TYPHOON_NOEXCEPT
  {
    return reinterpret_cast<TPointer>(memcpy(reinterpret_cast<void*>(db), 
                                             reinterpret_cast<void*>(sb), 
                                             sizeof(typename tpn::iterator_traits<TPointer>::value_type) * static_cast<size_t>(se - sb)));
  }

  //***************************************************************************
  /// Template wrapper for memcpy.
  /// Type must be trivially copyable.
  /// \param source begin
  /// \param source length
  /// \param destination begin
  //***************************************************************************
  template <typename TPointer>
  typename tpn::enable_if<tpn::is_trivially_copyable<typename tpn::iterator_traits<TPointer>::value_type>::value, TPointer>::type
    mem_copy(const TPointer sb, size_t n, TPointer db) TYPHOON_NOEXCEPT
  {
    return reinterpret_cast<TPointer>(memcpy(reinterpret_cast<void*>(db), 
                                             reinterpret_cast<void*>(sb), 
                                             sizeof(typename tpn::iterator_traits<TPointer>::value_type) * n));
  }

  //***************************************************************************
  /// Template wrapper for memmove.
  /// Type must be trivially copyable.
  /// \param source begin
  /// \param source end
  /// \param destination begin
  //***************************************************************************
  template <typename TPointer>
  typename tpn::enable_if<tpn::is_trivially_copyable<typename tpn::iterator_traits<TPointer>::value_type>::value, TPointer>::type
    mem_move(const TPointer sb, const TPointer se, TPointer db) TYPHOON_NOEXCEPT
  {
    return reinterpret_cast<TPointer>(memmove(reinterpret_cast<void*>(db), 
                                              reinterpret_cast<void*>(sb), 
                                              sizeof(typename tpn::iterator_traits<TPointer>::value_type) * static_cast<size_t>(se - sb)));
  }

  //***************************************************************************
  /// Template wrapper for memmove.
  /// Type must be trivially copyable.
  /// \param source begin
  /// \param source length
  /// \param destination begin
  //***************************************************************************
  template <typename TPointer>
  typename tpn::enable_if<tpn::is_trivially_copyable<typename tpn::iterator_traits<TPointer>::value_type>::value, TPointer>::type
    mem_move(const TPointer sb, size_t n, TPointer db) TYPHOON_NOEXCEPT
  {
    return reinterpret_cast<TPointer>(memmove(reinterpret_cast<void*>(db), 
                                              reinterpret_cast<void*>(sb), 
                                              sizeof(typename tpn::iterator_traits<TPointer>::value_type) * n));
  }

  //***************************************************************************
  /// Template wrapper for memcmp.
  /// \param source begin
  /// \param source end
  /// \param destination begin
  /// \return < 0	The first byte that does not match in both memory blocks has a lower value in 'sb' than in 'db' when evaluated as unsigned char values.
  ///   0 The contents of both memory blocks are equal
  /// > 0	The first byte that does not match in both memory blocks has a greater value in 'sb' than in 'db' when evaluated as unsigned char values.
  //***************************************************************************
  template <typename TPointer>
  TYPHOON_NODISCARD
    typename tpn::enable_if<tpn::is_trivially_copyable<typename tpn::iterator_traits<TPointer>::value_type>::value, int>::type
    mem_compare(const TPointer sb, const TPointer se, TPointer db) TYPHOON_NOEXCEPT
  {
    return memcmp(reinterpret_cast<void*>(db), 
                  reinterpret_cast<void*>(sb), 
                  sizeof(typename tpn::iterator_traits<TPointer>::value_type) * static_cast<size_t>(se - sb));
  }

  //***************************************************************************
  /// Template wrapper for memcmp.
  /// \param sb Source begin
  /// \param n  Source length
  /// \param db Destination begin
  /// \return < 0	The first byte that does not match in both memory blocks has a lower value in 'sb' than in 'db' when evaluated as unsigned char values.
  ///   0 The contents of both memory blocks are equal
  /// > 0	The first byte that does not match in both memory blocks has a greater value in 'sb' than in 'db' when evaluated as unsigned char values.
  //***************************************************************************
  template <typename TPointer>
  TYPHOON_NODISCARD
    typename tpn::enable_if<tpn::is_trivially_copyable<typename tpn::iterator_traits<TPointer>::value_type>::value, int>::type
    mem_compare(const TPointer sb, size_t n, TPointer db) TYPHOON_NOEXCEPT
  {
    return memcmp(reinterpret_cast<void*>(db), 
                  reinterpret_cast<void*>(sb), 
                  sizeof(typename tpn::iterator_traits<TPointer>::value_type) * n);
  }

  //***************************************************************************
  /// Template wrapper for memset.
  /// \param db    Destination begin.
  /// \param de    Destination end.
  /// \param value The value to set.
  /// \return The destination
  //***************************************************************************
  template <typename TPointer, typename T>
  typename tpn::enable_if<tpn::is_trivially_copyable<typename tpn::iterator_traits<TPointer>::value_type>::value, TPointer>::type
    mem_set(TPointer db, const TPointer de, T value) TYPHOON_NOEXCEPT
  {
    return reinterpret_cast<TPointer>(memset(reinterpret_cast<void*>(db), 
                                             static_cast<char>(value),
                                             sizeof(typename tpn::iterator_traits<TPointer>::value_type) * static_cast<size_t>(de - db)));
  }

  //***************************************************************************
  /// Template wrapper for memset.
  /// \param db    Destination begin.
  /// \param n     Destination length.
  /// \param value The value to set.
  /// \return The destination
  //***************************************************************************
  template <typename TPointer, typename T>
  typename tpn::enable_if<tpn::is_trivially_copyable<typename tpn::iterator_traits<TPointer>::value_type>::value, TPointer>::type
    mem_set(const TPointer db, size_t n, T value) TYPHOON_NOEXCEPT
  {
    return reinterpret_cast<TPointer>(memset(reinterpret_cast<void*>(db),
                                             static_cast<char>(value),
                                             sizeof(typename tpn::iterator_traits<TPointer>::value_type) * n));
  }

  //***************************************************************************
  /// Template wrapper for memchr.
  /// \param sb    Source begin.
  /// \param se    Source end.
  /// \param value The value to find.
  /// \return The position of the char or 'se'.
  //***************************************************************************
  template <typename TPointer, typename T>
  TYPHOON_NODISCARD
    typename tpn::enable_if<tpn::is_pointer<TPointer>::value && !tpn::is_const<typename tpn::remove_pointer<TPointer>::type>::value, char*>::type
    mem_char(TPointer sb, TPointer se, T value) TYPHOON_NOEXCEPT
  {
    void* result = memchr(reinterpret_cast<void*>(sb), 
                          static_cast<char>(value),
                          sizeof(typename tpn::iterator_traits<TPointer>::value_type) * static_cast<size_t>(se - sb));

    return (result == 0U) ? reinterpret_cast<char*>(se) : reinterpret_cast<char*>(result);
  }

  //***************************************************************************
  /// Template wrapper for memchr.
  /// \param sb    Source begin.
  /// \param se    Source end.
  /// \param value The value to find.
  /// \return The position of the char or 'se'.
  //***************************************************************************
  template <typename TPointer, typename T>
  TYPHOON_NODISCARD
    typename tpn::enable_if<tpn::is_pointer<TPointer>::value && tpn::is_const<typename tpn::remove_pointer<TPointer>::type>::value, const char*>::type
    mem_char(TPointer sb, TPointer se, T value) TYPHOON_NOEXCEPT
  {
    const void* result = memchr(reinterpret_cast<const void*>(sb),
                                static_cast<char>(value),
                                sizeof(typename tpn::iterator_traits<TPointer>::value_type) * static_cast<size_t>(se - sb));

    return (result == 0U) ? reinterpret_cast<const char*>(se) : reinterpret_cast<const char*>(result);
  }

  //***************************************************************************
  /// Template wrapper for memchr.
  /// \param sb    Source begin.
  /// \param n     Source length.
  /// \param value The value to find.
  /// \return The position of the char or 'sb + n'
  //***************************************************************************
  template <typename TPointer, typename T>
  TYPHOON_NODISCARD
    typename tpn::enable_if<tpn::is_pointer<TPointer>::value && !tpn::is_const<typename tpn::remove_pointer<TPointer>::type>::value, char*>::type
    mem_char(TPointer sb, size_t n, T value) TYPHOON_NOEXCEPT
  {
    void* result = memchr(reinterpret_cast<void*>(sb), 
                          static_cast<char>(value),
                          sizeof(typename tpn::iterator_traits<TPointer>::value_type) * n);

    return (result == 0U) ? reinterpret_cast<char*>(sb + n) : reinterpret_cast<char*>(result);
  }

  //***************************************************************************
  /// Template wrapper for memchr.
  /// \param sb    Source begin.
  /// \param n     Source length.
  /// \param value The value to find.
  /// \return The position of the char or 'sb + n'
  //***************************************************************************
  template <typename TPointer, typename T>
  TYPHOON_NODISCARD
    typename tpn::enable_if<tpn::is_pointer<TPointer>::value && tpn::is_const<typename tpn::remove_pointer<TPointer>::type>::value, const char*>::type
    mem_char(TPointer sb, size_t n, T value) TYPHOON_NOEXCEPT
  {
    const void* result = memchr(reinterpret_cast<const void*>(sb),
                                static_cast<char>(value),
                                sizeof(typename tpn::iterator_traits<TPointer>::value_type) * n);

    return (result == 0U) ? reinterpret_cast<const char*>(sb + n) : reinterpret_cast<const char*>(result);
  }
}

#endif
