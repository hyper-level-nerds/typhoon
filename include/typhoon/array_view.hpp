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

#ifndef TYPHOON_ARRAY_VIEW_HPP
#define TYPHOON_ARRAY_VIEW_HPP

#include "platform.hpp"
#include "memory.hpp"
#include "array.hpp"
#include "iterator.hpp"
#include "error_handler.hpp"
#include "exception.hpp"
#include "nullptr.hpp"
#include "hash.hpp"
#include "algorithm.hpp"
#include "type_traits.hpp"

#if TYPHOON_USING_STL && TYPHOON_USING_CPP11
#include <array>
#endif

///\defgroup array array
/// A wrapper for arrays
///\ingroup containers

namespace tpn
{
  //***************************************************************************
  /// The base class for array_view exceptions.
  //***************************************************************************
  class array_view_exception : public exception
  {
  public:

    array_view_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  ///\ingroup stack
  /// The exception thrown when the index is out of bounds.
  //***************************************************************************
  class array_view_bounds : public array_view_exception
  {
  public:

    array_view_bounds(string_type file_name_, numeric_type line_number_)
      : array_view_exception(TYPHOON_ERROR_TEXT("array_view:bounds", TYPHOON_ARRAY_VIEW_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  ///\ingroup stack
  /// The exception thrown when the view is uninitialised.
  //***************************************************************************
  class array_view_uninitialised : public array_view_exception
  {
  public:

    array_view_uninitialised(string_type file_name_, numeric_type line_number_)
      : array_view_exception(TYPHOON_ERROR_TEXT("array_view:uninitialised", TYPHOON_ARRAY_VIEW_FILE_ID"B"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Array view.
  //***************************************************************************
  template <typename T>
  class array_view
  {
  public:

    typedef T        value_type;
    typedef size_t   size_type;
    typedef const T& const_reference;
    typedef const T* const_pointer;
    typedef const T* const_iterator;
    typedef TYPHOON_OR_STD::reverse_iterator<const_iterator> const_reverse_iterator;

#if defined(TYPHOON_ARRAY_VIEW_IS_MUTABLE)
    typedef T* pointer;
    typedef T& reference;
    typedef T* iterator;
    typedef TYPHOON_OR_STD::reverse_iterator<iterator> reverse_iterator;
#else
    typedef const_pointer   pointer;
    typedef const_reference reference;
    typedef const_pointer   iterator;
    typedef const_reverse_iterator reverse_iterator;
#endif

    //*************************************************************************
    /// Default constructor.
    //*************************************************************************
    TYPHOON_CONSTEXPR array_view()
      : mbegin(TYPHOON_NULLPTR),
        mend(TYPHOON_NULLPTR)
    {
    }

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Construct from tpn::array.
    //*************************************************************************
    template <typename U, size_t N, typename = typename tpn::enable_if<tpn::is_same<tpn::remove_cv_t<T>, tpn::remove_cv_t<U>>::value, void>::type>
    TYPHOON_CONSTEXPR array_view(tpn::array<U, N>& a) TYPHOON_NOEXCEPT
      : mbegin(a.data())
      , mend(a.data() + a.size())
    {
    }

    //*************************************************************************
    /// Construct from tpn::array.
    //*************************************************************************
    template <typename U, size_t N, typename = typename tpn::enable_if<tpn::is_same<tpn::remove_cv_t<T>, tpn::remove_cv_t<U>>::value, void>::type>
    TYPHOON_CONSTEXPR array_view(const tpn::array<U, N>& a) TYPHOON_NOEXCEPT
      : mbegin(a.data())
      , mend(a.data() + a.size())
    {
    }
#else
    //*************************************************************************
    /// Construct from tpn::array.
    //*************************************************************************
    template <typename U, size_t N>
    TYPHOON_CONSTEXPR array_view(tpn::array<U, N>& a, typename tpn::enable_if<tpn::is_same<typename tpn::remove_cv<T>::type, typename tpn::remove_cv<U>::type>::value, void>::type* = 0) TYPHOON_NOEXCEPT
      : mbegin(a.data())
      , mend(a.data() + a.size())
    {
    }

    //*************************************************************************
    /// Construct from tpn::array.
    //*************************************************************************
    template <typename U, size_t N>
    TYPHOON_CONSTEXPR array_view(const tpn::array<U, N>& a, typename tpn::enable_if<tpn::is_same<typename tpn::remove_cv<T>::type, typename tpn::remove_cv<U>::type>::value, void>::type* = 0) TYPHOON_NOEXCEPT
      : mbegin(a.data())
      , mend(a.data() + a.size())
    {
    }
#endif

#if TYPHOON_USING_STL && TYPHOON_USING_CPP11
    //*************************************************************************
    /// Construct from std::array.
    //*************************************************************************
    template <typename U, size_t N, typename = typename tpn::enable_if<tpn::is_same<tpn::remove_cv_t<T>, tpn::remove_cv_t<U>>::value, void>::type>
    TYPHOON_CONSTEXPR array_view(std::array<U, N>& a) TYPHOON_NOEXCEPT
      : mbegin(a.data())
      , mend(a.data() + a.size())
    {
    }

    //*************************************************************************
    /// Construct from std::array.
    //*************************************************************************
    template <typename U, size_t N, typename = typename tpn::enable_if<tpn::is_same<tpn::remove_cv_t<T>, tpn::remove_cv_t<U>>::value, void>::type>
    TYPHOON_CONSTEXPR array_view(const std::array<U, N>& a) TYPHOON_NOEXCEPT
      : mbegin(a.data())
      , mend(a.data() + a.size())
    {
    }
#endif

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Construct from a container or other type that supports
    /// data() and size() member functions.
    //*************************************************************************
    template <typename TContainer, typename = typename tpn::enable_if<!tpn::is_pointer<tpn::remove_reference_t<TContainer>>::value &&
                                                                      !tpn::is_array<tpn::remove_reference_t<TContainer>>::value &&
                                                                      tpn::is_same<tpn::remove_cv_t<T>, tpn::remove_cv_t<typename tpn::remove_reference_t<TContainer>::value_type>>::value, void>::type>
      TYPHOON_CONSTEXPR array_view(TContainer&& a) TYPHOON_NOEXCEPT
      : mbegin(a.data())
      , mend(a.data() + a.size())
    {
    }
#else
    //*************************************************************************
    /// Construct from a container or other type that supports
    /// data() and size() member functions.
    //*************************************************************************
    template <typename TContainer>
    TYPHOON_CONSTEXPR array_view(TContainer& a, typename tpn::enable_if<!tpn::is_pointer<typename tpn::remove_reference<TContainer>::type>::value &&
                                                                    !tpn::is_array<TContainer>::value &&
                                                                    tpn::is_same<typename tpn::remove_cv<T>::type, typename tpn::remove_cv<typename tpn::remove_reference<TContainer>::type::value_type>::type>::value, void>::type* = 0) TYPHOON_NOEXCEPT
      : mbegin(a.data())
      , mend(a.data() + a.size())
    {
    }

    //*************************************************************************
    /// Construct from a container or other type that supports
    /// data() and size() member functions.
    //*************************************************************************
    template <typename TContainer>
    TYPHOON_CONSTEXPR array_view(const TContainer& a, typename tpn::enable_if<!tpn::is_pointer<typename tpn::remove_reference<TContainer>::type>::value &&
                                                                          !tpn::is_array<TContainer>::value &&
                                                                          tpn::is_same<typename tpn::remove_cv<T>::type, typename tpn::remove_cv<typename tpn::remove_reference<TContainer>::type::value_type>::type>::value, void>::type* = 0) TYPHOON_NOEXCEPT
      : mbegin(a.data())
      , mend(a.data() + a.size())
    {
    }
#endif

    //*************************************************************************
    /// Construct from iterators
    //*************************************************************************
    template <typename TIterator>
    TYPHOON_CONSTEXPR array_view(const TIterator begin_, const TIterator end_)
      : mbegin(tpn::addressof(*begin_)),
        mend(tpn::addressof(*begin_) + tpn::distance(begin_, end_))
    {
    }

    //*************************************************************************
    /// Construct from C array
    //*************************************************************************
    template <typename TIterator,
              typename TSize>
    TYPHOON_CONSTEXPR array_view(const TIterator begin_, const TSize size_)
      : mbegin(tpn::addressof(*begin_)),
        mend(tpn::addressof(*begin_) + size_)
    {
    }

    //*************************************************************************
    /// Construct from C array
    //*************************************************************************
    template<size_t ARRAY_SIZE>
    TYPHOON_CONSTEXPR array_view(T(&begin_)[ARRAY_SIZE])
      : mbegin(begin_),
        mend(begin_ + ARRAY_SIZE)
    {
    }

    //*************************************************************************
    /// Copy constructor
    //*************************************************************************
    TYPHOON_CONSTEXPR array_view(const array_view& other)
      : mbegin(other.mbegin),
        mend(other.mend)
    {
    }

    //*************************************************************************
    /// Returns a reference to the first element.
    //*************************************************************************
    reference front()
    {
      return *mbegin;
    }

    //*************************************************************************
    /// Returns a const reference to the first element.
    //*************************************************************************
    const_reference front() const
    {
      return *mbegin;
    }

    //*************************************************************************
    /// Returns a reference to the last element.
    //*************************************************************************
    reference back()
    {
      return *(mend - 1);
    }

    //*************************************************************************
    /// Returns a const reference to the last element.
    //*************************************************************************
    const_reference back() const
    {
      return *(mend - 1);
    }

    //*************************************************************************
    /// Returns a pointer to the first element of the internal storage.
    //*************************************************************************
    pointer data()
    {
      return mbegin;
    }

    //*************************************************************************
    /// Returns a const pointer to the first element of the internal storage.
    //*************************************************************************
    const_pointer data() const
    {
      return mbegin;
    }

    //*************************************************************************
    /// Returns an iterator to the beginning of the array.
    //*************************************************************************
    iterator begin()
    {
      return mbegin;
    }

    //*************************************************************************
    /// Returns a const iterator to the beginning of the array.
    //*************************************************************************
    const_iterator begin() const
    {
      return mbegin;
    }

    //*************************************************************************
    /// Returns a const iterator to the beginning of the array.
    //*************************************************************************
    const_iterator cbegin() const
    {
      return mbegin;
    }

    //*************************************************************************
    /// Returns an iterator to the end of the array.
    //*************************************************************************
    iterator end()
    {
      return mend;
    }

    //*************************************************************************
    /// Returns a const iterator to the end of the array.
    //*************************************************************************
    const_iterator end() const
    {
      return mend;
    }

    //*************************************************************************
    // Returns a const iterator to the end of the array.
    //*************************************************************************
    const_iterator cend() const
    {
      return mend;
    }

    //*************************************************************************
    // Returns an reverse iterator to the reverse beginning of the array.
    //*************************************************************************
    reverse_iterator rbegin()
    {
      return reverse_iterator(mend);
    }

    //*************************************************************************
    /// Returns a const reverse iterator to the reverse beginning of the array.
    //*************************************************************************
    const_reverse_iterator rbegin() const
    {
      return const_reverse_iterator(mend);
    }

    //*************************************************************************
    /// Returns a const reverse iterator to the reverse beginning of the array.
    //*************************************************************************
    const_reverse_iterator crbegin() const
    {
      return const_reverse_iterator(mend);
    }

    //*************************************************************************
    /// Returns a reverse iterator to the end of the array.
    //*************************************************************************
    reverse_iterator rend()
    {
      return reverse_iterator(mbegin);
    }

    //*************************************************************************
    /// Returns a const reverse iterator to the end of the array.
    //*************************************************************************
    const_reverse_iterator rend() const
    {
      return const_reverse_iterator(mbegin);
    }

    //*************************************************************************
    /// Returns a const reverse iterator to the end of the array.
    //*************************************************************************
    const_reverse_iterator crend() const
    {
      return const_reverse_iterator(mbegin);
    }

    //*************************************************************************
    /// Returns <b>true</b> if the array size is zero.
    //*************************************************************************
    bool empty() const
    {
      return (mbegin == mend);
    }

    //*************************************************************************
    /// Returns the size of the array.
    //*************************************************************************
    size_t size() const
    {
      return (mend - mbegin);
    }

    //*************************************************************************
    /// Returns the maximum possible size of the array.
    //*************************************************************************
    size_t max_size() const
    {
      return size();
    }

    //*************************************************************************
    /// Assign from a view.
    //*************************************************************************
    array_view& operator=(const array_view& other)
    {
      mbegin = other.mbegin;
      mend   = other.mend;
      return *this;
    }

    //*************************************************************************
    /// Assign from iterators
    //*************************************************************************
    template <typename TIterator>
    void assign(const TIterator begin_, const TIterator end_)
    {
      mbegin = tpn::addressof(*begin_);
      mend   = tpn::addressof(*begin_) + tpn::distance(begin_, end_);
    }

    //*************************************************************************
    /// Assign from iterator and size.
    //*************************************************************************
    template <typename TIterator,
              typename TSize>
    void assign(const TIterator begin_, const TSize size_)
    {
      mbegin = tpn::addressof(*begin_);
      mend   = tpn::addressof(*begin_) + size_;
    }

#if defined(TYPHOON_ARRAY_VIEW_IS_MUTABLE)
    //*************************************************************************
    /// Returns a reference to the indexed value.
    //*************************************************************************
    reference operator[](const size_t i)
    {
      return mbegin[i];
    }
#endif

    //*************************************************************************
    /// Returns a const reference to the indexed value.
    //*************************************************************************
    const_reference operator[](const size_t i) const
    {
      return mbegin[i];
    }

#if defined(TYPHOON_ARRAY_VIEW_IS_MUTABLE)
    //*************************************************************************
    /// Returns a reference to the indexed value.
    //*************************************************************************
    reference at(const size_t i)
    {
      TYPHOON_ASSERT((mbegin != TYPHOON_NULLPTR && mend != TYPHOON_NULLPTR), TYPHOON_ERROR(array_view_uninitialised));
      TYPHOON_ASSERT(i < size(), TYPHOON_ERROR(array_view_bounds));
      return mbegin[i];
    }
#endif

    //*************************************************************************
    /// Returns a const reference to the indexed value.
    //*************************************************************************
    const_reference at(const size_t i) const
    {
      TYPHOON_ASSERT((mbegin != TYPHOON_NULLPTR && mend != TYPHOON_NULLPTR), TYPHOON_ERROR(array_view_uninitialised));
      TYPHOON_ASSERT(i < size(), TYPHOON_ERROR(array_view_bounds));
      return mbegin[i];
    }

    //*************************************************************************
    /// Swaps with another array_view.
    //*************************************************************************
    void swap(array_view& other)
    {
      using TYPHOON_OR_STD::swap; // Allow ADL

      swap(mbegin, other.mbegin);
      swap(mend, other.mend);
    }

    //*************************************************************************
    /// Shrinks the view by moving its start forward.
    //*************************************************************************
    void remove_prefix(const size_type n)
    {
		if (n < size())
			mbegin += n;
		else
			mbegin = mend;
    }

    //*************************************************************************
    /// Shrinks the view by moving its end backward.
    //*************************************************************************
    void remove_suffix(const size_type n)
    {
		if (n < size())
			mend -= n;
		else
			mend = mbegin;
    }

    //*************************************************************************
    /// Fills the array.
    //*************************************************************************
    void fill(const T& value)
    {
      tpn::fill(begin(), end(), value);
    }

    //*************************************************************************
    /// Equality for array views.
    //*************************************************************************
    friend bool operator == (const array_view<T>& lhs, const array_view<T>& rhs)
    {
      return (lhs.size() == rhs.size()) &&
             tpn::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    //*************************************************************************
    /// Inequality for array views.
    //*************************************************************************
    friend bool operator != (const array_view<T>& lhs, const array_view<T>& rhs)
    {
      return !(lhs == rhs);
    }

    //*************************************************************************
    /// Less-than for array views.
    //*************************************************************************
    friend bool operator < (const array_view<T>& lhs, const array_view<T>& rhs)
    {
      return tpn::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    //*************************************************************************
    /// Greater-than for array views.
    //*************************************************************************
    friend bool operator > (const array_view<T>& lhs, const array_view<T>& rhs)
    {
      return rhs < lhs;
    }

    //*************************************************************************
    /// Less-than-equal for array views.
    //*************************************************************************
    friend bool operator <= (const array_view<T>& lhs, const array_view<T>& rhs)
    {
      return !(lhs > rhs);
    }

    //*************************************************************************
    /// Greater-than-equal for array views.
    //*************************************************************************
    friend bool operator >= (const array_view<T>& lhs, const array_view<T>& rhs)
    {
      return !(lhs < rhs);
    }

  private:

    pointer mbegin;
    pointer mend;
  };

  //*************************************************************************
  /// Template deduction guides.
  //*************************************************************************
#if TYPHOON_USING_CPP17
  template <typename TArray>
  array_view(TArray& a) 
    -> array_view<typename TArray::value_type>;

  template <typename TIterator>
  array_view(const TIterator begin_, const TIterator end_)
    -> array_view<tpn::remove_pointer_t<TIterator>>;

  template <typename TIterator,
            typename TSize>
  array_view(const TIterator begin_, const TSize size_)
    -> array_view<tpn::remove_pointer_t<TIterator>>;
#endif  

  //*************************************************************************
  /// Hash function.
  //*************************************************************************
#if TYPHOON_USING_8BIT_TYPES
  template <typename T>
  struct hash<tpn::array_view<T> >
  {
    size_t operator()(const tpn::array_view<T>& view) const
    {
      return tpn::private_hash::generic_hash<size_t>(reinterpret_cast<const uint8_t*>(&view[0]),
                                                     reinterpret_cast<const uint8_t*>(&view[view.size()]));
    }
  };
#endif
}

//*************************************************************************
/// Swaps the values.
//*************************************************************************
template <typename T>
void swap(tpn::array_view<T>& lhs, tpn::array_view<T>& rhs)
{
  lhs.swap(rhs);
}

#endif
