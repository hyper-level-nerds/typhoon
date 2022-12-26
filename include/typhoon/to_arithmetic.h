///\file
/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tphn
https://www.tphncpp.com

Copyright(c) 2022 John Wellbelove

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

#ifndef TYPHOON_TO_ARITHMETIC_INCLUDED
#define TYPHOON_TO_ARITHMETIC_INCLUDED

#include "platform.h"
#include "type_traits.h"
#include "integral_limits.h"
#include "limits.h"
#include "string_view.h"
#include "basic_string.h"
#include "format_spec.h"
#include "radix.h"
#include "string_utilities.h"
#include "iterator.h"
#include "bit.h"
#include "smallest.h"
#include "absolute.h"
#include "expected.h"

namespace tphn
{
  //***************************************************************************
  /// Status values for to_arithmetic.
  //***************************************************************************
  struct to_arithmetic_status
  {
    enum enum_type
    {
      Valid,
      Invalid_Radix,
      Invalid_Format,
      Invalid_Float,
      Signed_To_Unsigned,
      Overflow
    };

    TYPHOON_DECLARE_ENUM_TYPE(to_arithmetic_status, int)
    TYPHOON_ENUM_TYPE(Valid,              "Valid")
    TYPHOON_ENUM_TYPE(Invalid_Radix,      "Invalid Radix")
    TYPHOON_ENUM_TYPE(Invalid_Format,     "Invalid Format")
    TYPHOON_ENUM_TYPE(Invalid_Float,      "Invalid Float")
    TYPHOON_ENUM_TYPE(Signed_To_Unsigned, "Signed To Unsigned")
    TYPHOON_ENUM_TYPE(Overflow,           "Overflow")
    TYPHOON_END_ENUM_TYPE
  };

  //***************************************************************************
  /// Status values for to_arithmetic.
  //***************************************************************************
  template<typename TValue>
  class to_arithmetic_result
  {
  public:

    typedef TValue                                     value_type;
    typedef tphn::to_arithmetic_status                  error_type;
    typedef tphn::unexpected<tphn::to_arithmetic_status> unexpected_type;

    //*******************************************
    /// Default constructor.
    //*******************************************
    TYPHOON_CONSTEXPR14
    to_arithmetic_result()
      : conversion_value(static_cast<value_type>(0))
      , conversion_status(error_type::Valid)
    {
    }

    //*******************************************
    /// Copy constructor.
    //*******************************************
    TYPHOON_CONSTEXPR14
    to_arithmetic_result(const to_arithmetic_result& other)
      : conversion_value(other.conversion_value)
      , conversion_status(other.conversion_status)
    {
    }

    //*******************************************
    /// Returns <b>true</b> if the result has a valid value.
    //*******************************************
    TYPHOON_NODISCARD
    TYPHOON_CONSTEXPR14
    bool has_value() const
    {
      return (conversion_status.error() == error_type::Valid);
    }

    //*******************************************
    /// Returns <b>true</b> if the result has a valid value.
    //*******************************************
    TYPHOON_NODISCARD
    TYPHOON_CONSTEXPR14
    operator bool() const
    {
      return has_value();
    }

    //*******************************************
    /// Returns the value, if valid.
    /// Otherwise undefined.
    //*******************************************
    TYPHOON_NODISCARD
    TYPHOON_CONSTEXPR14
    value_type value() const
    {
      return conversion_value;
    }

    //*******************************************
    /// Returns the value, if valid.
    /// Otherwise undefined.
    //*******************************************
    TYPHOON_NODISCARD
    TYPHOON_CONSTEXPR14
    operator value_type() const
    {
      return value();
    }

    //*******************************************
    /// Returns the conversion status.
    /// One of the following:-
    /// Valid
    /// Invalid_Radix
    /// Invalid_Format
    /// Invalid_Float
    /// Signed_To_Unsigned
    /// Overflow
    //*******************************************
    TYPHOON_NODISCARD
    TYPHOON_CONSTEXPR14
    error_type error() const
    {
      return tphn::to_arithmetic_status(conversion_status.error());
    }

    //*******************************************
    /// Assignment from a value.
    //*******************************************
    TYPHOON_CONSTEXPR14
    to_arithmetic_result& operator =(value_type value_)
    {
      conversion_value = value_;

      return *this;
    }

    //*******************************************
    /// Assignment from an unexpected_type.
    //*******************************************
    TYPHOON_CONSTEXPR14
    to_arithmetic_result& operator =(unexpected_type status_)
    {
      conversion_status = status_;

      return *this;
    }

  private:

    value_type      conversion_value;
    unexpected_type conversion_status;
  };

  namespace private_to_arithmetic
  {
    static TYPHOON_CONSTANT char Positive_Char     = '+';
    static TYPHOON_CONSTANT char Negative_Char     = '-';
    static TYPHOON_CONSTANT char Radix_Point1_Char = '.';
    static TYPHOON_CONSTANT char Radix_Point2_Char = ',';
    static TYPHOON_CONSTANT char Exponential_Char  = 'e';

    //*******************************************
    TYPHOON_NODISCARD
    inline
    TYPHOON_CONSTEXPR14
    bool is_valid(char c, tphn::radix::value_type radix)
    {
      switch (radix)
      {
        case tphn::radix::binary:
        {
          return (c >= '0') && (c <= '1');
          break;
        }

        case tphn::radix::octal:
        {
          return (c >= '0') && (c <= '7');
          break;
        }

        case tphn::radix::decimal:
        {
          return (c >= '0') && (c <= '9');
          break;
        }

        case tphn::radix::hex:
        {
          return ((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f'));
          break;
        }

        default:
        {
          return false;
          break;
        }
      }
    }

    //*******************************************
    TYPHOON_NODISCARD
    inline
    TYPHOON_CONSTEXPR14
    char digit_value(char c, tphn::radix::value_type radix)
    {
      switch (radix)
      {
        case tphn::radix::binary:
        case tphn::radix::octal:
        case tphn::radix::decimal:
        {
          return c - '0';
          break;
        }

        case tphn::radix::hex:
        {
          if ((c >= '0') && (c <= '9'))
          {
            return c - '0';
          }
          else
          {
            return (c - 'a') + 10;
          }
          break;
        }

        default:
        {
          return 0;
          break;
        }
      }
    }

    //*******************************************
    TYPHOON_NODISCARD
    inline 
    TYPHOON_CONSTEXPR14
    char to_lower(char c)
    {
      if ((c >= 'A') && (c <= 'Z'))
      {
        c += 32;
      }

      return c;
    }

    //*******************************************
    template <typename TChar>
    TYPHOON_NODISCARD
    TYPHOON_CONSTEXPR14
    char convert(TChar c)
    {
      return to_lower(static_cast<char>(c));
    }

    //***************************************************************************
    /// Checks to see if the text starts with a '+' or '-' prefix, and modifies the view to remove it.
    /// Returns true if the text has a '-' prefix.
    //***************************************************************************
    template <typename TChar>
    TYPHOON_NODISCARD
    TYPHOON_CONSTEXPR14
    bool check_and_remove_sign_prefix(tphn::basic_string_view<TChar>& view)
    {
      if (!view.empty())
      {
        // Check for prefix.
        const char c = convert(view[0]);
        const bool has_positive_prefix = (c == Positive_Char);
        const bool has_negative_prefix = (c == Negative_Char);

        // Step over the prefix, if present.
        if (has_positive_prefix || has_negative_prefix)
        {
          view.remove_prefix(1);
          return has_negative_prefix;
        }
      }

      return false;
    }
 
    //***************************************************************************
    /// Checks to see if the radix is valid.
    //***************************************************************************
    TYPHOON_NODISCARD
    inline
    TYPHOON_CONSTEXPR14
    bool is_valid_radix(const tphn::radix::value_type radix)
    {
      return (radix == tphn::radix::binary)  ||
             (radix == tphn::radix::octal)   ||
             (radix == tphn::radix::decimal) ||
             (radix == tphn::radix::hex);
    }

    //***************************************************************************
    /// Accumulate integrals
    //***************************************************************************
    template <typename TValue>
    struct integral_accumulator
    {
      //*********************************
      TYPHOON_CONSTEXPR14
      integral_accumulator(tphn::radix::value_type radix_, TValue maximum_)
        : radix(radix_)
        , maximum(maximum_)
        , integral_value(0)
        , conversion_status(to_arithmetic_status::Valid)
      {
      }

      //*********************************
      TYPHOON_NODISCARD
      TYPHOON_CONSTEXPR14
      bool add(const char c)
      {
        bool is_success      = false;
        bool is_not_overflow = false;

        const bool is_valid_char = is_valid(c, radix);

        if (is_valid_char)
        {
          TValue old_value = integral_value;
          integral_value *= radix;

          // No multipication overflow?
          is_not_overflow = ((integral_value / radix) == old_value);

          if (is_not_overflow)
          {
            const char digit = digit_value(c, radix);

            // No addition overflow?
            is_not_overflow = ((maximum - digit) >= integral_value);

            if ((maximum - digit) >= integral_value)
            {
              integral_value += digit;
              is_success = true;
            }
          }
        }

        // Check the status of the conversion.
        if (is_valid_char == false)
        {
          conversion_status = to_arithmetic_status::Invalid_Format;
        }
        else if (is_not_overflow == false)
        {
          conversion_status = to_arithmetic_status::Overflow;
        }

        return is_success;
      }

      //*********************************
      TYPHOON_NODISCARD
      TYPHOON_CONSTEXPR14
      bool has_value() const
      {
        return conversion_status == to_arithmetic_status::Valid;
      }

      //*********************************
      TYPHOON_NODISCARD
      TYPHOON_CONSTEXPR14
      TValue value() const
      {
        return integral_value;
      }

      //*********************************
      TYPHOON_NODISCARD
      TYPHOON_CONSTEXPR14
      to_arithmetic_status status() const
      {
        return conversion_status;
      }

    private:

      tphn::radix::value_type radix;
      TValue maximum;
      TValue integral_value;
      to_arithmetic_status conversion_status;
    };

    //***************************************************************************
    /// Accumulate floating point
    //***************************************************************************
    struct floating_point_accumulator
    {
      //*********************************
      TYPHOON_CONSTEXPR14
      floating_point_accumulator()
        : divisor(1)
        , floating_point_value(0)
        , is_negative_mantissa(false)
        , is_negative_exponent(false)
        , expecting_sign(true)
        , exponent_value(0)
        , state(Parsing_Integral)
        , conversion_status(to_arithmetic_status::Valid)
      {
      }

      //*********************************
      TYPHOON_NODISCARD
      TYPHOON_CONSTEXPR14
      bool add(char c)
      {
        bool is_success = true;

        switch (state)
        {
          //***************************
          case Parsing_Integral:
          {
            if (expecting_sign && ((c == Positive_Char) || (c == Negative_Char)))
            {
              is_negative_mantissa = (c == Negative_Char);
              expecting_sign = false;
            }
            // Radix point?
            else if ((c == Radix_Point1_Char) || (c == Radix_Point2_Char))
            {
              expecting_sign = false;
              state = Parsing_Fractional;
            }
            // Exponential?
            else if (c == Exponential_Char)
            {
              expecting_sign = true;
              state = Parsing_Exponential;
            }
            else if (is_valid(c, tphn::radix::decimal))
            {
              const char digit = digit_value(c, tphn::radix::decimal);
              floating_point_value *= 10;
              is_negative_mantissa ? floating_point_value -= digit : floating_point_value += digit;
              conversion_status = to_arithmetic_status::Valid;
              expecting_sign = false;
            }
            else
            {
              conversion_status = to_arithmetic_status::Invalid_Format;
              is_success = false;
            }
            break;
          }

          //***************************
          case Parsing_Fractional:
          {
            // Radix point?
            if ((c == Radix_Point1_Char) || (c == Radix_Point2_Char))
            {
              conversion_status = to_arithmetic_status::Invalid_Format;
              is_success = false;
            }
            // Exponential?
            else if (c == Exponential_Char)
            {
              expecting_sign = true;
              state = Parsing_Exponential;
            }
            else if (is_valid(c, tphn::radix::decimal))
            {
              const char digit = digit_value(c, tphn::radix::decimal);
              divisor *= 10;
              long double fraction = digit / divisor;
              is_negative_mantissa ? floating_point_value -= fraction : floating_point_value += fraction;
              conversion_status = to_arithmetic_status::Valid;
            }
            else
            {
              conversion_status = to_arithmetic_status::Invalid_Format;
              is_success = false;
            }
            break;
          }

          //***************************
          case Parsing_Exponential:
          {
            if (expecting_sign && ((c == Positive_Char) || (c == Negative_Char)))
            {
              is_negative_exponent = (c == Negative_Char);
              expecting_sign = false;
            }
            // Radix point?
            else if ((c == Radix_Point1_Char) || (c == Radix_Point2_Char) || (c == Exponential_Char))
            {
              conversion_status = to_arithmetic_status::Invalid_Format;
              is_success = false;
            }
            else if (is_valid(c, tphn::radix::decimal))
            {
              const char digit = digit_value(c, tphn::radix::decimal);
              exponent_value *= tphn::radix::decimal;
              is_negative_exponent ? exponent_value -= digit : exponent_value += digit;
            }
            else
            {
              conversion_status = to_arithmetic_status::Invalid_Format;
              is_success = false;
            }
            break;
          }

          //***************************
          default:
          {
            is_success = false;
            break;
          }
        }

        return is_success;
      }

      //*********************************
      TYPHOON_NODISCARD
      TYPHOON_CONSTEXPR14
      bool has_value() const
      {
        return (conversion_status == to_arithmetic_status::Valid);
      }

      //*********************************
      TYPHOON_NODISCARD
      TYPHOON_CONSTEXPR14
      long double value() const
      {
        return floating_point_value;
      }

      //*********************************
      TYPHOON_NODISCARD
      TYPHOON_CONSTEXPR14
      to_arithmetic_status status() const
      {
        return conversion_status;
      }

      //*********************************
      TYPHOON_NODISCARD
      TYPHOON_CONSTEXPR14
      int exponent() const
      {
        return exponent_value;
      }

    private:

      enum
      {
        Parsing_Integral,
        Parsing_Fractional,
        Parsing_Exponential
      };

      long double divisor;
      long double floating_point_value;
      bool is_negative_mantissa;
      bool is_negative_exponent;
      bool expecting_sign;
      int  exponent_value;
      int  state;
      to_arithmetic_status conversion_status;
    };

    //***************************************************************************
    // Define an unsigned accumulator type that is at least as large as TValue.
    //***************************************************************************
    template <size_t Bits>
    struct accumulator_type_select;

    template <>
    struct accumulator_type_select<8U>
    {
      typedef uint32_t type;
    };

    template <>
    struct accumulator_type_select<16U>
    {
      typedef uint32_t type;
    };

    template <>
    struct accumulator_type_select<32U>
    {
      typedef uint32_t type;
    };

#if TYPHOON_USING_64BIT_TYPES
    template <>
    struct accumulator_type_select<64U>
    {
      typedef uint64_t type;
    };
#endif

    //***************************************************************************
    /// Text to integral from view, radix value and maximum.
    //***************************************************************************
    template <typename TChar, typename TAccumulatorType>
    TYPHOON_NODISCARD
    TYPHOON_CONSTEXPR14
    tphn::to_arithmetic_result<TAccumulatorType> to_arithmetic_integral(const tphn::basic_string_view<TChar>& view,
                                                                       const tphn::radix::value_type         radix,
                                                                       const TAccumulatorType               maximum)
    {
      tphn::to_arithmetic_result<TAccumulatorType>   accumulator_result;
      typedef typename tphn::unexpected<tphn::to_arithmetic_status> unexpected_type;

      typename tphn::basic_string_view<TChar>::const_iterator       itr     = view.begin();
      const typename tphn::basic_string_view<TChar>::const_iterator itr_end = view.end();

      integral_accumulator<TAccumulatorType> accumulator(radix, maximum);

      while ((itr != itr_end) && accumulator.add(convert(*itr)))
      {
        // Keep looping until done or an error occurs.
        ++itr;
      }

      if (accumulator.has_value())
      {
        accumulator_result = accumulator.value();
      }
      else
      {
        accumulator_result = unexpected_type(accumulator.status());
      }

      return accumulator_result;
    }
  }

  //***************************************************************************
  /// Text to integral from view and radix value type.
  //***************************************************************************
  template <typename TValue, typename TChar>
  TYPHOON_NODISCARD
  TYPHOON_CONSTEXPR14
  typename tphn::enable_if<tphn::is_integral<TValue>::value, tphn::to_arithmetic_result<TValue> >::type
    to_arithmetic(tphn::basic_string_view<TChar> view,
                  const tphn::radix::value_type  radix)
  {
    using namespace tphn::private_to_arithmetic;

    typedef tphn::to_arithmetic_result<TValue>     result_type;
    typedef typename result_type::unexpected_type unexpected_type;

    result_type result;    

    if (is_valid_radix(radix))
    {
      // Is this a negative number?
      const bool is_negative = check_and_remove_sign_prefix(view);

      if (view.empty())
      {
        result = unexpected_type(to_arithmetic_status::Invalid_Format);
      }
      else
      {
        // Make sure we're not trying to put a negative value into an unsigned type.
        if (is_negative && tphn::is_unsigned<TValue>::value)
        {
          result = unexpected_type(to_arithmetic_status::Signed_To_Unsigned);
        }
        else
        {
          const bool is_decimal = (radix == tphn::radix::decimal);

          // Select the type we use for the accumulator.
          typedef typename accumulator_type_select<tphn::integral_limits<TValue>::bits>::type accumulator_type;

          // Find the maximum absolute value for the type value we're trying to convert to.
          const accumulator_type maximum = is_negative ? tphn::absolute_unsigned(tphn::integral_limits<TValue>::min)
                                                       : is_decimal ? tphn::integral_limits<TValue>::max
                                                                    : tphn::integral_limits<typename tphn::make_unsigned<TValue>::type>::max;
          // Do the conversion.
          tphn::to_arithmetic_result<accumulator_type> accumulator_result = to_arithmetic_integral<TChar>(view, radix, maximum);

          result = unexpected_type(accumulator_result.error());

          // Was it successful?
          if (accumulator_result.has_value())
          {
            typedef typename tphn::make_unsigned<TValue>::type uvalue_t;
            const uvalue_t uvalue = static_cast<uvalue_t>(accumulator_result.value());

            // Convert from the accumulator type to the desired type.
            result = (is_negative ? static_cast<TValue>(0) - uvalue : tphn::bit_cast<TValue>(uvalue));
          }
        }
      }
    }
    else
    {
      result = unexpected_type(to_arithmetic_status::Invalid_Radix);
    }

    return result;
  }

  //***************************************************************************
  /// Text to integral from view and default decimal radix.
  //***************************************************************************
  template <typename TValue, typename TChar>
  TYPHOON_NODISCARD
  TYPHOON_CONSTEXPR14
  typename tphn::enable_if<tphn::is_integral<TValue>::value, tphn::to_arithmetic_result<TValue> >::type
    to_arithmetic(const tphn::basic_string_view<TChar>& view)
  {
    return tphn::to_arithmetic<TValue, TChar>(view, tphn::radix::decimal);
  }

  //***************************************************************************
  /// Text to integral from view and radix format spec.
  //***************************************************************************
  template <typename TValue, typename TChar>
  TYPHOON_NODISCARD
  TYPHOON_CONSTEXPR14 
  typename tphn::enable_if<tphn::is_integral<TValue>::value, tphn::to_arithmetic_result<TValue> >::type
    to_arithmetic(const tphn::basic_string_view<TChar>& view, const typename tphn::private_basic_format_spec::base_spec& spec)
  {
    return tphn::to_arithmetic<TValue, TChar>(view, spec.base);
  }

  //***************************************************************************
  /// Text to integral from pointer, length and radix value type.
  //***************************************************************************
  template <typename TValue, typename TChar>
  TYPHOON_NODISCARD
  TYPHOON_CONSTEXPR14
  typename tphn::enable_if<tphn::is_integral<TValue>::value, tphn::to_arithmetic_result<TValue> >::type
    to_arithmetic(const TChar* cp, size_t length, const tphn::radix::value_type radix)
  {
    return tphn::to_arithmetic<TValue, TChar>(tphn::basic_string_view<TChar>(cp, length), radix);
  }

  //***************************************************************************
  /// Text to integral from pointer, length and default decimal radix.
  //***************************************************************************
  template <typename TValue, typename TChar>
  TYPHOON_NODISCARD
  TYPHOON_CONSTEXPR14 
  typename tphn::enable_if<tphn::is_integral<TValue>::value, tphn::to_arithmetic_result<TValue> >::type
    to_arithmetic(const TChar* cp, size_t length)
  {
    return tphn::to_arithmetic<TValue, TChar>(tphn::basic_string_view<TChar>(cp, length), tphn::radix::decimal);
  }

  //***************************************************************************
  /// Text to integral from pointer, length and radix format spec.
  //***************************************************************************
  template <typename TValue, typename TChar>
  TYPHOON_NODISCARD
  TYPHOON_CONSTEXPR14 
  typename tphn::enable_if<tphn::is_integral<TValue>::value, tphn::to_arithmetic_result<TValue> >::type
    to_arithmetic(const TChar* cp, size_t length, const typename tphn::private_basic_format_spec::base_spec& spec)
  {
    return tphn::to_arithmetic<TValue, TChar>(tphn::basic_string_view<TChar>(cp, length), spec.base);
  }

  //***************************************************************************
  /// Text to integral from string and radix value type.
  //***************************************************************************
  template <typename TValue, typename TChar>
  TYPHOON_NODISCARD
  TYPHOON_CONSTEXPR14 
  typename tphn::enable_if<tphn::is_integral<TValue>::value, tphn::to_arithmetic_result<TValue> >::type
    to_arithmetic(const tphn::ibasic_string<TChar>& str, const tphn::radix::value_type radix)
  {
    return tphn::to_arithmetic<TValue, TChar>(tphn::basic_string_view<TChar>(str), radix);;
  }

  //***************************************************************************
  /// Text to integral from string and default decimal radix.
  //***************************************************************************
  template <typename TValue, typename TChar>
  TYPHOON_NODISCARD
    TYPHOON_CONSTEXPR14
    typename tphn::enable_if<tphn::is_integral<TValue>::value, tphn::to_arithmetic_result<TValue> >::type
    to_arithmetic(const tphn::ibasic_string<TChar>& str)
  {
    return tphn::to_arithmetic<TValue, TChar>(tphn::basic_string_view<TChar>(str), tphn::radix::decimal);;
  }

  //***************************************************************************
  /// Text to integral from string and radix format spec.
  //***************************************************************************
  template <typename TValue, typename TChar>
  TYPHOON_NODISCARD
  TYPHOON_CONSTEXPR14 
  typename tphn::enable_if<tphn::is_integral<TValue>::value, tphn::to_arithmetic_result<TValue> >::type
    to_arithmetic(const tphn::ibasic_string<TChar>& str, const typename tphn::private_basic_format_spec::base_spec& spec)
  {
    return tphn::to_arithmetic<TValue, TChar>(tphn::basic_string_view<TChar>(str), spec);;
  }

  //***************************************************************************
  /// Floating point from view.
  //***************************************************************************
  template <typename TValue, typename TChar>
  TYPHOON_NODISCARD
  TYPHOON_CONSTEXPR14 
  typename tphn::enable_if<tphn::is_floating_point<TValue>::value, tphn::to_arithmetic_result<TValue> >::type
    to_arithmetic(tphn::basic_string_view<TChar> view)
  {
    using namespace tphn::private_to_arithmetic;

    typedef tphn::to_arithmetic_result<TValue>     result_type;
    typedef typename result_type::unexpected_type unexpected_type;

    result_type result;

    if (view.empty())
    {
      result = unexpected_type(to_arithmetic_status::Invalid_Format);
    }
    else
    {
      floating_point_accumulator accumulator;

      typename tphn::basic_string_view<TChar>::const_iterator itr           = view.begin();
      const typename tphn::basic_string_view<TChar>::const_iterator itr_end = view.end();

      while ((itr != itr_end) && accumulator.add(convert(*itr)))
      {
        // Keep looping until done or an error occurs.
        ++itr;
      }

      result = unexpected_type(accumulator.status());

      if (result.has_value())
      {
        TValue value = static_cast<TValue>(accumulator.value());
        int exponent = accumulator.exponent();

        value *= pow(static_cast<TValue>(10.0), static_cast<TValue>(exponent));

        // Check that the result is a valid floating point number.
        if ((value == tphn::numeric_limits<TValue>::infinity()) ||
            (value == -tphn::numeric_limits<TValue>::infinity()))
        {
          result = unexpected_type(to_arithmetic_status::Overflow);
        }
        // Check for NaN.
        else if (value != value)
        {
          result = unexpected_type(to_arithmetic_status::Invalid_Float);
        }
        else
        {
          result = value;
        }
      }
    }

    return result;
  }

  //***************************************************************************
  /// Floating point from pointer and length.
  //***************************************************************************
  template <typename TValue, typename TChar>
  TYPHOON_NODISCARD
  TYPHOON_CONSTEXPR14 
  typename tphn::enable_if<tphn::is_floating_point<TValue>::value, tphn::to_arithmetic_result<TValue> >::type
    to_arithmetic(const TChar* cp, size_t length)
  {
    return tphn::to_arithmetic<TValue, TChar>(tphn::basic_string_view<TChar>(cp, length));
  }

  //***************************************************************************
  /// Floating point from pointer.
  //***************************************************************************
  template <typename TValue, typename TChar>
  TYPHOON_NODISCARD
  TYPHOON_CONSTEXPR14
  typename tphn::enable_if<tphn::is_floating_point<TValue>::value, tphn::to_arithmetic_result<TValue> >::type
    to_arithmetic(const TChar* cp)
  {
    return tphn::to_arithmetic<TValue, TChar>(tphn::basic_string_view<TChar>(cp, tphn::strlen<TChar>(cp)));
  }

  //***************************************************************************
  /// Floating point from string.
  //***************************************************************************
  template <typename TValue, typename TChar>
  TYPHOON_NODISCARD
  TYPHOON_CONSTEXPR14 
  typename tphn::enable_if<tphn::is_floating_point<TValue>::value, tphn::to_arithmetic_result<TValue> >::type
    to_arithmetic(const tphn::ibasic_string<TChar>& str)
  {
    return tphn::to_arithmetic<TValue, TChar>(tphn::basic_string_view<TChar>(str));
  }
}

//***************************************************************************
/// Equality test for tphn::to_arithmetic_result
//***************************************************************************
template <typename T>
TYPHOON_CONSTEXPR14 bool operator ==(const tphn::to_arithmetic_result<T>& lhs, const tphn::to_arithmetic_result<T>& rhs)
{
  if (lhs.has_value() && rhs.has_value())
  {
    return (lhs.value() == rhs.value());
  }
  else
  {
    return (lhs.status() == rhs.status());
  }
}

//***************************************************************************
/// Equality test for tphn::to_arithmetic_result
//***************************************************************************
template <typename T, typename U>
TYPHOON_CONSTEXPR14 bool operator ==(const tphn::to_arithmetic_result<T>& lhs, const U& rhs)
{
  return bool(lhs) ? lhs.value() == rhs : false;
}

//***************************************************************************
/// Equality test for tphn::to_arithmetic_result
//***************************************************************************
template <typename T, typename U>
TYPHOON_CONSTEXPR14 bool operator ==(const T& lhs, const tphn::to_arithmetic_result<U>& rhs)
{
  return bool(rhs) ? rhs.value() == lhs : false;
}

//***************************************************************************
/// Inequality test for tphn::to_arithmetic_result
//***************************************************************************
template <typename T>
TYPHOON_CONSTEXPR14 bool operator !=(const tphn::to_arithmetic_result<T>& lhs, const tphn::to_arithmetic_result<T>& rhs)
{
  return !(lhs == rhs);
}

//***************************************************************************
/// Inequality test for tphn::to_arithmetic_result
//***************************************************************************
template <typename T, typename U>
TYPHOON_CONSTEXPR14 bool operator !=(const tphn::to_arithmetic_result<T>& lhs, const U& rhs)
{
  return !(lhs == rhs);
}

//***************************************************************************
/// Inequality test for tphn::to_arithmetic_result
//***************************************************************************
template <typename T, typename U>
TYPHOON_CONSTEXPR14 bool operator !=(const T& lhs, const tphn::to_arithmetic_result<T>& rhs)
{
  return !(lhs == rhs);
}

#endif
