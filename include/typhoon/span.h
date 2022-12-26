///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tphn
https://www.tphncpp.com

Copyright(c) 2020 John Wellbelove

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

#ifndef TYPHOON_SPAN_INCLUDED
#define TYPHOON_SPAN_INCLUDED

#include "platform.h"
#include "iterator.h"
#include "circular_iterator.h"
#include "nullptr.h"
#include "hash.h"
#include "type_traits.h"
#include "integral_limits.h"
#include "memory.h"
#include "array.h"

#include "private/dynamic_extent.h"

#if TYPHOON_USING_STL && TYPHOON_USING_CPP11
  #include <array>
#endif

///\defgroup span span
///\ingroup containers

namespace tphn
{
  //***************************************************************************
  /// Span - Fixed Extent
  //***************************************************************************
  template <typename T, size_t Extent = tphn::dynamic_extent>
  class span
  {
  public:

    typedef T                                element_type;
    typedef typename tphn::remove_cv<T>::type value_type;
    typedef size_t                           size_type;
    typedef T&                               reference;
    typedef const T&                         const_reference;
    typedef T*                               pointer;
    typedef const T*                         const_pointer;

    typedef T*                                     iterator;
    typedef TYPHOON_OR_STD::reverse_iterator<iterator> reverse_iterator;

    typedef tphn::circular_iterator<pointer>                                circular_iterator;
    typedef tphn::circular_iterator<TYPHOON_OR_STD::reverse_iterator<pointer> > reverse_circular_iterator;

    static TYPHOON_CONSTANT size_t extent = Extent;

    //*************************************************************************
    /// Default constructor.
    //*************************************************************************
    TYPHOON_CONSTEXPR span() TYPHOON_NOEXCEPT
      : pbegin(TYPHOON_NULLPTR)
    {
    }

    //*************************************************************************
    /// Construct from iterators + size
    //*************************************************************************
    template <typename TIterator, typename TSize>
    TYPHOON_CONSTEXPR explicit span(const TIterator begin_, const TSize /*size_*/) TYPHOON_NOEXCEPT
      : pbegin(tphn::addressof(*begin_))
    {
    }

    //*************************************************************************
    /// Construct from iterators
    //*************************************************************************
    template <typename TIterator>
    TYPHOON_CONSTEXPR explicit span(const TIterator begin_, const TIterator /*end_*/)
      : pbegin(tphn::addressof(*begin_))
    {
    }

    //*************************************************************************
    /// Construct from C array
    //*************************************************************************
    template<size_t ARRAY_SIZE>
    TYPHOON_CONSTEXPR span(element_type(&begin_)[ARRAY_SIZE]) TYPHOON_NOEXCEPT
      : pbegin(begin_)
    {
    }

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Construct from tphn::array.
    //*************************************************************************
    template <typename U, size_t N, typename = typename tphn::enable_if<(N == Extent) && tphn::is_same<tphn::remove_cv_t<T>, tphn::remove_cv_t<U>>::value, void>::type>
    TYPHOON_CONSTEXPR span(tphn::array<U, N>& a) TYPHOON_NOEXCEPT
      : pbegin(a.data())
    {
    }

    //*************************************************************************
    /// Construct from tphn::array.
    //*************************************************************************
    template <typename U, size_t N, typename = typename tphn::enable_if<(N == Extent) && tphn::is_same<tphn::remove_cv_t<T>, tphn::remove_cv_t<U>>::value, void>::type>
    TYPHOON_CONSTEXPR span(const tphn::array<U, N>& a) TYPHOON_NOEXCEPT
      : pbegin(a.data())
    {
    }
#else
    //*************************************************************************
    /// Construct from tphn::array.
    //*************************************************************************
    template <typename U, size_t N>
    TYPHOON_CONSTEXPR span(tphn::array<U, N>& a, typename tphn::enable_if<(N == Extent) && tphn::is_same<typename tphn::remove_cv<T>::type, typename tphn::remove_cv<U>::type>::value, void>::type* = 0) TYPHOON_NOEXCEPT
      : pbegin(a.data())
    {
    }

    //*************************************************************************
    /// Construct from tphn::array.
    //*************************************************************************
    template <typename U, size_t N>
    TYPHOON_CONSTEXPR span(const tphn::array<U, N>& a, typename tphn::enable_if<(N == Extent) && tphn::is_same<typename tphn::remove_cv<T>::type, typename tphn::remove_cv<U>::type>::value, void>::type* = 0) TYPHOON_NOEXCEPT
      : pbegin(a.data())
    {
    }
#endif

#if TYPHOON_USING_STL && TYPHOON_USING_CPP11
    //*************************************************************************
    /// Construct from std::array.
    //*************************************************************************
    template <typename U, size_t N, typename = typename tphn::enable_if<(N == Extent) && tphn::is_same<tphn::remove_cv_t<T>, tphn::remove_cv_t<U>>::value, void>::type>
    TYPHOON_CONSTEXPR span(std::array<U, N>& a) TYPHOON_NOEXCEPT
      : pbegin(a.data())
    {
    }

    //*************************************************************************
    /// Construct from std::array.
    //*************************************************************************
    template <typename U, size_t N, typename = typename tphn::enable_if<(N == Extent) && tphn::is_same<tphn::remove_cv_t<T>, tphn::remove_cv_t<U>>::value, void>::type>
    TYPHOON_CONSTEXPR span(const std::array<U, N>& a) TYPHOON_NOEXCEPT
      : pbegin(a.data())
    {
    }
#endif

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Construct from a container or other type that supports
    /// data() and size() member functions.
    //*************************************************************************
    template <typename TContainer, typename = typename tphn::enable_if<!tphn::is_pointer<tphn::remove_reference_t<TContainer>>::value &&
                                                                      !tphn::is_array<tphn::remove_reference_t<TContainer>>::value&&
                                                                      tphn::is_same<tphn::remove_cv_t<T>, tphn::remove_cv_t<typename tphn::remove_reference_t<TContainer>::value_type>>::value, void>::type>
      TYPHOON_CONSTEXPR span(TContainer&& a) TYPHOON_NOEXCEPT
      : pbegin(a.data())
    {
    }
#else
    //*************************************************************************
    /// Construct from a container or other type that supports
    /// data() and size() member functions.
    //*************************************************************************
    template <typename TContainer>
    span(TContainer& a, typename tphn::enable_if<!tphn::is_pointer<typename tphn::remove_reference<TContainer>::type>::value &&
                                                !tphn::is_array<TContainer>::value &&
                                                tphn::is_same<typename tphn::remove_cv<T>::type, typename tphn::remove_cv<typename tphn::remove_reference<TContainer>::type::value_type>::type>::value, void>::type* = 0) TYPHOON_NOEXCEPT
      : pbegin(a.data())
    {
    }

    //*************************************************************************
    /// Construct from a container or other type that supports
    /// data() and size() member functions.
    //*************************************************************************
    template <typename TContainer>
    TYPHOON_CONSTEXPR span(const TContainer& a, typename tphn::enable_if<!tphn::is_pointer<typename tphn::remove_reference<TContainer>::type>::value &&
                                                                    !tphn::is_array<TContainer>::value&&
                                                                    tphn::is_same<typename tphn::remove_cv<T>::type, typename tphn::remove_cv<typename tphn::remove_reference<TContainer>::type::value_type>::type>::value, void>::type* = 0) TYPHOON_NOEXCEPT
      : pbegin(a.data())
    {
    }
#endif

    //*************************************************************************
    /// Copy constructor
    //*************************************************************************
    TYPHOON_CONSTEXPR span(const span& other) TYPHOON_NOEXCEPT
      : pbegin(other.pbegin)
    {
    }

    //*************************************************************************
    /// Copy constructor
    //*************************************************************************
    template <typename U, size_t N>
    TYPHOON_CONSTEXPR span(const tphn::span<U, N>& other, typename tphn::enable_if<(Extent == tphn::dynamic_extent) || (N == tphn::dynamic_extent) || (N == Extent), void>::type) TYPHOON_NOEXCEPT
      : pbegin(other.pbegin)
    {
    }

    //*************************************************************************
    /// Returns a reference to the first element.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR reference front() const TYPHOON_NOEXCEPT
    {
      return *pbegin;
    }

    //*************************************************************************
    /// Returns a reference to the last element.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR reference back() const TYPHOON_NOEXCEPT
    {
      return *((pbegin + Extent) - 1);
    }

    //*************************************************************************
    /// Returns a pointer to the first element of the internal storage.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR pointer data() const TYPHOON_NOEXCEPT
    {
      return pbegin;
    }

    //*************************************************************************
    /// Returns an iterator to the beginning of the span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR iterator begin() const TYPHOON_NOEXCEPT
    {
      return pbegin;
    }

    //*************************************************************************
    /// Returns a circular iterator to the beginning of the span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR circular_iterator begin_circular() const TYPHOON_NOEXCEPT
    {
      return circular_iterator(begin(), end());
    }

    //*************************************************************************
    /// Returns an iterator to the end of the span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR iterator end() const TYPHOON_NOEXCEPT
    {
      return (pbegin + Extent);
    }

    //*************************************************************************
    // Returns an reverse iterator to the reverse beginning of the span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR reverse_iterator rbegin() const TYPHOON_NOEXCEPT
    {
      return reverse_iterator((pbegin + Extent));
    }

    //*************************************************************************
    /// Returns a reverse circular iterator to the end of the span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR reverse_circular_iterator rbegin_circular() const TYPHOON_NOEXCEPT
    {
      return reverse_circular_iterator(rbegin(), rend());
    }

    //*************************************************************************
    /// Returns a reverse iterator to the end of the span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR reverse_iterator rend() const TYPHOON_NOEXCEPT
    {
      return reverse_iterator(pbegin);
    }

    //*************************************************************************
    /// Returns <b>true</b> if the span size is zero.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR bool empty() const TYPHOON_NOEXCEPT
    {
      return false;
    }

    //*************************************************************************
    /// Returns the size of the span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR size_t size() const TYPHOON_NOEXCEPT
    {
      return Extent;
    }

    //*************************************************************************
    /// Returns the size of the span in bytes.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR size_t size_bytes() const TYPHOON_NOEXCEPT
    {
      return sizeof(element_type) * Extent;
    }

    //*************************************************************************
    /// Returns the maximum possible size of the span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR size_t max_size() const TYPHOON_NOEXCEPT
    {
      return size();
    }

    //*************************************************************************
    /// Assign from a span.
    //*************************************************************************
    TYPHOON_CONSTEXPR14 span& operator =(const span& other) TYPHOON_NOEXCEPT
    {
      pbegin = other.pbegin;
      return *this;
    }

    //*************************************************************************
    /// Returns a reference to the indexed value.
    //*************************************************************************
    TYPHOON_CONSTEXPR reference operator[](const size_t i) const
    {
      return pbegin[i];
    }

    //*************************************************************************
    /// Obtains a span that is a view over the first COUNT elements of this span.
    //*************************************************************************
    template <size_t COUNT>
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR tphn::span<element_type, COUNT> first() const TYPHOON_NOEXCEPT
    {
      return tphn::span<element_type, COUNT>(pbegin, pbegin + COUNT);
    }

    //*************************************************************************
    /// Obtains a span that is a view over the first count elements of this span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR tphn::span<element_type, tphn::dynamic_extent> first(size_t count) const TYPHOON_NOEXCEPT
    {
      return tphn::span<element_type, tphn::dynamic_extent>(pbegin, pbegin + count);
    }

    //*************************************************************************
    /// Obtains a span that is a view over the last COUNT elements of this span.
    //*************************************************************************
    template <size_t COUNT>
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR tphn::span<element_type, COUNT> last() const TYPHOON_NOEXCEPT
    {
      return tphn::span<element_type, COUNT>(pbegin + Extent - COUNT, (pbegin + Extent));
    }

    //*************************************************************************
    /// Obtains a span that is a view over the last count elements of this span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR tphn::span<element_type, tphn::dynamic_extent> last(size_t count) const TYPHOON_NOEXCEPT
    {
      return tphn::span<element_type, tphn::dynamic_extent>((pbegin + Extent) - count, (pbegin + Extent));
    }

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Obtains a span that is a view from OFFSET over the next COUNT elements of this span.
    //*************************************************************************
    template <const size_t OFFSET, size_t COUNT = tphn::dynamic_extent>
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR
    tphn::span<element_type, COUNT != tphn::dynamic_extent ? COUNT : Extent - OFFSET> subspan() const TYPHOON_NOEXCEPT
    {
      return (COUNT == tphn::dynamic_extent) ? tphn::span<element_type, COUNT != tphn::dynamic_extent ? COUNT : Extent - OFFSET>(pbegin + OFFSET, (pbegin + Extent))
                                            : tphn::span<element_type, COUNT != tphn::dynamic_extent ? COUNT : Extent - OFFSET>(pbegin + OFFSET, pbegin + OFFSET + COUNT);
    }
#else
    //*************************************************************************
    /// Obtains a span that is a view from OFFSET over the next COUNT elements of this span.
    //*************************************************************************
    template <const size_t OFFSET, size_t COUNT>
    tphn::span<element_type, COUNT != tphn::dynamic_extent ? COUNT : Extent - OFFSET> subspan() const
    {
      if (COUNT == tphn::dynamic_extent)
      {
        return tphn::span<element_type, (COUNT != tphn::dynamic_extent ? COUNT : Extent - OFFSET)>(pbegin + OFFSET, (pbegin + Extent));
      }
      else
      {
        return tphn::span<element_type, COUNT != tphn::dynamic_extent ? COUNT : Extent - OFFSET>(pbegin + OFFSET, pbegin + OFFSET + COUNT);
      }
    }
#endif

    //*************************************************************************
    /// Obtains a span that is a view from 'offset' over the next 'count' elements of this span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR tphn::span<element_type, tphn::dynamic_extent> subspan(size_t offset, size_t count = tphn::dynamic_extent) const TYPHOON_NOEXCEPT
    {
      return (count == tphn::dynamic_extent) ? tphn::span<element_type, tphn::dynamic_extent>(pbegin + offset, (pbegin + Extent))
                                            : tphn::span<element_type, tphn::dynamic_extent>(pbegin + offset, pbegin + offset + count);
    }

  private:

    pointer pbegin;
  };

  //***************************************************************************
  /// Span - Dynamic Extent
  //***************************************************************************
  template <typename T>
  class span<T, tphn::dynamic_extent>
  {
  public:

    typedef T        element_type;
    typedef typename tphn::remove_cv<T>::type value_type;
    typedef size_t   size_type;
    typedef T&       reference;
    typedef const T& const_reference;
    typedef T*       pointer;
    typedef const T* const_pointer;
    
    typedef T*                                     iterator;
    typedef TYPHOON_OR_STD::reverse_iterator<iterator> reverse_iterator;

    typedef tphn::circular_iterator<pointer>                                circular_iterator;
    typedef tphn::circular_iterator<TYPHOON_OR_STD::reverse_iterator<pointer> > reverse_circular_iterator;

    static TYPHOON_CONSTANT size_t extent = tphn::dynamic_extent;

    //*************************************************************************
    /// Default constructor.
    //*************************************************************************
    TYPHOON_CONSTEXPR span() TYPHOON_NOEXCEPT
      : pbegin(TYPHOON_NULLPTR)
      , pend(TYPHOON_NULLPTR)
    {
    }

    //*************************************************************************
    /// Construct from pointer + size
    //*************************************************************************
    template <typename TIterator, typename TSize>
    TYPHOON_CONSTEXPR span(const TIterator begin_, const TSize size_) TYPHOON_NOEXCEPT
      : pbegin(tphn::addressof(*begin_))
      , pend(tphn::addressof(*begin_) + size_)
    {
    }

    //*************************************************************************
    /// Construct from iterators
    //*************************************************************************
    template <typename TIterator>
    TYPHOON_CONSTEXPR span(const TIterator begin_, const TIterator end_)
      : pbegin(tphn::addressof(*begin_))
      , pend(tphn::addressof(*begin_) + tphn::distance(begin_, end_))
    {
    }

    //*************************************************************************
    /// Construct from C array
    //*************************************************************************
    template<size_t ARRAY_SIZE>
    TYPHOON_CONSTEXPR span(element_type(&begin_)[ARRAY_SIZE]) TYPHOON_NOEXCEPT
      : pbegin(begin_)
      , pend(begin_ + ARRAY_SIZE)
    {
    }

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Construct from tphn::array.
    //*************************************************************************
    template <typename U, size_t N, typename = typename tphn::enable_if<tphn::is_same<tphn::remove_cv_t<T>, tphn::remove_cv_t<U>>::value, void>::type>
    TYPHOON_CONSTEXPR span(tphn::array<U, N>& a) TYPHOON_NOEXCEPT
      : pbegin(a.data())
      , pend(a.data() + a.size())
    {
    }

    //*************************************************************************
    /// Construct from tphn::array.
    //*************************************************************************
    template <typename U, size_t N, typename = typename tphn::enable_if<tphn::is_same<tphn::remove_cv_t<T>, tphn::remove_cv_t<U>>::value, void>::type>
    TYPHOON_CONSTEXPR span(const tphn::array<U, N>& a) TYPHOON_NOEXCEPT
      : pbegin(a.data())
      , pend(a.data() + a.size())
    {
    }
#else
    //*************************************************************************
    /// Construct from tphn::array.
    //*************************************************************************
    template <typename U, size_t N>
    TYPHOON_CONSTEXPR span(tphn::array<U, N>& a, typename tphn::enable_if<tphn::is_same<typename tphn::remove_cv<T>::type, typename tphn::remove_cv<U>::type>::value, void>::type* = 0) TYPHOON_NOEXCEPT
      : pbegin(a.data())
      , pend(a.data() + a.size())
    {
    }

    //*************************************************************************
    /// Construct from tphn::array.
    //*************************************************************************
    template <typename U, size_t N>
    TYPHOON_CONSTEXPR span(const tphn::array<U, N>& a, typename tphn::enable_if<tphn::is_same<typename tphn::remove_cv<T>::type, typename tphn::remove_cv<U>::type>::value, void>::type* = 0) TYPHOON_NOEXCEPT
      : pbegin(a.data())
      , pend(a.data() + a.size())
    {
    }
#endif

#if TYPHOON_USING_STL && TYPHOON_USING_CPP11
    //*************************************************************************
    /// Construct from std::array.
    //*************************************************************************
    template <typename U, size_t N, typename = typename tphn::enable_if<tphn::is_same<tphn::remove_cv_t<T>, tphn::remove_cv_t<U>>::value, void>::type>
    TYPHOON_CONSTEXPR span(std::array<U, N>& a) TYPHOON_NOEXCEPT
      : pbegin(a.data())
      , pend(a.data() + a.size())
    {
    }

    //*************************************************************************
    /// Construct from std::array.
    //*************************************************************************
    template <typename U, size_t N, typename = typename tphn::enable_if<tphn::is_same<tphn::remove_cv_t<T>, tphn::remove_cv_t<U>>::value, void>::type>
    TYPHOON_CONSTEXPR span(const std::array<U, N>& a) TYPHOON_NOEXCEPT
      : pbegin(a.data())
      , pend(a.data() + a.size())
    {
    }
#endif

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Construct from a container or other type that supports
    /// data() and size() member functions.
    //*************************************************************************
    template <typename TContainer, typename = typename tphn::enable_if<!tphn::is_pointer<tphn::remove_reference_t<TContainer>>::value &&
                                                                      !tphn::is_array<tphn::remove_reference_t<TContainer>>::value &&
                                                                      tphn::is_same<tphn::remove_cv_t<T>, tphn::remove_cv_t<typename tphn::remove_reference_t<TContainer>::value_type>>::value, void>::type>
      TYPHOON_CONSTEXPR span(TContainer&& a) TYPHOON_NOEXCEPT
      : pbegin(a.data())
      , pend(a.data() + a.size())
    {
    }
#else
    //*************************************************************************
    /// Construct from a container or other type that supports
    /// data() and size() member functions.
    //*************************************************************************
    template <typename TContainer>
    TYPHOON_CONSTEXPR span(TContainer& a, typename tphn::enable_if<!tphn::is_pointer<typename tphn::remove_reference<TContainer>::type>::value &&
                                                              !tphn::is_array<TContainer>::value &&
                                                              tphn::is_same<typename tphn::remove_cv<T>::type, typename tphn::remove_cv<typename tphn::remove_reference<TContainer>::type::value_type>::type>::value, void>::type* = 0) TYPHOON_NOEXCEPT
      : pbegin(a.data())
      , pend(a.data() + a.size())
    {
    }

    //*************************************************************************
    /// Construct from a container or other type that supports
    /// data() and size() member functions.
    //*************************************************************************
    template <typename TContainer>
    TYPHOON_CONSTEXPR span(const TContainer& a, typename tphn::enable_if<!tphn::is_pointer<typename tphn::remove_reference<TContainer>::type>::value &&
                                                                    !tphn::is_array<TContainer>::value &&
                                                                    tphn::is_same<typename tphn::remove_cv<T>::type, typename tphn::remove_cv<typename tphn::remove_reference<TContainer>::type::value_type>::type>::value, void>::type* = 0) TYPHOON_NOEXCEPT
      : pbegin(a.data())
      , pend(a.data() + a.size())
    {
    }
#endif

    //*************************************************************************
    /// Copy constructor
    //*************************************************************************
    TYPHOON_CONSTEXPR span(const span& other) TYPHOON_NOEXCEPT
      : pbegin(other.pbegin)
      , pend(other.pend)
    {
    }

    //*************************************************************************
    /// Copy constructor
    //*************************************************************************
    template <typename U, size_t N>
    TYPHOON_CONSTEXPR span(const tphn::span<U, N>& other) TYPHOON_NOEXCEPT
      : pbegin(other.begin())
      , pend(other.begin() + N)
    {
    }

    //*************************************************************************
    /// Returns a reference to the first element.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR reference front() const TYPHOON_NOEXCEPT
    {
      return *pbegin;
    }

    //*************************************************************************
    /// Returns a reference to the last element.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR reference back() const TYPHOON_NOEXCEPT
    {
      return *(pend - 1);
    }

    //*************************************************************************
    /// Returns a pointer to the first element of the internal storage.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR pointer data() const TYPHOON_NOEXCEPT
    {
      return pbegin;
    }

    //*************************************************************************
    /// Returns an iterator to the beginning of the span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR iterator begin() const TYPHOON_NOEXCEPT
    {
      return pbegin;
    }

    //*************************************************************************
    /// Returns a circular iterator to the beginning of the span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR circular_iterator begin_circular() const TYPHOON_NOEXCEPT
    {
      return circular_iterator(begin(), end());
    }

    //*************************************************************************
    /// Returns an iterator to the end of the span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR iterator end() const TYPHOON_NOEXCEPT
    {
      return pend;
    }

    //*************************************************************************
    // Returns an reverse iterator to the reverse beginning of the span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR reverse_iterator rbegin() const TYPHOON_NOEXCEPT
    {
      return reverse_iterator(pend);
    }

    //*************************************************************************
    /// Returns a reverse circular iterator to the end of the span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR reverse_circular_iterator rbegin_circular() const TYPHOON_NOEXCEPT
    {
      return reverse_circular_iterator(rbegin(), rend());
    }

    //*************************************************************************
    /// Returns a reverse iterator to the end of the span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR reverse_iterator rend() const TYPHOON_NOEXCEPT
    {
      return reverse_iterator(pbegin);
    }

    //*************************************************************************
    /// Returns <b>true</b> if the span size is zero.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR bool empty() const TYPHOON_NOEXCEPT
    {
      return (pbegin == pend);
    }

    //*************************************************************************
    /// Returns the size of the span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR size_t size() const TYPHOON_NOEXCEPT
    {
      return (pend - pbegin);
    }

    //*************************************************************************
    /// Returns the size of the span in bytes.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR size_t size_bytes() const TYPHOON_NOEXCEPT
    {
      return sizeof(element_type) * (pend - pbegin);
    }

    //*************************************************************************
    /// Returns the maximum possible size of the span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR size_t max_size() const TYPHOON_NOEXCEPT
    {
      return size();
    }

    //*************************************************************************
    /// Assign from a span.
    //*************************************************************************
    TYPHOON_CONSTEXPR14 span& operator =(const span& other) TYPHOON_NOEXCEPT
    {
      pbegin = other.pbegin;
      pend = other.pend;
      return *this;
    }

    //*************************************************************************
    /// Returns a reference to the indexed value.
    //*************************************************************************
    TYPHOON_CONSTEXPR reference operator[](const size_t i) const
    {
      return pbegin[i];
    }

    //*************************************************************************
    /// Obtains a span that is a view over the first COUNT elements of this span.
    //*************************************************************************
    template <size_t COUNT>
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR tphn::span<element_type, COUNT> first() const TYPHOON_NOEXCEPT
    {
      return tphn::span<element_type, COUNT>(pbegin, pbegin + COUNT);
    }

    //*************************************************************************
    /// Obtains a span that is a view over the first count elements of this span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR tphn::span<element_type, tphn::dynamic_extent> first(size_t count) const TYPHOON_NOEXCEPT
    {
      return tphn::span<element_type, tphn::dynamic_extent>(pbegin, pbegin + count);
    }

    //*************************************************************************
    /// Obtains a span that is a view over the last COUNT elements of this span.
    //*************************************************************************
    template <size_t COUNT>
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR tphn::span<element_type, COUNT> last() const TYPHOON_NOEXCEPT
    {
      return tphn::span<element_type, COUNT>(pend - COUNT, pend);
    }

    //*************************************************************************
    /// Obtains a span that is a view over the last count elements of this span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR tphn::span<element_type, tphn::dynamic_extent> last(size_t count) const TYPHOON_NOEXCEPT
    {
      return tphn::span<element_type, tphn::dynamic_extent>(pend - count, pend);
    }

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Obtains a span that is a view from OFFSET over the next COUNT elements of this span.
    //*************************************************************************
    template <const size_t OFFSET, size_t COUNT = tphn::dynamic_extent>
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR
    tphn::span<element_type, COUNT != tphn::dynamic_extent ? COUNT : tphn::dynamic_extent> subspan() const TYPHOON_NOEXCEPT
    {
      return (COUNT == tphn::dynamic_extent) ? tphn::span<element_type, COUNT != tphn::dynamic_extent ? COUNT : tphn::dynamic_extent>(pbegin + OFFSET, pend)
                                            : tphn::span<element_type, COUNT != tphn::dynamic_extent ? COUNT : tphn::dynamic_extent>(pbegin + OFFSET, pbegin + OFFSET + COUNT);
    }
#else
    //*************************************************************************
    /// Obtains a span that is a view from OFFSET over the next COUNT elements of this span.
    //*************************************************************************
    template <const size_t OFFSET, size_t COUNT>
    tphn::span<element_type, COUNT != tphn::dynamic_extent ? COUNT : tphn::dynamic_extent> subspan() const
    {
      if (COUNT == tphn::dynamic_extent)
      {
        return tphn::span<element_type, COUNT != tphn::dynamic_extent ? COUNT : tphn::dynamic_extent>(pbegin + OFFSET, pend);
      }
      else
      {
        return tphn::span<element_type, COUNT != tphn::dynamic_extent ? COUNT : tphn::dynamic_extent>(pbegin + OFFSET, pbegin + OFFSET + COUNT);
      }
    }
#endif

    //*************************************************************************
    /// Obtains a span that is a view from 'offset' over the next 'count' elements of this span.
    //*************************************************************************
    TYPHOON_NODISCARD TYPHOON_CONSTEXPR14 tphn::span<element_type, tphn::dynamic_extent> subspan(size_t offset, size_t count = tphn::dynamic_extent) const TYPHOON_NOEXCEPT
    {
      return (count == tphn::dynamic_extent) ? tphn::span<element_type, tphn::dynamic_extent>(pbegin + offset, pend)
                                            : tphn::span<element_type, tphn::dynamic_extent>(pbegin + offset, pbegin + offset + count);
    }

  private:

    pointer pbegin;
    pointer pend;
  };

  //*************************************************************************
  /// Template deduction guides.
  //*************************************************************************
#if TYPHOON_USING_CPP17
  template <typename TIterator>
  span(const TIterator begin_, const TIterator end_)
    ->span<tphn::remove_pointer_t<TIterator>, tphn::dynamic_extent>;

  template <typename TIterator, typename TSize>
  span(const TIterator begin_, const TSize size_)
    ->span<tphn::remove_pointer_t<TIterator>, tphn::dynamic_extent>;

  template <typename T, size_t N>
  span(T(&)[N])
    -> span<T, N>;

  template <typename T, size_t N>
  span(tphn::array<T, N>&)
    -> span<T, N>;

  template <typename T, size_t N>
  span(const tphn::array<T, N>&)
    -> span<const T, N>;

#if TYPHOON_USING_STL
  template <typename T, size_t N>
  span(std::array<T, N>&)
    ->span<T, N>;

  template <typename T, size_t N>
  span(const std::array<T, N>&)
    ->span<const T, N>;
#endif
#endif 

  //*************************************************************************
  /// Hash function.
  //*************************************************************************
#if TYPHOON_USING_8BIT_TYPES
  template <typename T, size_t Extent>
  struct hash<tphn::span<T, Extent> >
  {
    size_t operator()(const tphn::span<T>& view) const
    {
      return tphn::private_hash::generic_hash<size_t>(reinterpret_cast<const uint8_t*>(&view[0]),
        reinterpret_cast<const uint8_t*>(&view[view.size()]));
    }
  };
#endif
}

#endif
