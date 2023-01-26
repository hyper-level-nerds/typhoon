///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tpn
https://www.tpncpp.com

Copyright(c) 2016 John Wellbelove

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

#ifndef TYPHOON_WSTRING_HPP
#define TYPHOON_WSTRING_HPP

#include "platform.hpp"
#include "basic_string.hpp"
#include "string_view.hpp"
#include "hash.hpp"
#include "initializer_list.hpp"

#include "private/minmax_push.hpp"

namespace tpn
{
  typedef ibasic_string<wchar_t> iwstring;

  //***************************************************************************
  /// A wstring implementation that uses a fixed size buffer.
  ///\tparam MAX_SIZE_ The maximum number of elements that can be stored.
  ///\ingroup wstring
  //***************************************************************************
  template <const size_t MAX_SIZE_>
  class wstring : public iwstring
  {
  public:

    typedef iwstring base_type;
    typedef iwstring interface_type;

    typedef iwstring::value_type value_type;

    static const size_t MAX_SIZE = MAX_SIZE_;

    //*************************************************************************
    /// Constructor.
    //*************************************************************************
    wstring()
      : iwstring(reinterpret_cast<value_type*>(&buffer), MAX_SIZE)
    {
      this->initialise();
    }

    //*************************************************************************
    /// Copy constructor.
    ///\param other The other string.
    //*************************************************************************
    wstring(const tpn::wstring<MAX_SIZE_>& other)
      : iwstring(reinterpret_cast<value_type*>(&buffer), MAX_SIZE)
    {
      this->assign(other);
    }

    //*************************************************************************
    /// From other iwstring.
    ///\param other The other iwstring.
    //*************************************************************************
    wstring(const tpn::iwstring& other)
      : iwstring(reinterpret_cast<value_type*>(&buffer), MAX_SIZE)
    {
      this->assign(other);
    }

    //*************************************************************************
    /// From other string, position, length.
    ///\param other The other string.
    ///\param position The position of the first character.
    ///\param length   The number of characters. Default = npos.
    //*************************************************************************
    wstring(const tpn::iwstring& other, size_type position, size_type length = npos)
      : iwstring(reinterpret_cast<value_type*>(&buffer), MAX_SIZE)
    {
      TYPHOON_ASSERT(position < other.size(), TYPHOON_ERROR(string_out_of_bounds));

      this->assign(other, position, length);
    }

    //*************************************************************************
    /// Constructor, from null terminated text.
    ///\param text The initial text of the wstring.
    //*************************************************************************
    TYPHOON_EXPLICIT_STRING_FROM_CHAR wstring(const value_type* text)
      : iwstring(reinterpret_cast<value_type*>(&buffer), MAX_SIZE)
    {
      this->assign(text, text + tpn::char_traits<value_type>::length(text));
    }

    //*************************************************************************
    /// Constructor, from null terminated text and count.
    ///\param text  The initial text of the wstring.
    ///\param count The number of characters to copy.
    //*************************************************************************
    wstring(const value_type* text, size_type count)
      : iwstring(reinterpret_cast<value_type*>(&buffer), MAX_SIZE)
    {
      this->assign(text, text + count);
    }

    //*************************************************************************
    /// Constructor, from initial size and value.
    ///\param initialSize  The initial size of the wstring.
    ///\param value        The value to fill the wstring with.
    //*************************************************************************
    wstring(size_type count, value_type c)
      : iwstring(reinterpret_cast<value_type*>(&buffer), MAX_SIZE)
    {
      this->initialise();
      this->resize(count, c);
    }

    //*************************************************************************
    /// Constructor, from an iterator range.
    ///\tparam TIterator The iterator type.
    ///\param first The iterator to the first element.
    ///\param last  The iterator to the last element + 1.
    //*************************************************************************
    template <typename TIterator>
    wstring(TIterator first, TIterator last, typename tpn::enable_if<!tpn::is_integral<TIterator>::value, int>::type = 0)
      : iwstring(reinterpret_cast<value_type*>(&buffer), MAX_SIZE)
    {
      this->assign(first, last);
    }

#if TYPHOON_HAS_INITIALIZER_LIST
    //*************************************************************************
    /// Construct from initializer_list.
    //*************************************************************************
    wstring(std::initializer_list<value_type> init)
      : iwstring(reinterpret_cast<value_type*>(&buffer), MAX_SIZE)
    {
      this->assign(init.begin(), init.end());
    }
#endif

    //*************************************************************************
    /// From string_view.
    ///\param view The string_view.
    //*************************************************************************
    explicit wstring(const tpn::wstring_view& view)
      : iwstring(reinterpret_cast<value_type*>(&buffer), MAX_SIZE)
    {
      this->assign(view.begin(), view.end());
    }

    //*************************************************************************
    /// Returns a sub-string.
    ///\param position The position of the first character. Default = 0.
    ///\param length   The number of characters. Default = npos.
    //*************************************************************************
    tpn::wstring<MAX_SIZE_> substr(size_type position = 0, size_type length_ = npos) const
    {
      tpn::wstring<MAX_SIZE_> new_string;

      if (position != size())
      {
        TYPHOON_ASSERT(position < size(), TYPHOON_ERROR(string_out_of_bounds));

        length_ = tpn::min(length_, size() - position);

        new_string.assign(buffer + position, buffer + position + length_);
      }

      return new_string;
    }

    //*************************************************************************
    /// Assignment operator.
    //*************************************************************************
    wstring& operator = (const wstring& rhs)
    {
      if (&rhs != this)
      {
        this->assign(rhs);
      }

      return *this;
    }

    //*************************************************************************
    /// Assignment operator.
    //*************************************************************************
    wstring& operator = (const value_type* text)
    {
      this->assign(text);

      return *this;
    }

    //*************************************************************************
    /// Fix the internal pointers after a low level memory copy.
    //*************************************************************************
#if TYPHOON_HAS_ISTRING_REPAIR
    virtual
#endif
    void repair()
    {
      tpn::iwstring::repair_buffer(buffer);
    }

  private:

    value_type buffer[MAX_SIZE + 1];
  };

  //***************************************************************************
  /// A wstring implementation that uses a fixed size external buffer.
  ///\ingroup wstring
  //***************************************************************************
  class wstring_ext : public iwstring
  {
  public:

    typedef iwstring base_type;
    typedef iwstring interface_type;

    typedef iwstring::value_type value_type;

    //*************************************************************************
    /// Constructor.
    //*************************************************************************
    wstring_ext(value_type* buffer, size_type buffer_size)
      : iwstring(buffer, buffer_size - 1U)
    {
      this->initialise();
    }

    //*************************************************************************
    /// Copy constructor.
    ///\param other The other wstring_ext.
    //*************************************************************************
    wstring_ext(const tpn::wstring_ext& other, value_type* buffer, size_type buffer_size)
      : iwstring(buffer, buffer_size - 1U)
    {
      this->assign(other);
    }

    //*************************************************************************
    /// From other iwstring.
    ///\param other The other iwstring.
    //*************************************************************************
    wstring_ext(const tpn::iwstring& other, value_type* buffer, size_type buffer_size)
      : iwstring(buffer, buffer_size - 1U)
    {
      this->assign(other);
    }

    //*************************************************************************
    /// From other wstring_ext, position, length.
    ///\param other The other wstring_ext.
    ///\param position The position of the first character.
    ///\param length   The number of characters. Default = npos.
    //*************************************************************************
    wstring_ext(const tpn::iwstring& other, value_type* buffer, size_type buffer_size, size_type position, size_type length = npos)
      : iwstring(buffer, buffer_size - 1U)
    {
      TYPHOON_ASSERT(position < other.size(), TYPHOON_ERROR(string_out_of_bounds));

      this->assign(other, position, length);
    }

    //*************************************************************************
    /// Constructor, from null terminated text.
    ///\param text The initial text of the wstring_ext.
    //*************************************************************************
    wstring_ext(const value_type* text, value_type* buffer, size_type buffer_size)
      : iwstring(buffer, buffer_size - 1U)
    {
      // Is the initial text at the same address as the buffer?
      if (text == buffer)
      {
        this->current_size = tpn::strlen(buffer);
      }
      else
      {
        this->assign(text, text + tpn::strlen(text));
      }
    }

    //*************************************************************************
    /// Constructor, from null terminated text and count.
    ///\param text  The initial text of the wstring_ext.
    ///\param count The number of characters to copy.
    //*************************************************************************
    wstring_ext(const value_type* text, size_type count, value_type* buffer, size_type buffer_size)
      : iwstring(buffer, buffer_size - 1U)
    {
      this->assign(text, text + count);
    }

    //*************************************************************************
    /// Constructor, from initial size and value.
    ///\param initialSize  The initial size of the wstring_ext.
    ///\param value        The value to fill the wstring_ext with.
    //*************************************************************************
    wstring_ext(size_type count, value_type c, value_type* buffer, size_type buffer_size)
      : iwstring(buffer, buffer_size - 1U)
    {
      this->initialise();
      this->resize(count, c);
    }

    //*************************************************************************
    /// Constructor, from an iterator range.
    ///\tparam TIterator The iterator type.
    ///\param first The iterator to the first element.
    ///\param last  The iterator to the last element + 1.
    //*************************************************************************
    template <typename TIterator>
    wstring_ext(TIterator first, TIterator last, value_type* buffer, size_type buffer_size, typename tpn::enable_if<!tpn::is_integral<TIterator>::value, int>::type = 0)
      : iwstring(buffer, buffer_size - 1U)
    {
      this->assign(first, last);
    }

#if TYPHOON_HAS_INITIALIZER_LIST
    //*************************************************************************
    /// Construct from initializer_list.
    //*************************************************************************
    wstring_ext(std::initializer_list<value_type> init, value_type* buffer, size_type buffer_size)
      : iwstring(buffer, buffer_size - 1U)
    {
      this->assign(init.begin(), init.end());
    }
#endif

    //*************************************************************************
    /// From wstring_view.
    ///\param view The wstring_view.
    //*************************************************************************
    explicit wstring_ext(const tpn::wstring_view& view, value_type* buffer, size_type buffer_size)
      : iwstring(buffer, buffer_size - 1U)
    {
      this->assign(view.begin(), view.end());
    }

    //*************************************************************************
    /// Assignment operator.
    //*************************************************************************
    wstring_ext& operator = (const wstring_ext& rhs)
    {
      if (&rhs != this)
      {
        this->assign(rhs);
      }

      return *this;
    }


    //*************************************************************************
    /// Assignment operator.
    //*************************************************************************
    wstring_ext& operator = (const iwstring& rhs)
    {
      if (&rhs != this)
      {
        this->assign(rhs);
      }

      return *this;
    }

    //*************************************************************************
    /// Assignment operator.
    //*************************************************************************
    wstring_ext& operator = (const value_type* text)
    {
      this->assign(text);

      return *this;
    }

    //*************************************************************************
    /// Fix the internal pointers after a low level memory copy.
    //*************************************************************************
    void repair()
#if TYPHOON_HAS_ISTRING_REPAIR
      TYPHOON_OVERRIDE
#endif
    {
    }

  private:

    //*************************************************************************
    /// Deleted.
    //*************************************************************************
    wstring_ext(const wstring_ext& other) TYPHOON_DELETE;
  };

  //*************************************************************************
  /// Hash function.
  //*************************************************************************
#if TYPHOON_USING_8BIT_TYPES
  template <>
  struct hash<tpn::iwstring>
  {
    size_t operator()(const tpn::iwstring& text) const
    {
      return tpn::private_hash::generic_hash<size_t>(reinterpret_cast<const uint8_t*>(&text[0]),
                                                     reinterpret_cast<const uint8_t*>(&text[text.size()]));
    }
  };

  template <const size_t SIZE>
  struct hash<tpn::wstring<SIZE> >
  {
    size_t operator()(const tpn::wstring<SIZE>& text) const
    {
      return tpn::private_hash::generic_hash<size_t>(reinterpret_cast<const uint8_t*>(&text[0]),
                                                     reinterpret_cast<const uint8_t*>(&text[text.size()]));
    }
  };

  template <>
  struct hash<tpn::wstring_ext>
  {
    size_t operator()(const tpn::wstring_ext& text) const
    {
      return tpn::private_hash::generic_hash<size_t>(reinterpret_cast<const uint8_t*>(&text[0]),
                                                     reinterpret_cast<const uint8_t*>(&text[text.size()]));
    }
  };
#endif

  //***************************************************************************
  /// Make string from string literal or array
  //***************************************************************************
  template<size_t ARRAY_SIZE>
  tpn::wstring<ARRAY_SIZE - 1U> make_string(const wchar_t(&text)[ARRAY_SIZE])
  {
    return tpn::wstring<ARRAY_SIZE - 1U>(text, tpn::strlen(text, ARRAY_SIZE - 1U));
  }

  //***************************************************************************
  /// Make string with max capacity from string literal or array
  //***************************************************************************
  template<const size_t MAX_SIZE, const size_t SIZE>
  tpn::wstring<MAX_SIZE> make_string_with_capacity(const wchar_t(&text)[SIZE])
  {
    return tpn::wstring<MAX_SIZE>(text, tpn::strlen(text, SIZE));
  }
}

#include "private/minmax_pop.hpp"

#endif
