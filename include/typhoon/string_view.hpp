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

#ifndef TYPHOON_STRING_VIEW_HPP
#define TYPHOON_STRING_VIEW_HPP

#include "platform.hpp"
#include "memory.hpp"
#include "iterator.hpp"
#include "error_handler.hpp"
#include "exception.hpp"
#include "char_traits.hpp"
#include "integral_limits.hpp"
#include "hash.hpp"
#include "basic_string.hpp"
#include "algorithm.hpp"
#include "private/minmax_push.hpp"

#include <stdint.h>

namespace tpn
{
  //***************************************************************************
  /// The base class for basic_string_view exceptions.
  //***************************************************************************
  class string_view_exception : public exception
  {
  public:

    string_view_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  ///\ingroup stack
  /// The exception thrown when the index is out of bounds.
  //***************************************************************************
  class string_view_bounds : public string_view_exception
  {
  public:

    string_view_bounds(string_type file_name_, numeric_type line_number_)
      : string_view_exception(TYPHOON_ERROR_TEXT("basic_string_view:bounds", TYPHOON_STRING_VIEW_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  ///\ingroup stack
  /// The exception thrown when the view is uninitialised.
  //***************************************************************************
  class string_view_uninitialised : public string_view_exception
  {
  public:

    string_view_uninitialised(string_type file_name_, numeric_type line_number_)
      : string_view_exception(TYPHOON_ERROR_TEXT("basic_string_view:uninitialised", TYPHOON_STRING_VIEW_FILE_ID"B"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// String view.
  //***************************************************************************
  template <typename T, typename TTraits = tpn::char_traits<T> >
  class basic_string_view
  {
  public:

    typedef T                                     value_type;
    typedef TTraits                               traits_type;
    typedef size_t                                size_type;
    typedef const T& const_reference;
    typedef const T* const_pointer;
    typedef const T* const_iterator;
    typedef TYPHOON_OR_STD::reverse_iterator<const_iterator> const_reverse_iterator;

    enum
    {
      npos = tpn::integral_limits<size_t>::max
    };

    //*************************************************************************
    /// Default constructor.
    //*************************************************************************
    TYPHOON_CONSTEXPR basic_string_view() TYPHOON_NOEXCEPT
      : mbegin(TYPHOON_NULLPTR)
      , mend(TYPHOON_NULLPTR)
    {
    }

    //*************************************************************************
    /// Construct from string.
    //*************************************************************************
    TYPHOON_CONSTEXPR basic_string_view(const tpn::ibasic_string<T>& str)
      : mbegin(str.begin())
      , mend(str.end())
    {
    }

    //*************************************************************************
    /// Construct from T*.
    //*************************************************************************
    TYPHOON_CONSTEXPR14  TYPHOON_EXPLICIT_STRING_FROM_CHAR basic_string_view(const T* begin_)
      : mbegin(begin_)
      , mend(begin_ + TTraits::length(begin_))
    {
    }

    //*************************************************************************
    /// Construct from pointer range.
    //*************************************************************************
    TYPHOON_CONSTEXPR basic_string_view(const T* begin_, const T* end_)
      : mbegin(begin_)
      , mend(end_)
    {
    }

    //*************************************************************************
    /// Construct from pointer/size.
    //*************************************************************************
    TYPHOON_CONSTEXPR basic_string_view(const T* begin_, size_t size_)
      : mbegin(begin_)
      , mend(begin_ + size_)
    {
    }

    //*************************************************************************
    /// Copy constructor
    //*************************************************************************
    TYPHOON_CONSTEXPR basic_string_view(const basic_string_view& other) TYPHOON_NOEXCEPT
      : mbegin(other.mbegin)
      , mend(other.mend)
    {
    }

    //*************************************************************************
    /// Returns a const reference to the first element.
    //*************************************************************************
    TYPHOON_CONSTEXPR const_reference front() const
    {
      return *mbegin;
    }

    //*************************************************************************
    /// Returns a const reference to the last element.
    //*************************************************************************
    TYPHOON_CONSTEXPR const_reference back() const
    {
      return *(mend - 1);
    }

    //*************************************************************************
    /// Returns a const pointer to the first element of the internal storage.
    //*************************************************************************
    TYPHOON_CONSTEXPR const_pointer data() const
    {
      return mbegin;
    }

    //*************************************************************************
    /// Returns a const iterator to the beginning of the array.
    //*************************************************************************
    TYPHOON_CONSTEXPR const_iterator begin() const
    {
      return mbegin;
    }

    //*************************************************************************
    /// Returns a const iterator to the beginning of the array.
    //*************************************************************************
    TYPHOON_CONSTEXPR const_iterator cbegin() const
    {
      return mbegin;
    }

    //*************************************************************************
    /// Returns a const iterator to the end of the array.
    //*************************************************************************
    TYPHOON_CONSTEXPR const_iterator end() const
    {
      return mend;
    }

    //*************************************************************************
    // Returns a const iterator to the end of the array.
    //*************************************************************************
    TYPHOON_CONSTEXPR const_iterator cend() const
    {
      return mend;
    }

    //*************************************************************************
    /// Returns a const reverse iterator to the reverse beginning of the array.
    //*************************************************************************
    TYPHOON_CONSTEXPR const_reverse_iterator rbegin() const
    {
      return const_reverse_iterator(mend);
    }

    //*************************************************************************
    /// Returns a const reverse iterator to the reverse beginning of the array.
    //*************************************************************************
    TYPHOON_CONSTEXPR const_reverse_iterator crbegin() const
    {
      return const_reverse_iterator(mend);
    }

    //*************************************************************************
    /// Returns a const reverse iterator to the end of the array.
    //*************************************************************************
    TYPHOON_CONSTEXPR const_reverse_iterator rend() const
    {
      return const_reverse_iterator(mbegin);
    }

    //*************************************************************************
    /// Returns a const reverse iterator to the end of the array.
    //*************************************************************************
    TYPHOON_CONSTEXPR const_reverse_iterator crend() const
    {
      return const_reverse_iterator(mbegin);
    }

    //*************************************************************************
    // Capacity
    //*************************************************************************

    //*************************************************************************
    /// Returns <b>true</b> if the array size is zero.
    //*************************************************************************
    TYPHOON_CONSTEXPR bool empty() const
    {
      return (mbegin == mend);
    }

    //*************************************************************************
    /// Returns the size of the array.
    //*************************************************************************
    TYPHOON_CONSTEXPR size_t size() const
    {
      return (mend - mbegin);
    }

    //*************************************************************************
    /// Returns the size of the array.
    //*************************************************************************
    TYPHOON_CONSTEXPR size_t length() const
    {
      return size();
    }

    //*************************************************************************
    /// Returns the maximum possible size of the array.
    //*************************************************************************
    TYPHOON_CONSTEXPR size_t max_size() const
    {
      return size();
    }

    //*************************************************************************
    /// Assign from a view.
    //*************************************************************************
    TYPHOON_CONSTEXPR14 tpn::basic_string_view<T, TTraits>& operator=(const tpn::basic_string_view<T, TTraits>& other)
    {
      mbegin = other.mbegin;
      mend   = other.mend;
      return *this;
    }

    //*************************************************************************
    /// Assign from iterators
    //*************************************************************************
    TYPHOON_CONSTEXPR14 void assign(const_pointer begin_, const_pointer end_)
    {
      mbegin = begin_;
      mend   = end_;
    }

    //*************************************************************************
    /// Assign from iterator and size.
    //*************************************************************************
    TYPHOON_CONSTEXPR14 void assign(const_pointer begin_, size_t size_)
    {
      mbegin = begin_;
      mend   = begin_ + size_;
    }

    //*************************************************************************
    /// Returns a const reference to the indexed value.
    //*************************************************************************
    TYPHOON_CONSTEXPR const_reference operator[](size_t i) const
    {
      return mbegin[i];
    }

    //*************************************************************************
    /// Returns a const reference to the indexed value.
    //*************************************************************************
    TYPHOON_CONSTEXPR const_reference at(size_t i) const
    {
      TYPHOON_ASSERT((mbegin != TYPHOON_NULLPTR && mend != TYPHOON_NULLPTR), TYPHOON_ERROR(string_view_uninitialised));
      TYPHOON_ASSERT(i < size(), TYPHOON_ERROR(string_view_bounds));
      return mbegin[i];
    }

    //*************************************************************************
    /// Swaps with another basic_string_view.
    //*************************************************************************
    TYPHOON_CONSTEXPR14 void swap(basic_string_view& other) TYPHOON_NOEXCEPT
    {
      using TYPHOON_OR_STD::swap; // Allow ADL

      swap(mbegin, other.mbegin);
      swap(mend, other.mend);
    }

    //*************************************************************************
    /// Copies characters
    //*************************************************************************
    TYPHOON_CONSTEXPR14 size_type copy(T* destination, size_type count, size_type position = 0) const
    {
      size_t n = 0UL;

      if (position < size())
      {
        n = tpn::min(count, size() - position);

        tpn::copy(mbegin + position, mbegin + position + n, destination);
      }

      return n;
    }

    //*************************************************************************
    /// Returns a substring
    //*************************************************************************
    TYPHOON_CONSTEXPR14 basic_string_view substr(size_type position = 0, size_type count = npos) const
    {
      basic_string_view view;

      if (position < size())
      {
        size_t n = tpn::min(count, size() - position);

        view = basic_string_view(mbegin + position, mbegin + position + n);
      }

      return view;
    }

    //*************************************************************************
    /// Shrinks the view by moving its start forward.
    //*************************************************************************
    TYPHOON_CONSTEXPR14 void remove_prefix(size_type n)
    {
      mbegin += n;
    }

    //*************************************************************************
    /// Shrinks the view by moving its end backward.
    //*************************************************************************
    TYPHOON_CONSTEXPR14 void remove_suffix(size_type n)
    {
      mend -= n;
    }

    //*************************************************************************
    /// Compares two views
    //*************************************************************************
    TYPHOON_CONSTEXPR14 int compare(basic_string_view<T, TTraits> view) const
    {
      return (*this == view) ? 0 : ((*this > view) ? 1 : -1);
    }

    TYPHOON_CONSTEXPR14 int compare(size_type position, size_type count, basic_string_view view) const
    {
      return substr(position, count).compare(view);
    }

    TYPHOON_CONSTEXPR14 int compare(size_type position1, size_type count1,
      basic_string_view view,
      size_type position2, size_type count2) const
    {
      return substr(position1, count1).compare(view.substr(position2, count2));
    }

    TYPHOON_CONSTEXPR14 int compare(const T* text) const
    {
      return compare(tpn::basic_string_view<T, TTraits>(text));
    }

    TYPHOON_CONSTEXPR14 int compare(size_type position, size_type count, const T* text) const
    {
      return substr(position, count).compare(tpn::basic_string_view<T, TTraits>(text));
    }

    TYPHOON_CONSTEXPR14 int compare(size_type position, size_type count1, const T* text, size_type count2) const
    {
      return substr(position, count1).compare(tpn::basic_string_view<T, TTraits>(text, count2));
    }

    //*************************************************************************
    /// Checks if the string view starts with the given prefix
    //*************************************************************************
    TYPHOON_CONSTEXPR14 bool starts_with(tpn::basic_string_view<T, TTraits> view) const
    {
      return (size() >= view.size()) &&
              (compare(0, view.size(), view) == 0);
    }

    TYPHOON_CONSTEXPR14 bool starts_with(T c) const
    {
      return !empty() && (front() == c);
    }

    TYPHOON_CONSTEXPR14 bool starts_with(const T* text) const
    {
      size_t lengthtext = TTraits::length(text);

      return (size() >= lengthtext) &&
              (compare(0, lengthtext, text) == 0);
    }

    //*************************************************************************
    /// Checks if the string view ends with the given suffix
    //*************************************************************************
    TYPHOON_CONSTEXPR14 bool ends_with(tpn::basic_string_view<T, TTraits> view) const
    {
      return (size() >= view.size()) &&
              (compare(size() - view.size(), npos, view) == 0);
    }

    TYPHOON_CONSTEXPR14 bool ends_with(T c) const
    {
      return !empty() && (back() == c);
    }

    TYPHOON_CONSTEXPR14 bool ends_with(const T* text) const
    {
      size_t lengthtext = TTraits::length(text);
      size_t lengthview = size();

      return (lengthview >= lengthtext) &&
              (compare(lengthview - lengthtext, lengthtext, text) == 0);
    }

    //*************************************************************************
    /// Find characters in the view
    //*************************************************************************
    TYPHOON_CONSTEXPR14 size_type find(tpn::basic_string_view<T, TTraits> view, size_type position = 0) const
    {
      if ((size() < view.size()))
      {
        return npos;
      }

      const_iterator iposition = tpn::search(begin() + position, end(), view.begin(), view.end());

      if (iposition == end())
      {
        return npos;
      }
      else
      {
        return tpn::distance(begin(), iposition);
      }
    }

    TYPHOON_CONSTEXPR14 size_type find(T c, size_type position = 0) const
    {
      return find(tpn::basic_string_view<T, TTraits>(&c, 1), position);
    }

    TYPHOON_CONSTEXPR14 size_type find(const T* text, size_type position, size_type count) const
    {
      return find(tpn::basic_string_view<T, TTraits>(text, count), position);
    }

    TYPHOON_CONSTEXPR14 size_type find(const T* text, size_type position = 0) const
    {
      return find(tpn::basic_string_view<T, TTraits>(text), position);
    }

    //*************************************************************************
    /// Find the last occurrence of a substring
    //*************************************************************************
    TYPHOON_CONSTEXPR14 size_type rfind(tpn::basic_string_view<T, TTraits> view, size_type position = npos) const
    {
      if ((size() < view.size()))
      {
        return npos;
      }

      position = tpn::min(position, size());

      const_iterator iposition = tpn::find_end(begin(),
        begin() + position,
        view.begin(),
        view.end());

      if (iposition == end())
      {
        return npos;
      }
      else
      {
        return tpn::distance(begin(), iposition);
      }
    }

    TYPHOON_CONSTEXPR14 size_type rfind(T c, size_type position = npos) const
    {
      return rfind(tpn::basic_string_view<T, TTraits>(&c, 1), position);
    }

    TYPHOON_CONSTEXPR14 size_type rfind(const T* text, size_type position, size_type count) const
    {
      return rfind(tpn::basic_string_view<T, TTraits>(text, count), position);
    }

    TYPHOON_CONSTEXPR14 size_type rfind(const T* text, size_type position = npos) const
    {
      return rfind(tpn::basic_string_view<T, TTraits>(text), position);
    }

    //*************************************************************************
    ///  Find first occurrence of characters
    //*************************************************************************
    TYPHOON_CONSTEXPR14 size_type find_first_of(tpn::basic_string_view<T, TTraits> view, size_type position = 0) const
    {
      const size_t lengthtext = size();

      if (position < lengthtext)
      {
        for (size_t i = position; i < lengthtext; ++i)
        {
          const size_t lengthview = view.size();

          for (size_t j = 0UL; j < lengthview; ++j)
          {
            if (mbegin[i] == view[j])
            {
              return i;
            }
          }
        }
      }

      return npos;
    }

    TYPHOON_CONSTEXPR14 size_type find_first_of(T c, size_type position = 0) const
    {
      return find_first_of(tpn::basic_string_view<T, TTraits>(&c, 1), position);
    }

    TYPHOON_CONSTEXPR14 size_type find_first_of(const T* text, size_type position, size_type count) const
    {
      return find_first_of(tpn::basic_string_view<T, TTraits>(text, count), position);
    }

    TYPHOON_CONSTEXPR14 size_type find_first_of(const T* text, size_type position = 0) const
    {
      return find_first_of(tpn::basic_string_view<T, TTraits>(text), position);
    }

    //*************************************************************************
    /// Find last occurrence of characters
    //*************************************************************************
    TYPHOON_CONSTEXPR14 size_type find_last_of(tpn::basic_string_view<T, TTraits> view, size_type position = npos) const
    {
      if (empty())
      {
        return npos;
      }

      position = tpn::min(position, size() - 1);

      const_reverse_iterator it = rbegin() + size() - position - 1;

      while (it != rend())
      {
        const size_t viewlength = view.size();

        for (size_t j = 0UL; j < viewlength; ++j)
        {
          if (mbegin[position] == view[j])
          {
            return position;
          }
        }

        ++it;
        --position;
      }

      return npos;
    }

    TYPHOON_CONSTEXPR14 size_type find_last_of(T c, size_type position = npos) const
    {
      return find_last_of(tpn::basic_string_view<T, TTraits>(&c, 1), position);
    }

    TYPHOON_CONSTEXPR14 size_type find_last_of(const T* text, size_type position, size_type count) const
    {
      return find_last_of(tpn::basic_string_view<T, TTraits>(text, count), position);
    }

    TYPHOON_CONSTEXPR14 size_type find_last_of(const T* text, size_type position = npos) const
    {
      return find_last_of(tpn::basic_string_view<T, TTraits>(text), position);
    }

    //*************************************************************************
    /// Find first absence of characters
    //*************************************************************************
    TYPHOON_CONSTEXPR14 size_type find_first_not_of(tpn::basic_string_view<T, TTraits> view, size_type position = 0) const
    {
      const size_t lengthtext = size();

      if (position < lengthtext)
      {
        for (size_t i = position; i < lengthtext; ++i)
        {
          bool found = false;

          const size_t viewlength = view.size();

          for (size_t j = 0UL; j < viewlength; ++j)
          {
            if (mbegin[i] == view[j])
            {
              found = true;
            }
          }

          if (!found)
          {
            return i;
          }
        }
      }

      return npos;
    }

    TYPHOON_CONSTEXPR14 size_type find_first_not_of(T c, size_type position = 0) const
    {
      return find_first_not_of(tpn::basic_string_view<T, TTraits>(&c, 1), position);
    }

    TYPHOON_CONSTEXPR14 size_type find_first_not_of(const T* text, size_type position, size_type count) const
    {
      return find_first_not_of(tpn::basic_string_view<T, TTraits>(text, count), position);
    }

    TYPHOON_CONSTEXPR14 size_type find_first_not_of(const T* text, size_type position = 0) const
    {
      return find_first_not_of(tpn::basic_string_view<T, TTraits>(text), position);
    }

    //*************************************************************************
    /// Find last absence of characters
    //*************************************************************************
    TYPHOON_CONSTEXPR14 size_type find_last_not_of(tpn::basic_string_view<T, TTraits> view, size_type position = npos) const
    {
      if (empty())
      {
        return npos;
      }

      position = tpn::min(position, size() - 1);

      const_reverse_iterator it = rbegin() + size() - position - 1;

      while (it != rend())
      {
        bool found = false;

        const size_t viewlength = view.size();

        for (size_t j = 0UL; j < viewlength; ++j)
        {
          if (mbegin[position] == view[j])
          {
            found = true;
          }
        }

        if (!found)
        {
          return position;
        }

        ++it;
        --position;
      }

      return npos;
    }

    TYPHOON_CONSTEXPR14 size_type find_last_not_of(T c, size_type position = npos) const
    {
      return find_last_not_of(tpn::basic_string_view<T, TTraits>(&c, 1), position);
    }

    TYPHOON_CONSTEXPR14 size_type find_last_not_of(const T* text, size_type position, size_type count) const
    {
      return find_last_not_of(tpn::basic_string_view<T, TTraits>(text, count), position);
    }

    TYPHOON_CONSTEXPR14 size_type find_last_not_of(const T* text, size_type position = npos) const
    {
      return find_last_not_of(tpn::basic_string_view<T, TTraits>(text), position);
    }

    //*************************************************************************
    /// Equality for string_view.
    //*************************************************************************
    friend TYPHOON_CONSTEXPR14 bool operator == (const tpn::basic_string_view<T, TTraits>& lhs, const tpn::basic_string_view<T, TTraits>& rhs)
    {
      return (lhs.size() == rhs.size()) &&
              tpn::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    //*************************************************************************
    /// Inequality for string_view.
    //*************************************************************************
    friend TYPHOON_CONSTEXPR14 bool operator != (const tpn::basic_string_view<T, TTraits>& lhs, const tpn::basic_string_view<T, TTraits>& rhs)
    {
      return !(lhs == rhs);
    }

    //*************************************************************************
    /// Less-than for string_view.
    //*************************************************************************
    friend TYPHOON_CONSTEXPR14 bool operator < (const tpn::basic_string_view<T, TTraits>& lhs, const tpn::basic_string_view<T, TTraits>& rhs)
    {
      return tpn::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    //*************************************************************************
    /// Greater-than for string_view.
    //*************************************************************************
    friend TYPHOON_CONSTEXPR14 bool operator > (const tpn::basic_string_view<T, TTraits>& lhs, const tpn::basic_string_view<T, TTraits>& rhs)
    {
      return rhs < lhs;
    }

    //*************************************************************************
    /// Less-than-equal for string_view.
    //*************************************************************************
    friend TYPHOON_CONSTEXPR14 bool operator <= (const tpn::basic_string_view<T, TTraits>& lhs, const tpn::basic_string_view<T, TTraits>& rhs)
    {
      return !(lhs > rhs);
    }

    //*************************************************************************
    /// Greater-than-equal for string_view.
    //*************************************************************************
    friend TYPHOON_CONSTEXPR14 bool operator >= (const tpn::basic_string_view<T, TTraits>& lhs, const tpn::basic_string_view<T, TTraits>& rhs)
    {
      return !(lhs < rhs);
    }

  private:

    const_pointer mbegin;
    const_pointer mend;
  };

  typedef tpn::basic_string_view<char>     string_view;
  typedef tpn::basic_string_view<wchar_t>  wstring_view;
  typedef tpn::basic_string_view<char16_t> u16string_view;
  typedef tpn::basic_string_view<char32_t> u32string_view;

  //*************************************************************************
  /// make_string_view.
  //*************************************************************************
  template<size_t ARRAY_SIZE>
  TYPHOON_CONSTEXPR14 string_view make_string_view(const char(&text)[ARRAY_SIZE])
  {
    size_t length = tpn::char_traits<char>::length(text, ARRAY_SIZE - 1U);

    return string_view(text, length);
  }

  //***********************************
  template<size_t ARRAY_SIZE>
  TYPHOON_CONSTEXPR14 wstring_view make_string_view(const wchar_t(&text)[ARRAY_SIZE])
  {
    size_t length = tpn::char_traits<wchar_t>::length(text, ARRAY_SIZE - 1U);

    return wstring_view(text, length);
  }

  //***********************************
  template<size_t ARRAY_SIZE>
  TYPHOON_CONSTEXPR14 u16string_view make_string_view(const char16_t(&text)[ARRAY_SIZE])
  {
    size_t length = tpn::char_traits<char16_t>::length(text, ARRAY_SIZE - 1U);

    return u16string_view(text, length);
  }

  //***********************************
  template<size_t ARRAY_SIZE>
  TYPHOON_CONSTEXPR14 u32string_view make_string_view(const char32_t(&text)[ARRAY_SIZE])
  {
    size_t length = tpn::char_traits<char32_t>::length(text, ARRAY_SIZE - 1U);

    return u32string_view(text, length);
  }

  //*************************************************************************
  /// Hash function.
  //*************************************************************************
#if TYPHOON_USING_8BIT_TYPES
  template <>
  struct hash<tpn::string_view>
  {
    size_t operator()(const tpn::string_view& text) const
    {
      return tpn::private_hash::generic_hash<size_t>(reinterpret_cast<const uint8_t*>(&text[0]),
        reinterpret_cast<const uint8_t*>(&text[text.size()]));
    }
  };

  template <>
  struct hash<tpn::wstring_view>
  {
    size_t operator()(const tpn::wstring_view& text) const
    {
      return tpn::private_hash::generic_hash<size_t>(reinterpret_cast<const uint8_t*>(&text[0]),
        reinterpret_cast<const uint8_t*>(&text[text.size()]));
    }
  };

  template <>
  struct hash<tpn::u16string_view>
  {
    size_t operator()(const tpn::u16string_view& text) const
    {
      return tpn::private_hash::generic_hash<size_t>(reinterpret_cast<const uint8_t*>(&text[0]),
        reinterpret_cast<const uint8_t*>(&text[text.size()]));
    }
  };

  template <>
  struct hash<tpn::u32string_view>
  {
    size_t operator()(const tpn::u32string_view& text) const
    {
      return tpn::private_hash::generic_hash<size_t>(reinterpret_cast<const uint8_t*>(&text[0]),
        reinterpret_cast<const uint8_t*>(&text[text.size()]));
    }
  };
#endif
}

//*************************************************************************
/// Swaps the values.
//*************************************************************************
template <typename T, typename TTraits >
void swap(tpn::basic_string_view<T, TTraits>& lhs, tpn::basic_string_view<T, TTraits>& rhs)
{
  lhs.swap(rhs);
}

template <typename T>
void swap(tpn::basic_string_view<T, tpn::char_traits<T> >& lhs, tpn::basic_string_view<T, tpn::char_traits<T> >& rhs)
{
  lhs.swap(rhs);
}

#include "private/minmax_pop.hpp"

#endif

