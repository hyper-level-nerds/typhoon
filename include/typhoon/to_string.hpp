///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tpn
https://www.tpncpp.com

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

#ifndef TYPHOON_TO_STRING_HPP
#define TYPHOON_TO_STRING_HPP

///\ingroup string

#include "platform.hpp"
#include "type_traits.hpp"
#include "string.hpp"
#include "format_spec.hpp"
#include "private/to_string_helper.hpp"

namespace tpn
{
  //***************************************************************************
  /// Default format spec.
  /// !tpn::istring && !tpn::string_view
  //***************************************************************************
  template <typename T>
  typename tpn::enable_if<!tpn::is_same<T, tpn::istring>::value && !tpn::is_same<T, tpn::string_view>::value, const tpn::istring&>::type
    to_string(const T value, tpn::istring& str, bool append = false)
  {
    tpn::format_spec format;

    return private_to_string::to_string(value, str, format, append);
  }

  //***************************************************************************
  /// Supplied format spec.
  /// !tpn::istring && !tpn::string_view
  //***************************************************************************
  template <typename T>
  typename tpn::enable_if<!tpn::is_same<T, tpn::istring>::value && !tpn::is_same<T, tpn::string_view>::value, const tpn::istring&>::type
    to_string(const T value, tpn::istring& str, const tpn::format_spec& format, bool append = false)
  {
    return private_to_string::to_string(value, str, format, append);
  }

  //***************************************************************************
  /// Default format spec.
  /// !tpn::istring && !tpn::string_view
  //***************************************************************************
  template <typename T>
  typename tpn::enable_if<!tpn::is_same<T, tpn::istring>::value && !tpn::is_same<T, tpn::string_view>::value, const tpn::istring&>::type
    to_string(const T value, uint32_t denominator_exponent, tpn::istring& str, bool append = false)
  {
    tpn::format_spec format;

    return private_to_string::to_string(value, denominator_exponent, str, format, append);
  }

  //***************************************************************************
  /// Supplied format spec.
  /// !tpn::istring && !tpn::string_view
  //***************************************************************************
  template <typename T>
  typename tpn::enable_if<!tpn::is_same<T, tpn::istring>::value && !tpn::is_same<T, tpn::string_view>::value, const tpn::istring&>::type
    to_string(const T value, uint32_t denominator_exponent, tpn::istring& str, const tpn::format_spec& format, bool append = false)
  {
    return private_to_string::to_string(value, denominator_exponent, str, format, append);
  }

  //***************************************************************************
  /// Default format spec.
  /// tpn::istring
  //***************************************************************************
  template <typename T>
  typename tpn::enable_if<tpn::is_same<T, tpn::istring>::value, const tpn::istring&>::type
    to_string(const T& value, tpn::istring& str, bool append = false)
  {
    tpn::format_spec format;

    private_to_string::add_string(value, str, format, append);

    return str;
  }

  //***************************************************************************
  /// Supplied format spec.
  /// tpn::istring
  //***************************************************************************
  template <typename T>
  typename tpn::enable_if<tpn::is_same<T, tpn::istring>::value, const tpn::istring&>::type
    to_string(const tpn::istring& value, T& str, const tpn::format_spec& format, bool append = false)
  {
    private_to_string::add_string(value, str, format, append);

    return str;
  }

  //***************************************************************************
  /// Default format spec.
  /// tpn::string_view
  //***************************************************************************
  template <typename T>
  typename tpn::enable_if<tpn::is_same<T, tpn::string_view>::value, const tpn::istring&>::type
    to_string(T value, tpn::istring& str, bool append = false)
  {
    tpn::format_spec format;

    private_to_string::add_string_view(value, str, format, append);

    return str;
  }

  //***************************************************************************
  /// Supplied format spec.
  /// tpn::string_view
  //***************************************************************************
  template <typename T>
  typename tpn::enable_if<tpn::is_same<T, tpn::string_view>::value, const tpn::istring&>::type
    to_string(T value, tpn::istring& str, const tpn::format_spec& format, bool append = false)
  {
    private_to_string::add_string_view(value, str, format, append);

    return str;
  }
}

#endif
