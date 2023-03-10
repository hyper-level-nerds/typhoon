///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tpn
https://www.tpncpp.com

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

#ifndef TYPHOON_MEM_CAST_HPP
#define TYPHOON_MEM_CAST_HPP

#include "platform.hpp"
#include "memory.hpp"
#include "static_assert.hpp"
#include "largest.hpp"
#include "utility.hpp"
#include "placement_new.hpp"
#include "exception.hpp"
#include "error_handler.hpp"
#include "file_error_numbers.hpp"
#include "binary.hpp"

#include <stdint.h>
#include <string.h>

namespace tpn
{
  //***************************************************************************
  /// The base class for array_wrapper exceptions.
  //***************************************************************************
  class mem_cast_exception : public tpn::exception
  {
  public:

    mem_cast_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// The exception thrown when the type size is too large.
  //***************************************************************************
  class mem_cast_size_exception : public tpn::mem_cast_exception
  {
  public:

    mem_cast_size_exception(string_type file_name_, numeric_type line_number_)
      : mem_cast_exception(TYPHOON_ERROR_TEXT("mem_cast:size", TYPHOON_MEM_CAST_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// The exception thrown when the pointer is null.
  //***************************************************************************
  class mem_cast_nullptr_exception : public tpn::mem_cast_exception
  {
  public:

    mem_cast_nullptr_exception(string_type file_name_, numeric_type line_number_)
      : mem_cast_exception(TYPHOON_ERROR_TEXT("mem_cast:null pointer", TYPHOON_MEM_CAST_FILE_ID"B"), file_name_, line_number_)
    {
    }
  };

  //*****************************************************************************
  /// mem_cast
  //*****************************************************************************
  template <size_t Size_, size_t Alignment_>
  class mem_cast
  {
  public:

    static TYPHOON_CONSTANT size_t Size      = Size_;
    static TYPHOON_CONSTANT size_t Alignment = Alignment_;

    TYPHOON_STATIC_ASSERT((Alignment == 1) || tpn::is_power_of_2<Alignment>::value, "Alignment must be a power of 2");

    //***********************************
    /// Default constructor
    //***********************************
    mem_cast()
      : buffer()
    {
    }

    //***********************************
    /// Copy constructor
    //***********************************
    template <size_t Other_Size, size_t Other_Alignment>
    mem_cast(const mem_cast<Other_Size, Other_Alignment>& other)
    {
      TYPHOON_STATIC_ASSERT(Size >= Other_Size, "Other size is too large");

      memcpy(buffer, other.buffer, Size_);
    }
   
    //***********************************
    /// Assignment operator
    //***********************************
    template <size_t Other_Size, size_t Other_Alignment>
    mem_cast& operator =(const mem_cast<Other_Size, Other_Alignment>& rhs)
    {
      TYPHOON_STATIC_ASSERT(Size >= Other_Size, "RHS size is too large");

      memcpy(buffer, rhs.buffer, Size_);

      return *this;
    }

    //***********************************
    /// Assign from value
    //***********************************
    template <typename T>
    void assign(const T& value)
    {
      TYPHOON_STATIC_ASSERT(Size >= sizeof(T), "Type size is too large");

      ::new (static_cast<void*>(buffer)) T(value);
    }

    //***********************************
    /// Assign from value at offset
    //***********************************
    template <typename T>
    void assign_at_offset(size_t offset, const T& value)
    {
      char* p = static_cast<char*>(buffer) + offset;
      TYPHOON_ASSERT(sizeof(T) <= (Size - offset), TYPHOON_ERROR(tpn::mem_cast_size_exception));

      ::new (p) T(value);
    }

    //***********************************
    /// Assign from value at offset
    //***********************************
    template <typename T, size_t Offset>
    void assign_at_offset(const T& value)
    {
      char* p = static_cast<char*>(buffer) + Offset;
      TYPHOON_STATIC_ASSERT(sizeof(T) <= (Size - Offset), "Type size is too large");

      ::new (p) T(value);
    }

#if TYPHOON_USING_CPP11
    //***********************************
    /// Emplace from parameters
    //***********************************
    template <typename T, typename... TArgs>
    void emplace(TArgs... args)
    {
      TYPHOON_STATIC_ASSERT(Size >= sizeof(T), "Type size is too large");

      ::new (static_cast<void*>(buffer)) T(tpn::forward<TArgs>(args)...);
    }

    //***********************************
    /// Emplace from parameters at offset
    //***********************************
    template <typename T, typename... TArgs>
    void emplace_at_offset(size_t offset, TArgs... args)
    {
      char* p = static_cast<char*>(buffer) + offset;
      TYPHOON_ASSERT(sizeof(T) <= (Size - offset), TYPHOON_ERROR(tpn::mem_cast_size_exception));

      ::new (p) T(tpn::forward<TArgs>(args)...);
    }

    //***********************************
    /// Emplace from parameters at offset
    //***********************************
    template <typename T, size_t Offset, typename... TArgs>
    void emplace_at_offset(TArgs... args)
    {
      char* p = static_cast<char*>(buffer) + Offset;
      TYPHOON_STATIC_ASSERT(sizeof(T) <= (Size - Offset), "Type size is too large");

      ::new (p) T(tpn::forward<TArgs>(args)...);
    }
#endif

    //***********************************
    /// Get a reference to T
    //***********************************
    template <typename T>
    TYPHOON_NODISCARD T& ref()
    {
      TYPHOON_STATIC_ASSERT(sizeof(T) <= Size, "Size of T is too large");

      return *static_cast<T*>(buffer);
    }

    //***********************************
    /// Get a const reference to T
    //***********************************
    template <typename T>
    TYPHOON_NODISCARD const T& ref() const
    {
      TYPHOON_STATIC_ASSERT(sizeof(T) <= Size, "Size of T is too large");

      return *static_cast<const T*>(buffer);
    }

    //***********************************
    /// Get a reference to T at offset (dynamic)
    //***********************************
    template <typename T>
    TYPHOON_NODISCARD T& ref_at_offset(size_t offset)
    {
      TYPHOON_ASSERT(sizeof(T) <= (Size - offset), TYPHOON_ERROR(tpn::mem_cast_size_exception));

      char* p = buffer + offset;

      return *static_cast<T*>(p);
    }

    //***********************************
    /// Get a const reference to T at offset (dynamic)
    //***********************************
    template <typename T>
    TYPHOON_NODISCARD const T& ref_at_offset(size_t offset) const
    {
      TYPHOON_ASSERT(sizeof(T) <= (Size - offset), TYPHOON_ERROR(tpn::mem_cast_size_exception));

      char* p = buffer + offset;

      return *static_cast<const T*>(p);
    }

    //***********************************
    /// Get a reference to T at offset (static)
    //***********************************
    template <typename T, size_t Offset>
    TYPHOON_NODISCARD T& ref_at_offset()
    {
      TYPHOON_STATIC_ASSERT(sizeof(T) <= (Size - Offset), "Size of T is too large");

      char* p = buffer + Offset;

      return *static_cast<T*>(p);
    }

    //***********************************
    /// Get a const reference to T at offset (static)
    //***********************************
    template <typename T, size_t Offset>
    TYPHOON_NODISCARD const T& ref_at_offset() const
    {
      TYPHOON_STATIC_ASSERT(sizeof(T) <= (Size - Offset), "Size of T is too large");

      char* p = buffer + Offset;

      return *static_cast<const T*>(p);
    }

    //***********************************
    /// Get the size of the buffer
    //***********************************
    TYPHOON_NODISCARD static TYPHOON_CONSTEXPR size_t size()
    {
      return Size;
    }

    //***********************************
    /// Get the alignment of the buffer
    //***********************************
    TYPHOON_NODISCARD static TYPHOON_CONSTEXPR size_t alignment()
    {
      return Alignment;
    }

    //***********************************
    /// Get a pointer to the internal buffer
    //***********************************
    TYPHOON_NODISCARD char* data()
    {
      return buffer;
    }

    //***********************************
    /// Get a const pointer to the internal buffer
    //***********************************
    TYPHOON_NODISCARD const char* data() const 
    {
      return buffer;
    }

  private:

    /// The internal buffer
    tpn::uninitialized_buffer<Size, 1U, Alignment> buffer;
  };

  //*****************************************************************************
  /// mem_cast_ptr
  //*****************************************************************************
  class mem_cast_ptr
  {
  public:

    static TYPHOON_CONSTANT size_t Undefined_Size = tpn::integral_limits<size_t>::max;

    //***********************************
    /// Default constructor
    //***********************************
    mem_cast_ptr()
      : pbuffer(TYPHOON_NULLPTR)
      , buffer_size(Undefined_Size)
    {
    }

    //***********************************
    /// Construct with pointer to buffer and optional size
    //***********************************
    mem_cast_ptr(char* pbuffer_, size_t buffer_size_ = Undefined_Size)
      : pbuffer(pbuffer_)
      , buffer_size(buffer_size_)
    {
    }

    //***********************************
    /// Copy construct
    //***********************************
    mem_cast_ptr(const mem_cast_ptr& other)
      : pbuffer(other.pbuffer)
      , buffer_size(other.buffer_size)
    {
    }

    //***********************************
    /// Assignment operator
    //***********************************
    mem_cast_ptr& operator =(const mem_cast_ptr& rhs)
    {
      pbuffer     = rhs.pbuffer;
      buffer_size = rhs.buffer_size;

      return *this;
    }

    //***********************************
    /// Assign from value
    //***********************************
    template <typename T>
    void assign(const T& value)
    {
      TYPHOON_ASSERT((pbuffer != TYPHOON_NULLPTR), TYPHOON_ERROR(tpn::mem_cast_nullptr_exception));
      TYPHOON_ASSERT(sizeof(T) <= buffer_size, TYPHOON_ERROR(tpn::mem_cast_size_exception));

      ::new (pbuffer) T(value);
    }

    //***********************************
    /// Assign from value at offset
    //***********************************
    template <typename T>
    void assign_at_offset(size_t offset, const T& value)
    {
      TYPHOON_ASSERT((pbuffer != TYPHOON_NULLPTR), TYPHOON_ERROR(tpn::mem_cast_nullptr_exception));
      char* p = pbuffer + offset;
      TYPHOON_ASSERT(sizeof(T) <= (buffer_size - offset), TYPHOON_ERROR(tpn::mem_cast_size_exception));

      ::new (p) T(value);
    }

    //***********************************
    /// Assign from value at offset
    //***********************************
    template <typename T, size_t Offset>
    void assign_at_offset(const T& value)
    {
      TYPHOON_ASSERT((pbuffer != TYPHOON_NULLPTR), TYPHOON_ERROR(tpn::mem_cast_nullptr_exception));
      char* p = pbuffer + Offset;
      TYPHOON_ASSERT(sizeof(T) <= (buffer_size - Offset), TYPHOON_ERROR(tpn::mem_cast_size_exception));

      ::new (p) T(value);
    }

#if TYPHOON_USING_CPP11
    //***********************************
    /// Emplace from parameters
    //***********************************
    template <typename T, typename... TArgs>
    void emplace(TArgs... args)
    {
      TYPHOON_ASSERT((pbuffer != TYPHOON_NULLPTR), TYPHOON_ERROR(tpn::mem_cast_nullptr_exception));
      TYPHOON_ASSERT(sizeof(T) <= buffer_size, TYPHOON_ERROR(tpn::mem_cast_size_exception));

      ::new (pbuffer) T(tpn::forward<TArgs>(args)...);
    }

    //***********************************
    /// Emplace from parameters at offset
    //***********************************
    template <typename T, typename... TArgs>
    void emplace_at_offset(size_t offset, TArgs... args)
    {
      TYPHOON_ASSERT((pbuffer != TYPHOON_NULLPTR), TYPHOON_ERROR(tpn::mem_cast_nullptr_exception));
      char* p = pbuffer + offset;
      TYPHOON_ASSERT(sizeof(T) <= (buffer_size - offset), TYPHOON_ERROR(tpn::mem_cast_size_exception));

      ::new (p) T(tpn::forward<TArgs>(args)...);
    }

    //***********************************
    /// Emplace from parameters at offset
    //***********************************
    template <typename T, size_t Offset, typename... TArgs>
    void emplace_at_offset(TArgs... args)
    {
      TYPHOON_ASSERT((pbuffer != TYPHOON_NULLPTR), TYPHOON_ERROR(tpn::mem_cast_nullptr_exception));
      char* p = pbuffer + Offset;
      TYPHOON_ASSERT(sizeof(T) <= (buffer_size - Offset), TYPHOON_ERROR(tpn::mem_cast_size_exception));

      ::new (p) T(tpn::forward<TArgs>(args)...);
    }
#endif

    //***********************************
    /// Get a reference to T
    //***********************************
    template <typename T>
    TYPHOON_NODISCARD T& ref()
    {
      TYPHOON_ASSERT((pbuffer != TYPHOON_NULLPTR), TYPHOON_ERROR(tpn::mem_cast_nullptr_exception));
      TYPHOON_ASSERT(sizeof(T) <= buffer_size, TYPHOON_ERROR(tpn::mem_cast_size_exception));

      return *reinterpret_cast<T*>(pbuffer);
    }

    //***********************************
    /// Get a const reference to T
    //***********************************
    template <typename T>
    TYPHOON_NODISCARD const T& ref() const
    {
      TYPHOON_ASSERT((pbuffer != TYPHOON_NULLPTR), TYPHOON_ERROR(tpn::mem_cast_nullptr_exception));
      TYPHOON_ASSERT(sizeof(T) <= buffer_size, TYPHOON_ERROR(tpn::mem_cast_size_exception));

      return *reinterpret_cast<const T*>(pbuffer);
    }

    //***********************************
    /// Get a reference to T at offset (dynamic)
    //***********************************
    template <typename T>
    TYPHOON_NODISCARD T& ref_at_offset(size_t offset)
    {
      TYPHOON_ASSERT((pbuffer != TYPHOON_NULLPTR), TYPHOON_ERROR(tpn::mem_cast_nullptr_exception));
      char* p = pbuffer + offset;
      TYPHOON_ASSERT(sizeof(T) <= (buffer_size - offset), TYPHOON_ERROR(tpn::mem_cast_size_exception));

      return *reinterpret_cast<T*>(p);
    }

    //***********************************
    /// Get a const reference to T at offset (dynamic)
    //***********************************
    template <typename T>
    TYPHOON_NODISCARD const T& ref_at_offset(size_t offset) const
    {
      TYPHOON_ASSERT((pbuffer != TYPHOON_NULLPTR), TYPHOON_ERROR(tpn::mem_cast_nullptr_exception));
      char* p = pbuffer + offset;
      TYPHOON_ASSERT(sizeof(T) <= (buffer_size - offset), TYPHOON_ERROR(tpn::mem_cast_size_exception));

      return *reinterpret_cast<const T*>(p);
    }

    //***********************************
    /// Get a reference to T at offset (static)
    //***********************************
    template <typename T, size_t Offset>
    TYPHOON_NODISCARD T& ref_at_offset()
    {
      TYPHOON_ASSERT((pbuffer != TYPHOON_NULLPTR), TYPHOON_ERROR(tpn::mem_cast_nullptr_exception));
      char* p = pbuffer + Offset;
      TYPHOON_ASSERT(sizeof(T) <= (buffer_size - Offset), TYPHOON_ERROR(tpn::mem_cast_size_exception));

      return *reinterpret_cast<T*>(p);
    }

    //***********************************
    /// Get a const reference to T at offset (static)
    //***********************************
    template <typename T, size_t Offset>
    TYPHOON_NODISCARD const T& ref_at_offset() const
    {
      TYPHOON_ASSERT((pbuffer != TYPHOON_NULLPTR), TYPHOON_ERROR(tpn::mem_cast_nullptr_exception));
      char* p = pbuffer + Offset;
      TYPHOON_ASSERT(sizeof(T) <= (buffer_size - Offset), TYPHOON_ERROR(tpn::mem_cast_size_exception));

      return *reinterpret_cast<const T*>(p);
    }

    //***********************************
    /// Get the size of the buffer
    //***********************************
    TYPHOON_NODISCARD size_t size() const
    {
      return buffer_size;
    }

    //***********************************
    /// Get the alignment of the buffer
    //***********************************
    TYPHOON_NODISCARD size_t alignment() const
    {
      typedef typename tpn::smallest_uint_for_bits<sizeof(uintptr_t)* CHAR_BIT>::type type;

      const type p = reinterpret_cast<type>(pbuffer);    

      return size_t(1U) << tpn::count_trailing_zeros(p);
    }

    //***********************************
    /// Set the pointer to the external buffer
    //***********************************
    void data(char* pbuffer_, size_t buffer_size_ = Undefined_Size)
    {
      pbuffer     = pbuffer_;
      buffer_size = buffer_size_;
    }

    //***********************************
    /// Get a pointer to the external buffer
    //***********************************
    TYPHOON_NODISCARD char* data()
    {
      return pbuffer;
    }

    //***********************************
    /// Get const a pointer to the external buffer
    //***********************************
    TYPHOON_NODISCARD const char* data() const
    {
      return pbuffer;
    }

  private:

    /// Pointer to the buffer
    char*  pbuffer;
    size_t buffer_size;
  };

  //*****************************************************************************
  /// mem_cast_var
  /// mem_cast from a variadic list of types
  //*****************************************************************************
#if TYPHOON_USING_CPP11 && !defined(TYPHOON_MEM_CAST_FORCE_CPP03_IMPLEMENTATION)
  template <typename... TTypes>
  using mem_cast_types = tpn::mem_cast<tpn::largest<TTypes...>::size, tpn::largest<TTypes...>::alignment>;
#else
  template <typename T1,         typename T2  = char, typename T3  = char, typename T4  = char,
            typename T5 = char,  typename T6  = char, typename T7  = char, typename T8  = char,
            typename T9 = char,  typename T10 = char, typename T11 = char, typename T12 = char,
            typename T13 = char, typename T14 = char, typename T15 = char, typename T16 = char>
  struct mem_cast_types : public tpn::mem_cast<tpn::largest<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::size,
                                               tpn::largest<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::alignment>
  {
  };
#endif
}

#endif
