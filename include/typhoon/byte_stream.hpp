///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tphn
https://www.tphncpp.com

Copyright(c) 2021 John Wellbelove

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

#ifndef TYPHOON_BYTE_STREAM_HPP
#define TYPHOON_BYTE_STREAM_HPP

#include "platform.hpp"
#include "type_traits.hpp"
#include "nullptr.hpp"
#include "endianness.hpp"
#include "integral_limits.hpp"
#include "algorithm.hpp"
#include "iterator.hpp"
#include "memory.hpp"
#include "span.hpp"
#include "iterator.hpp"
#include "optional.hpp"
#include "delegate.hpp"
#include "exception.hpp"
#include "error_handler.hpp"

#include <stdint.h>
#include <limits.h>

namespace tphn
{
  //***************************************************************************
  /// Exception base for byte streams
  //***************************************************************************
  class byte_stream_exception : public tphn::exception
  {
  public:

    byte_stream_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  ///\ingroup string
  /// String empty exception.
  //***************************************************************************
  class byte_stream_overflow : public tphn::byte_stream_exception
  {
  public:

    byte_stream_overflow(string_type file_name_, numeric_type line_number_)
      : byte_stream_exception(TYPHOON_ERROR_TEXT("byte_stream:overflow", TYPHOON_BYTE_STREAM_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Encodes a byte stream.
  //***************************************************************************
  class byte_stream_writer
  {
  public:

    typedef char* iterator;
    typedef const char* const_iterator;
    typedef tphn::span<char> callback_parameter_type;
    typedef tphn::delegate<void(callback_parameter_type)> callback_type;

    //***************************************************************************
    /// Construct from span.
    //***************************************************************************
    byte_stream_writer(tphn::span<char> span_, tphn::endian stream_endianness_, callback_type callback_ = callback_type())
      : pdata(span_.begin())
      , pcurrent(span_.begin())
      , length(span_.size_bytes())
      , stream_endianness(stream_endianness_)
      , callback(callback_)
    {
    }

    //***************************************************************************
    /// Construct from span.
    //***************************************************************************
    byte_stream_writer(tphn::span<unsigned char> span_, tphn::endian stream_endianness_, callback_type callback_ = callback_type())
      : pdata(reinterpret_cast<char*>(span_.begin()))
      , pcurrent(reinterpret_cast<char*>(span_.begin()))
      , length(span_.size_bytes())
      , stream_endianness(stream_endianness_)
      , callback(callback_)
    {
    }

    //***************************************************************************
    /// Construct from range.
    //***************************************************************************
    byte_stream_writer(void* begin_, void* end_, tphn::endian stream_endianness_, callback_type callback_ = callback_type())
      : pdata(reinterpret_cast<char*>(begin_))
      , pcurrent(reinterpret_cast<char*>(begin_))
      , length(tphn::distance(reinterpret_cast<char*>(begin_), reinterpret_cast<char*>(end_)))
      , stream_endianness(stream_endianness_)
      , callback(callback_)
    {
    }

    //***************************************************************************
    /// Construct from begin and length.
    //***************************************************************************
    byte_stream_writer(void* begin_, size_t length_, tphn::endian stream_endianness_, callback_type callback_ = callback_type())
      : pdata(reinterpret_cast<char*>(begin_))
      , pcurrent(reinterpret_cast<char*>(begin_))
      , length(length_)
      , stream_endianness(stream_endianness_)
      , callback(callback_)
    {
    }

    //***************************************************************************
    /// Construct from array.
    //***************************************************************************
    template <typename T, size_t Size>
    byte_stream_writer(T(&begin_)[Size], tphn::endian stream_endianness_, callback_type callback_ = callback_type())
      : pdata(begin_)
      , pcurrent(begin_)
      , length(begin_ + (Size * sizeof(T)))
      , stream_endianness(stream_endianness_)
      , callback(callback_)
    {
    }

    //***************************************************************************
    /// Writes a boolean to the stream
    //***************************************************************************
    void write_unchecked(bool value)
    {
      *pcurrent++ = value ? 1 : 0;
    }

    //***************************************************************************
    /// Writes a boolean to the stream
    //***************************************************************************
    bool write(bool value)
    {
      bool success = (available<bool>() > 0U);

      if (success)
      {
        write_unchecked(value);
      }
      else
      {
        TYPHOON_ASSERT_FAIL(TYPHOON_ERROR(tphn::byte_stream_overflow));
      }
 
      return success;
    }

    //***************************************************************************
    /// Write a value to the stream.
    //***************************************************************************
    template <typename T>
    typename tphn::enable_if<tphn::is_integral<T>::value || tphn::is_floating_point<T>::value, void>::type
      write_unchecked(T value)
    {
      to_bytes<T>(value);
    }

    //***************************************************************************
    /// Write a value to the stream.
    //***************************************************************************
    template <typename T>
    typename tphn::enable_if<tphn::is_integral<T>::value || tphn::is_floating_point<T>::value, bool>::type
      write(T value)
    {
      bool success = (available<T>() > 0U);

      if (success)
      {
        write_unchecked(value);        
      }
      else
      {
        TYPHOON_ASSERT_FAIL(TYPHOON_ERROR(tphn::byte_stream_overflow));
      }

      return success;
    }

    //***************************************************************************
    /// Write a range of T to the stream.
    //***************************************************************************
    template <typename T>
    typename tphn::enable_if<tphn::is_integral<T>::value || tphn::is_floating_point<T>::value, void>::type
      write_unchecked(const tphn::span<T>& range)
    {
      typename tphn::span<T>::iterator itr = range.begin();

      while (itr != range.end())
      {
        to_bytes(*itr);
        ++itr;
      }
    }

    //***************************************************************************
    /// Write a range of T to the stream.
    //***************************************************************************
    template <typename T>
    typename tphn::enable_if<tphn::is_integral<T>::value || tphn::is_floating_point<T>::value, bool>::type
      write(const tphn::span<T>& range)
    {
      bool success = (available<T>() >= range.size());

      if (success)
      {
        write_unchecked(range);
      }
      else
      {
        TYPHOON_ASSERT_FAIL(TYPHOON_ERROR(tphn::byte_stream_overflow));
      }

      return success;
    }

    //***************************************************************************
    /// Write a range of T to the stream.
    //***************************************************************************
    template <typename T>
    typename tphn::enable_if<tphn::is_integral<T>::value || tphn::is_floating_point<T>::value, void>::type
      write_unchecked(const T* start, size_t length)
    {
      while (length-- != 0U)
      {
        to_bytes(*start);
        ++start;
      }
    }

    //***************************************************************************
    /// Write a range of T to the stream.
    //***************************************************************************
    template <typename T>
    typename tphn::enable_if<tphn::is_integral<T>::value || tphn::is_floating_point<T>::value, bool>::type
      write(const T* start, size_t length)
    {
      bool success = (available<T>() >= length);

      if (success)
      {
        write_unchecked(start, length);
      }
      else
      {
        TYPHOON_ASSERT_FAIL(TYPHOON_ERROR(tphn::byte_stream_overflow));
      }

      return success;
    }

    //***************************************************************************
    /// Skip n items of T, if the total space is available.
    /// Returns <b>true</b> if the skip was possible.
    /// Returns <b>false</b> if the skip size was not possible.
    //***************************************************************************
    template <typename T>
    bool skip(size_t n)
    {
      bool success = (available<T>() >= n);

      if (success)
      {
        step(n * sizeof(T));
      }
      else
      {
        TYPHOON_ASSERT_FAIL(TYPHOON_ERROR(tphn::byte_stream_overflow));
      }

      return success;
    }

    //***************************************************************************
    /// Sets the index back to the position in the stream. Default = 0.
    //***************************************************************************
    void restart(size_t n = 0U)
    {
      pcurrent = const_cast<char*>(pdata + n);
    }

    //***************************************************************************
    /// Returns start of the stream.
    //***************************************************************************
    iterator begin()
    {
      return pdata;
    }

    //***************************************************************************
    /// Returns start of the stream.
    //***************************************************************************
    const_iterator begin() const
    {
      return pdata;
    }

    //***************************************************************************
    /// Returns end of the stream.
    //***************************************************************************
    iterator end()
    {
      return pcurrent;
    }

    //***************************************************************************
    /// Returns end of the stream.
    //***************************************************************************
    const_iterator end() const
    {
      return pcurrent;
    }

    //***************************************************************************
    /// Returns start of the stream.
    //***************************************************************************
    const_iterator cbegin() const
    {
      return pdata;
    }

    //***************************************************************************
    /// Returns end of the stream.
    //***************************************************************************
    const_iterator cend() const
    {
      return pcurrent;
    }

    //***************************************************************************
    /// Returns a span of the used portion of the stream.
    //***************************************************************************
    tphn::span<char> used_data()
    {
      return tphn::span<char>(pdata, pcurrent);
    }

    //***************************************************************************
    /// Returns a span of the used portion of the stream.
    //***************************************************************************
    tphn::span<const char> used_data() const
    {
      return tphn::span<const char>(pdata, pcurrent);
    }

    //***************************************************************************
    /// Returns a span of the free portion of the stream.
    //***************************************************************************
    tphn::span<char> free_data()
    {
      return tphn::span<char>(pcurrent, pdata + length);
    }

    //***************************************************************************
    /// Returns a span of the free portion of the stream.
    //***************************************************************************
    tphn::span<const char> free_data() const
    {
      return tphn::span<const char>(pcurrent, pdata + length);
    }

    //***************************************************************************
    /// Returns a span of whole the stream.
    //***************************************************************************
    tphn::span<char> data()
    {
      return tphn::span<char>(pdata, pdata + length);
    }

    //***************************************************************************
    /// Returns a span of whole the stream.
    //***************************************************************************
    tphn::span<const char> data() const
    {
      return tphn::span<const char>(pdata, pdata + length);
    }

    //***************************************************************************
    /// Returns <b>true</b> if the byte stream index has reached the end.
    //***************************************************************************
    bool full() const
    {
      return size_bytes() == capacity();
    }

    //***************************************************************************
    /// Returns <b>true</b> if the byte stream is empty.
    //***************************************************************************
    bool empty() const
    {
      return size_bytes() == 0U;
    }

    //***************************************************************************
    /// Returns the number of bytes used in the stream.
    //***************************************************************************
    size_t size_bytes() const
    {
      return tphn::distance(pdata, pcurrent);
    }

    //***************************************************************************
    /// Returns the maximum number of bytes in the stream.
    //***************************************************************************
    size_t capacity() const
    {
      return length;
    }

    //***************************************************************************
    /// The number of T left in the stream.
    //***************************************************************************
    template <typename T>
    size_t available() const
    {
      return (capacity() - size_bytes()) / sizeof(T);
    }

    //***************************************************************************
    /// The number of bytes left in the stream.
    //***************************************************************************
    size_t available_bytes() const
    {
      return available<char>();
    }

    //***************************************************************************
    /// Sets the function to call afer every write.
    //***************************************************************************
    void set_callback(callback_type callback_)
    {
      callback = callback_;
    }

    //***************************************************************************
    /// Gets the function to call afer every write.
    //***************************************************************************
    callback_type get_callback() const
    {
      return callback;
    }

    //***************************************************************************
    /// Gets the endianness of the stream.
    //***************************************************************************
    tphn::endian get_endianness() const
    {
      return stream_endianness;
    }

  private:

    //***************************************************************************
    /// to_bytes
    //***************************************************************************
    template <typename T>
    typename tphn::enable_if<sizeof(T) == 1U, void>::type
      to_bytes(const T value)
    {
      *pcurrent = static_cast<char>(value);
      step(1U);
    }

    //*********************************
    template <typename T>
    typename tphn::enable_if<sizeof(T) != 1U, void>::type
      to_bytes(const T value)
    {
      const char* pv = reinterpret_cast<const char*>(&value);
      copy_value(pv, pcurrent, sizeof(T));
      step(sizeof(T));
    }

    //*********************************
    void step(size_t n)
    {
      callback.call_if(tphn::span<char>(pcurrent, pcurrent + n));

      pcurrent += n;
    }

    //*********************************
    void copy_value(const char* source, char* destination, size_t length) const
    {
      const tphn::endian platform_endianness = tphn::endianness::value();

      if (stream_endianness == platform_endianness)
      {
        tphn::copy(source, source + length, destination);
      }
      else
      {
        tphn::reverse_copy(source, source + length, destination);
      }
    }

    char* const       pdata;             ///< The start of the byte stream buffer.
    char*             pcurrent;          ///< The current position in the byte stream buffer.
    const size_t      length;            ///< The length of the byte stream buffer.
    const tphn::endian stream_endianness; ///< The endianness of the stream data.
    callback_type     callback;          ///< An optional callback on every step on the write buffer.
  };

  //***************************************************************************
  /// Decodes byte streams.
  /// Data must be stored in the stream in network order.
  //***************************************************************************
  class byte_stream_reader
  {
  public:

    typedef char* iterator;
    typedef const char* const_iterator;

    //***************************************************************************
    /// Construct from span.
    //***************************************************************************
    byte_stream_reader(tphn::span<char> span_, tphn::endian stream_endianness_)
      : pdata(span_.begin())
      , pcurrent(span_.begin())
      , length(span_.size_bytes())
      , stream_endianness(stream_endianness_)
    {
    }

    //***************************************************************************
    /// Construct from span.
    //***************************************************************************
    byte_stream_reader(tphn::span<const char> span_, tphn::endian stream_endianness_)
      : pdata(span_.begin())
      , pcurrent(span_.begin())
      , length(span_.size_bytes())
      , stream_endianness(stream_endianness_)
    {
    }

    //***************************************************************************
    /// Construct from range.
    //***************************************************************************
    byte_stream_reader(const void* begin_, const void* end_, tphn::endian stream_endianness_)
      : pdata(reinterpret_cast<const char*>(begin_))
      , pcurrent(reinterpret_cast<const char*>(begin_))
      , length(tphn::distance(reinterpret_cast<const char*>(begin_), reinterpret_cast<const char*>(end_)))
      , stream_endianness(stream_endianness_)
    {
    }

    //***************************************************************************
    /// Construct from begin and length.
    //***************************************************************************
    byte_stream_reader(const void* begin_, size_t length_, tphn::endian stream_endianness_)
      : pdata(reinterpret_cast<const char*>(begin_))
      , pcurrent(reinterpret_cast<const char*>(begin_))
      , length(length_)
      , stream_endianness(stream_endianness_)
    {
    }

    //***************************************************************************
    /// Construct from array.
    //***************************************************************************
    template <typename T, size_t Size>
    byte_stream_reader(T(&begin_)[Size], tphn::endian stream_endianness_)
      : pdata(begin_)
      , pcurrent(begin_)
      , length(begin_ + (Size * sizeof(T)))
      , stream_endianness(stream_endianness_)
    {
    }

    //***************************************************************************
    /// Construct from const array.
    //***************************************************************************
    template <typename T, size_t Size>
    byte_stream_reader(const T(&begin_)[Size], tphn::endian stream_endianness_)
      : pdata(begin_)
      , pcurrent(begin_)
      , length(begin_ + (Size * sizeof(T)))
      , stream_endianness(stream_endianness_)
    {
    }

    //***************************************************************************
    /// Read a value from the stream.
    //***************************************************************************
    template <typename T>
    typename tphn::enable_if<tphn::is_integral<T>::value || tphn::is_floating_point<T>::value, T>::type
      read_unchecked()
    {
      return from_bytes<T>();
    }

    //***************************************************************************
    /// Read a value from the stream.
    //***************************************************************************
    template <typename T>
    typename tphn::enable_if<tphn::is_integral<T>::value || tphn::is_floating_point<T>::value, tphn::optional<T> >::type
      read()
    {
      tphn::optional<T> result;

      // Do we have enough room?
      if (available<T>() > 0U)
      {
        result = read_unchecked<T>();
      }

      return result;
    }

    //***************************************************************************
    /// Read a byte range from the stream.
    //***************************************************************************
    template <typename T>
    typename tphn::enable_if<sizeof(T) == 1U, tphn::span<const T> >::type
      read_unchecked(size_t n)
    {
      tphn::span<const T> result;

      const char* pend = pcurrent + (n * sizeof(T));

      result = tphn::span<const T>(reinterpret_cast<const T*>(pcurrent), reinterpret_cast<const T*>(pend));
      pcurrent = pend;

      return result;
    }

    //***************************************************************************
    /// Read a byte range from the stream.
    //***************************************************************************
    template <typename T>
    typename tphn::enable_if<sizeof(T) == 1U, tphn::optional<tphn::span<const T> > >::type
      read(size_t n)
    {
      tphn::optional<tphn::span<const T> > result;

      // Do we have enough room?
      if (available<T>() >= n)
      {
        result = read_unchecked<T>(n);
      }

      return result;
    }

    //***************************************************************************
    /// Read a range of T from the stream.
    //***************************************************************************
    template <typename T>
    typename tphn::enable_if<tphn::is_integral<T>::value || tphn::is_floating_point<T>::value, tphn::span<const T> >::type
      read_unchecked(tphn::span<T> range)
    {
      typename tphn::span<T>::iterator destination = range.begin();

      while (destination != range.end())
      {
        *destination++ = from_bytes<T>();
      }

      return tphn::span<const T>(range.begin(), range.end());
    }

    //***************************************************************************
    /// Read a range of T from the stream.
    //***************************************************************************
    template <typename T>
    typename tphn::enable_if<tphn::is_integral<T>::value || tphn::is_floating_point<T>::value, tphn::optional<tphn::span<const T> > >::type
      read(tphn::span<T> range)
    {
      // Do we have enough room?
      if (available<T>() >= range.size())
      {
        return tphn::optional<tphn::span<const T> >(read_unchecked<T>(range));
      }

      return tphn::optional<tphn::span<const T> >();
    }

    //***************************************************************************
    /// Read a range of T from the stream.
    //***************************************************************************
    template <typename T>
    typename tphn::enable_if<tphn::is_integral<T>::value || tphn::is_floating_point<T>::value, tphn::span<const T> >::type
      read_unchecked(T* start,  size_t length)
    {
      T* destination = start;

      for (size_t i = 0; i < length; ++i)
      {
        *destination++ = from_bytes<T>();
      }

      return tphn::span<const T>(start, length);
    }

    //***************************************************************************
    /// Read a range of T from the stream.
    //***************************************************************************
    template <typename T>
    typename tphn::enable_if<tphn::is_integral<T>::value || tphn::is_floating_point<T>::value, tphn::optional<tphn::span<const T> > >::type
      read(T* start, size_t length)
    {
      // Do we have enough room?
      if (available<T>() >= length)
      {
        return tphn::optional<tphn::span<const T> >(read_unchecked<T>(start, length));
      }

      return tphn::optional<tphn::span<const T> >();
    }

    //***************************************************************************
    /// Skip n items of T, up to the maximum space available.
    /// Returns <b>true</b> if the skip was possible.
    /// Returns <b>false</b> if the full skip size was not possible.
    //***************************************************************************
    template <typename T>
    bool skip(size_t n)
    {
      if (n < available<T>())
      {
        pcurrent += (n * sizeof(T));
        return true;
      }
      else
      {
        TYPHOON_ASSERT_FAIL(TYPHOON_ERROR(tphn::byte_stream_overflow));
        return false;
      }
    }

    //***************************************************************************
    /// Sets the index back to the position in the stream. Default = 0.
    //***************************************************************************
    void restart(size_t n = 0U)
    {
      pcurrent = pdata + n;
    }

    //***************************************************************************
    /// Returns start of the stream.
    //***************************************************************************
    const_iterator begin() const
    {
      return pdata;
    }

    //***************************************************************************
    /// Returns end of the stream.
    //***************************************************************************
    const_iterator end() const
    {
      return pcurrent;
    }

    //***************************************************************************
    /// Returns start of the stream.
    //***************************************************************************
    const_iterator cbegin() const
    {
      return pdata;
    }

    //***************************************************************************
    /// Returns end of the stream.
    //***************************************************************************
    const_iterator cend() const
    {
      return pcurrent;
    }

    //***************************************************************************
    /// Returns a span of the used portion of the stream.
    //***************************************************************************
    tphn::span<const char> used_data() const
    {
      return tphn::span<const char>(pdata, pcurrent);
    }

    //***************************************************************************
    /// Returns a span of the free portion of the stream.
    //***************************************************************************
    tphn::span<const char> free_data() const
    {
      return tphn::span<const char>(pcurrent, pdata + length);
    }

    //***************************************************************************
    /// Returns a span of whole the stream.
    //***************************************************************************
    tphn::span<const char> data() const
    {
      return tphn::span<const char>(pdata, pdata + length);
    }

    //***************************************************************************
    /// Returns <b>true</b> if the byte stream is empty.
    //***************************************************************************
    bool empty() const
    {
      return available<char>() == 0U;
    }

    //***************************************************************************
    /// Returns the number of bytes used in the stream.
    //***************************************************************************
    size_t size_bytes() const
    {
      return length;
    }

    //***************************************************************************
    /// The number of T left in the stream.
    //***************************************************************************
    template <typename T>
    size_t available() const
    {
      size_t used = tphn::distance(pdata, pcurrent);

      return (length - used) / sizeof(T);
    }

    //***************************************************************************
    /// The number of bytes left in the stream.
    //***************************************************************************
    size_t available_bytes() const
    {
      return available<char>();
    }

  private:

    //***************************************************************************
    /// from_bytes
    //***************************************************************************
    template <typename T>
    typename tphn::enable_if<sizeof(T) == 1U, T>::type
      from_bytes()
    {
      return static_cast<T>(*pcurrent++);
    }

    //*********************************
    template <typename T>
    typename tphn::enable_if<sizeof(T) != 1U, T>::type
      from_bytes()
    {
      T value;

      char* pv = reinterpret_cast<char*>(&value);
      copy_value(pcurrent, pv, sizeof(T));
      pcurrent += sizeof(T);

      return value;
    }

    //*********************************
    void copy_value(const char* source, char* destination, size_t length) const
    {
      const tphn::endian platform_endianness = tphn::endianness::value();

      if (stream_endianness == platform_endianness)
      {
        tphn::copy(source, source + length, destination);
      }
      else
      {
        tphn::reverse_copy(source, source + length, destination);
      }
    }

    const char* const pdata;             ///< The start of the byte stream buffer.
    const char*       pcurrent;          ///< The current position in the byte stream buffer.
    const size_t      length;            ///< The length of the byte stream buffer.
    const tphn::endian stream_endianness; ///< The endianness of the stream data.
  };

  //***************************************************************************
  /// Default implementation of the write function.
  /// For integral and floating point types only.
  /// Overload this to support custom types.
  //***************************************************************************
  template <typename T>
  void write_unchecked(tphn::byte_stream_writer& stream, const T& value)
  {
    stream.write_unchecked(value);
  }

  //***************************************************************************
  /// Implementation of the write function.
  //***************************************************************************
  template <typename T>
  bool write(tphn::byte_stream_writer& stream, const T& value)
  {
    return stream.write(value);
  }

  //***************************************************************************
  /// Default implementation of the read function.
  /// For integral and floating point types only.
  /// Overload this to support custom types.
  //***************************************************************************
  template <typename T>
  T  read_unchecked(tphn::byte_stream_reader& stream)
  {
    return stream.read_unchecked<T>();
  }

  //***************************************************************************
  /// Implementation of the read function.
  //***************************************************************************
  template <typename T>
  tphn::optional<T> read(tphn::byte_stream_reader& stream)
  {
    return stream.read<T>();
  }
}

#endif
