///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tpn
https://www.tpncpp.com

Copyright(c) 2015 John Wellbelove

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

#ifndef TYPHOON_OPTIONAL_HPP
#define TYPHOON_OPTIONAL_HPP

#include "platform.hpp"
#include "alignment.hpp"
#include "type_traits.hpp"
#include "exception.hpp"
#include "error_handler.hpp"
#include "utility.hpp"
#include "placement_new.hpp"

namespace tpn
{
  //*****************************************************************************
  /// A null option type.
  ///\ingroup utilities
  //*****************************************************************************
  class nullopt_t
  {
  public:

    // Convertible to any type of null non-member pointer.
    template<class T>
    operator T*() const
    {
      return 0;
    }

  private:

    // Can't take address of nullopt.
    void operator&() const;
  };

  //*****************************************************************************
  /// A null option.
  ///\ingroup utilities
  //*****************************************************************************
  const nullopt_t nullopt = {};

  //***************************************************************************
  /// Exception for optional.
  ///\ingroup list
  //***************************************************************************
  class optional_exception : public exception
  {
  public:

    optional_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Invalid exception for optional.
  ///\ingroup list
  //***************************************************************************
  class optional_invalid : public optional_exception
  {
  public:

    optional_invalid(string_type file_name_, numeric_type line_number_)
      : optional_exception("optional: invalid", file_name_, line_number_)
    {
    }
  };

  //*****************************************************************************
  /// An optional type.
  /// If the optional type is not initialised then a type is not constructed.
  /// See http://en.cppreference.com/w/cpp/utility/optional
  ///\tparam The type to store.
  ///\ingroup utilities
  //*****************************************************************************
  template <typename T, bool is_pod = tpn::is_pod<T>::value>
  class optional;

  //*****************************************************************************
  /// For non POD types.
  //*****************************************************************************
  template <typename T>
  class optional<T, false>
  {
  public:

    //***************************************************************************
    /// Constructor.
    //***************************************************************************
#include "tpn/private/diagnostic_uninitialized_push.hpp"
    optional()
      : valid(false)
    {
    }
#include "tpn/private/diagnostic_pop.hpp"

    //***************************************************************************
    /// Constructor with nullopt.
    //***************************************************************************
#include "tpn/private/diagnostic_uninitialized_push.hpp"
    optional(tpn::nullopt_t)
      : valid(false)
    {
    }
#include "tpn/private/diagnostic_pop.hpp"

    //***************************************************************************
    /// Copy constructor.
    //***************************************************************************
    optional(const optional& other)
      : valid(bool(other))
    {
      if (valid)
      {
       ::new (storage.template get_address<T>()) T(other.value());
      }
    }

#if TYPHOON_USING_CPP11
    //***************************************************************************
    /// Move constructor.
    //***************************************************************************
    optional(optional&& other)
      : valid(bool(other))
    {
      if (valid)
      {
        ::new (storage.template get_address<T>()) T(tpn::move(other.value()));
      }
    }
#endif

    //***************************************************************************
    /// Constructor from value type.
    //***************************************************************************
    optional(const T& value_)
    {
     ::new (storage.template get_address<T>()) T(value_);
      valid = true;
    }

#if TYPHOON_USING_CPP11
    //***************************************************************************
    /// Constructor from value type.
    //***************************************************************************
    optional(T&& value_)
    {
      ::new (storage.template get_address<T>()) T(tpn::move(value_));
      valid = true;
    }
#endif

    //***************************************************************************
    /// Destructor.
    //***************************************************************************
    ~optional()
    {
      if (valid)
      {
        storage.template get_reference<T>().~T();
      }
    }

    //***************************************************************************
    /// Assignment operator from nullopt.
    //***************************************************************************
    optional& operator =(tpn::nullopt_t)
    {
      if (valid)
      {
        storage.template get_reference<T>().~T();
        valid = false;
      }

      return *this;
    }

    //***************************************************************************
    /// Assignment operator from optional.
    //***************************************************************************
    optional& operator =(const optional& other)
    {
      if (this != &other)
      {
        if (valid && !bool(other))
        {
          storage.template get_reference<T>().~T();
          valid = false;
        }
        else if (bool(other))
        {
          if (valid)
          {
            storage.template get_reference<T>() = other.value();
          }
          else
          {
           ::new (storage.template get_address<T>()) T(other.value());
            valid = true;
          }
        }
      }

      return *this;
    }

#if TYPHOON_USING_CPP11
    //***************************************************************************
    /// Assignment operator from optional.
    //***************************************************************************
    optional& operator =(optional&& other)
    {
      if (this != &other)
      {
        if (valid && !bool(other))
        {
          storage.template get_reference<T>().~T();
          valid = false;
        }
        else if (bool(other))
        {
          if (valid)
          {
            storage.template get_reference<T>() = tpn::move(other.value());
          }
          else
          {
            ::new (storage.template get_address<T>()) T(tpn::move(other.value()));
            valid = true;
          }
        }
      }

      return *this;
    }
#endif

    //***************************************************************************
    /// Assignment operator from value type.
    //***************************************************************************
    optional& operator =(const T& value_)
    {
      if (valid)
      {
        storage.template get_reference<T>() = value_;
      }
      else
      {
       ::new (storage.template get_address<T>()) T(value_);
        valid = true;
      }

      return *this;
    }

#if TYPHOON_USING_CPP11
    //***************************************************************************
    /// Assignment operator from value type.
    //***************************************************************************
    optional& operator =(T&& value_)
    {
      if (valid)
      {
        storage.template get_reference<T>() = tpn::move(value_);
      }
      else
      {
        ::new (storage.template get_address<T>()) T(tpn::move(value_));
        valid = true;
      }

      return *this;
    }
#endif

    //***************************************************************************
    /// Pointer operator.
    //***************************************************************************
    T* operator ->()
    {
#if TYPHOON_IS_DEBUG_BUILD
      TYPHOON_ASSERT(valid, TYPHOON_ERROR(optional_invalid));
#endif

      return storage.template get_address<T>();
    }

    //***************************************************************************
    /// Pointer operator.
    //***************************************************************************
    const T* operator ->() const
    {
#if TYPHOON_IS_DEBUG_BUILD
      TYPHOON_ASSERT(valid, TYPHOON_ERROR(optional_invalid));
#endif

      return storage.template get_address<T>();
    }

    //***************************************************************************
    /// Dereference operator.
    //***************************************************************************
    T& operator *()
    {
#if TYPHOON_IS_DEBUG_BUILD
      TYPHOON_ASSERT(valid, TYPHOON_ERROR(optional_invalid));
#endif

      return storage.template get_reference<T>();
    }

    //***************************************************************************
    /// Dereference operator.
    //***************************************************************************
    const T& operator *() const
    {
#if TYPHOON_IS_DEBUG_BUILD
      TYPHOON_ASSERT(valid, TYPHOON_ERROR(optional_invalid));
#endif

      return storage.template get_reference<T>();
    }

    //***************************************************************************
    /// Bool conversion operator.
    //***************************************************************************
    TYPHOON_EXPLICIT operator bool() const
    {
      return valid;
    }

    //***************************************************************************
    // Check whether optional contains value
    //***************************************************************************
    TYPHOON_CONSTEXPR bool has_value() const TYPHOON_NOEXCEPT
    {
      return valid;
    }

    //***************************************************************************
    /// Get a reference to the value.
    //***************************************************************************
    T& value()
    {
#if TYPHOON_IS_DEBUG_BUILD
      TYPHOON_ASSERT(valid, TYPHOON_ERROR(optional_invalid));
#endif

      return storage.template get_reference<T>();
    }

    //***************************************************************************
    /// Get a const reference to the value.
    //***************************************************************************
    const T& value() const
    {
#if TYPHOON_IS_DEBUG_BUILD
      TYPHOON_ASSERT(valid, TYPHOON_ERROR(optional_invalid));
#endif

      return storage.template get_reference<T>();
    }

    //***************************************************************************
    /// Gets the value or a default if no valid.
    //***************************************************************************
    T value_or(T default_value) const
    {
      return valid ? value() : default_value;
    }

    //***************************************************************************
    /// Swaps this value with another.
    //***************************************************************************
    void swap(optional& other)
    {
      optional temp(*this);
      *this = other;
      other = temp;
    }

    //***************************************************************************
    /// Reset back to invalid.
    //***************************************************************************
    void reset()
    {
      if (valid)
      {
        storage.template get_reference<T>().~T();
        valid = false;
      }
    }

#if TYPHOON_USING_CPP11  && TYPHOON_NOT_USING_STLPORT && !defined(TYPHOON_OPTIONAL_FORCE_CPP03_IMPLEMENTATION)
    //*************************************************************************
    /// Emplaces a value.
    ///\param args The arguments to construct with.
    //*************************************************************************
    template <typename ... Args>
    void emplace(Args && ... args)
    {
      if (valid)
      {
        // Destroy the old one.
        storage.template get_reference<T>().~T();
      }

      ::new (storage.template get_address<T>()) T(TYPHOON_OR_STD::forward<Args>(args)...);
      valid = true;
    }
#else
    //*************************************************************************
    /// Emplaces a value.
    /// 1 parameter.
    //*************************************************************************
    template <typename T1>
    void emplace(const T1& value1)
    {
      if (valid)
      {
        // Destroy the old one.
        storage.template get_reference<T>().~T();
      }

      ::new (storage.template get_address<T>()) T(value1);
      valid = true;
    }

    //*************************************************************************
    /// Emplaces a value.
    /// 2 parameters.
    //*************************************************************************
    template <typename T1, typename T2>
    void emplace(const T1& value1, const T2& value2)
    {
      if (valid)
      {
        // Destroy the old one.
        storage.template get_reference<T>().~T();
      }

      ::new (storage.template get_address<T>()) T(value1, value2);
      valid = true;
    }

    //*************************************************************************
    /// Emplaces a value.
    /// 3 parameters.
    //*************************************************************************
    template <typename T1, typename T2, typename T3>
    void emplace(const T1& value1, const T2& value2, const T3& value3)
    {
      if (valid)
      {
        // Destroy the old one.
        storage.template get_reference<T>().~T();
      }

      ::new (storage.template get_address<T>()) T(value1, value2, value3);
      valid = true;
    }

    //*************************************************************************
    /// Emplaces a value.
    /// 4 parameters.
    //*************************************************************************
    template <typename T1, typename T2, typename T3, typename T4>
    void emplace(const T1& value1, const T2& value2, const T3& value3, const T4& value4)
    {
      if (valid)
      {
        // Destroy the old one.
        storage.template get_reference<T>().~T();
      }

      ::new (storage.template get_address<T>()) T(value1, value2, value3, value4);
      valid = true;
    }
#endif

  private:

    bool valid;
    typename tpn::aligned_storage_as<sizeof(T), T>::type storage;
  };

  //*****************************************************************************
  /// For POD types.
  //*****************************************************************************
  template <typename T>
  class optional<T, true>
  {
  public:

    //***************************************************************************
    /// Constructor.
    //***************************************************************************
    TYPHOON_CONSTEXPR14 optional()
      : valid(false)
      , storage()
    {
    }

    //***************************************************************************
    /// Constructor with nullopt.
    //***************************************************************************
    TYPHOON_CONSTEXPR14 optional(tpn::nullopt_t)
      : valid(false)
      , storage()
    {
    }

    //***************************************************************************
    /// Copy constructor.
    //***************************************************************************
    TYPHOON_CONSTEXPR14 optional(const optional& other)
      : valid(bool(other))
      , storage(other.storage)
    {
    }

#if TYPHOON_USING_CPP11
    //***************************************************************************
    /// Move constructor.
    //***************************************************************************
    TYPHOON_CONSTEXPR14 optional(optional&& other)
      : valid(bool(other))
      , storage(tpn::move(other.storage))
    {
    }
#endif

    //***************************************************************************
    /// Constructor from value type.
    //***************************************************************************
    TYPHOON_CONSTEXPR14 optional(const T& value_)
      : valid(true)
      , storage(value_)
    {
    }

#if TYPHOON_USING_CPP11
    //***************************************************************************
    /// Constructor from value type.
    //***************************************************************************
    TYPHOON_CONSTEXPR14 optional(T&& value_)
      : valid(true)
      , storage(tpn::move(value_))
    {
    }
#endif

    //***************************************************************************
    /// Assignment operator from nullopt.
    //***************************************************************************
    TYPHOON_CONSTEXPR14 optional& operator =(tpn::nullopt_t)
    {
      return *this;
    }

    //***************************************************************************
    /// Assignment operator from optional.
    //***************************************************************************
    TYPHOON_CONSTEXPR14 optional& operator =(const optional& other)
    {
      if (this != &other)
      {
        storage = other.storage;
        valid   = other.valid;
      }

      return *this;
    }

#if TYPHOON_USING_CPP11
    //***************************************************************************
    /// Assignment operator from optional.
    //***************************************************************************
    TYPHOON_CONSTEXPR14 optional& operator =(optional&& other)
    {
      if (this != &other)
      {
        storage = tpn::move(other.storage);
        valid   = other.valid;
      }

      return *this;
    }
#endif

    //***************************************************************************
    /// Assignment operator from value type.
    //***************************************************************************
    TYPHOON_CONSTEXPR14 optional& operator =(const T& value_)
    {
      storage = value_;
      valid = true;

      return *this;
    }

#if TYPHOON_USING_CPP11
    //***************************************************************************
    /// Assignment operator from value type.
    //***************************************************************************
    TYPHOON_CONSTEXPR14 optional& operator =(T&& value_)
    {
      storage = tpn::move(value_);
      valid = true;

      return *this;
    }
#endif

    //***************************************************************************
    /// Pointer operator.
    //***************************************************************************
    TYPHOON_CONSTEXPR14 T* operator ->()
    {
#if TYPHOON_IS_DEBUG_BUILD
      TYPHOON_ASSERT(valid, TYPHOON_ERROR(optional_invalid));
#endif

      return &storage;
    }

    //***************************************************************************
    /// Pointer operator.
    //***************************************************************************
    TYPHOON_CONSTEXPR14 const T* operator ->() const
    {
#if TYPHOON_IS_DEBUG_BUILD
      TYPHOON_ASSERT(valid, TYPHOON_ERROR(optional_invalid));
#endif

      return &storage;
    }

    //***************************************************************************
    /// Dereference operator.
    //***************************************************************************
    TYPHOON_CONSTEXPR14 T& operator *()
    {
#if TYPHOON_IS_DEBUG_BUILD
      TYPHOON_ASSERT(valid, TYPHOON_ERROR(optional_invalid));
#endif

      return storage;
    }

    //***************************************************************************
    /// Dereference operator.
    //***************************************************************************
    TYPHOON_CONSTEXPR14 const T& operator *() const
    {
#if TYPHOON_IS_DEBUG_BUILD
      TYPHOON_ASSERT(valid, TYPHOON_ERROR(optional_invalid));
#endif

      return storage;
    }

    //***************************************************************************
    /// Bool conversion operator.
    //***************************************************************************
    TYPHOON_CONSTEXPR14 operator bool() const
    {
      return valid;
    }

    //***************************************************************************
    // Check whether optional contains value
    //***************************************************************************
    TYPHOON_CONSTEXPR14 bool has_value() const TYPHOON_NOEXCEPT
    {
      return valid;
    }

    //***************************************************************************
    /// Get a reference to the value.
    //***************************************************************************
    TYPHOON_CONSTEXPR14 T& value()
    {
#if TYPHOON_IS_DEBUG_BUILD
      TYPHOON_ASSERT(valid, TYPHOON_ERROR(optional_invalid));
#endif

      return storage;
    }

    //***************************************************************************
    /// Get a const reference to the value.
    //***************************************************************************
    TYPHOON_CONSTEXPR14 const T& value() const
    {
#if TYPHOON_IS_DEBUG_BUILD
      TYPHOON_ASSERT(valid, TYPHOON_ERROR(optional_invalid));
#endif

      return storage;
    }

    //***************************************************************************
    /// Gets the value or a default if no valid.
    //***************************************************************************
    TYPHOON_CONSTEXPR14 T value_or(T default_value) const
    {
      return valid ? value() : default_value;
    }

    //***************************************************************************
    /// Swaps this value with another.
    //***************************************************************************
    TYPHOON_CONSTEXPR14 void swap(optional& other)
    {
      optional temp(*this);
      *this = other;
      other = temp;
    }

    //***************************************************************************
    /// Reset back to invalid.
    //***************************************************************************
    TYPHOON_CONSTEXPR14 void reset()
    {
      valid = false;
    }

#if TYPHOON_USING_CPP11  && TYPHOON_NOT_USING_STLPORT && !defined(TYPHOON_OPTIONAL_FORCE_CPP03_IMPLEMENTATION)
    //*************************************************************************
    /// Emplaces a value.
    ///\param args The arguments to construct with.
    //*************************************************************************
    template <typename ... Args>
    TYPHOON_CONSTEXPR14 void emplace(Args && ... args)
    {
      storage = T(TYPHOON_OR_STD::forward<Args>(args)...);
      valid = true;
    }
#else
    //*************************************************************************
    /// Emplaces a value.
    /// 1 parameter.
    //*************************************************************************
    template <typename T1>
    void emplace(const T1& value1)
    {
      storage = value1;
      valid = true;
    }

    //*************************************************************************
    /// Emplaces a value.
    /// 2 parameters.
    //*************************************************************************
    template <typename T1, typename T2>
    void emplace(const T1& value1, const T2& value2)
    {
      storage = T(value1, value2);
      valid = true;
    }

    //*************************************************************************
    /// Emplaces a value.
    /// 3 parameters.
    //*************************************************************************
    template <typename T1, typename T2, typename T3>
    void emplace(const T1& value1, const T2& value2, const T3& value3)
    {
      storage = T(value1, value2, value3);
      valid = true;
    }

    //*************************************************************************
    /// Emplaces a value.
    /// 4 parameters.
    //*************************************************************************
    template <typename T1, typename T2, typename T3, typename T4>
    void emplace(const T1& value1, const T2& value2, const T3& value3, const T4& value4)
    {
      storage = T(value1, value2, value3, value4);
      valid = true;
    }
#endif

  private:

    bool valid;
    T    storage;
  };

  //***************************************************************************
  /// Equality operator. cppreference 1
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR14 bool operator ==(const tpn::optional<T>& lhs, const tpn::optional<T>& rhs)
  {
    if (bool(lhs) != bool(rhs))
    {
      return false;
    }
    else if (!bool(lhs) && !bool(rhs))
    {
      return true;
    }
    else
    {
      return lhs.value() == rhs.value();
    }
  }

  //***************************************************************************
  /// Equality operator. cppreference 2
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR14 bool operator !=(const tpn::optional<T>& lhs, const tpn::optional<T>& rhs)
  {
    return !(lhs == rhs);
  }

  //***************************************************************************
  /// Less than operator. cppreference 3
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR14 bool operator <(const tpn::optional<T>& lhs, const tpn::optional<T>& rhs)
  {
    if (!bool(rhs))
    {
      return false;
    }
    else if (!bool(lhs))
    {
      return true;
    }
    else
    {
      return lhs.value() < rhs.value();
    }
  }

  //***************************************************************************
  /// Less than equal operator. cppreference 4
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR14 bool operator <=(const tpn::optional<T>& lhs, const tpn::optional<T>& rhs)
  {
    if (!bool(lhs))
    {
      return true;
    }
    else if (!bool(rhs))
    {
      return false;
    }
    else
    {
      return lhs.value() <= rhs.value();
    }
  }

  //***************************************************************************
  /// greater than operator. cppreference 5
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR14 bool operator >(const tpn::optional<T>& lhs, const tpn::optional<T>& rhs)
  {
      if (!bool(lhs))
      {
        return false;
      }
      else if (!bool(rhs))
      {
        return true;
      }
      else
      {
        return lhs.value() > rhs.value();
      }
  }

  //***************************************************************************
  /// greater than equal operator. cppreference 6
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR14 bool operator >=(const tpn::optional<T>& lhs, const tpn::optional<T>& rhs)
  {
    if (!bool(rhs))
    {
      return true;
    }
    else if (!bool(lhs))
    {
      return false;
    }
    else
    {
      return lhs.value() >= rhs.value();
    }
  }

  //***************************************************************************
  /// Equality operator. cppreference 7
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR14 bool operator ==(const tpn::optional<T>& lhs, tpn::nullopt_t)
  {
    return !bool(lhs);
  }

  //***************************************************************************
  /// Equality operator. cppreference 8
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR14 bool operator ==(tpn::nullopt_t, const tpn::optional<T>& rhs)
  {
    return !bool(rhs);
  }

  //***************************************************************************
  /// Inequality operator. cppreference 9
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR14 bool operator !=(const tpn::optional<T>& lhs, tpn::nullopt_t)
  {
    return !(lhs == tpn::nullopt);
  }

  //***************************************************************************
  /// Inequality operator. cppreference 10
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR14 bool operator !=(tpn::nullopt_t , const tpn::optional<T>& rhs)
  {
    return !(tpn::nullopt == rhs);
  }

  //***************************************************************************
  /// Less than operator. cppreference 11
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR14 bool operator <(const tpn::optional<T>&, tpn::nullopt_t)
  {
    return false;
  }

  //***************************************************************************
  /// Less than operator. cppreference 12
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR14 bool operator <(tpn::nullopt_t, const tpn::optional<T>& rhs)
  {
    return bool(rhs);
  }

  //***************************************************************************
  /// Less than equal operator. cppreference 13
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR14 bool operator <=(const tpn::optional<T>& lhs, tpn::nullopt_t)
  {
    return !bool(lhs);
  }

  //***************************************************************************
  /// Less than equal operator. cppreference 14
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR14 bool operator <=(tpn::nullopt_t, const tpn::optional<T>&)
  {
    return true;
  }

  //***************************************************************************
  /// Greater than operator. cppreference 15
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR14 bool operator >(const tpn::optional<T>& lhs, tpn::nullopt_t)
  {
    return bool(lhs);
  }

  //***************************************************************************
  /// Greater than operator. cppreference 16
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR14 bool operator >(tpn::nullopt_t, const tpn::optional<T>&)
  {
    return false;
  }

  //***************************************************************************
  /// Greater than equal operator. cppreference 17
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR14 bool operator >=(const tpn::optional<T>&, tpn::nullopt_t)
  {
    return true;
  }

  //***************************************************************************
  /// Greater than equal operator. cppreference 18
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR14 bool operator >=(tpn::nullopt_t, const tpn::optional<T>& rhs)
  {
    return !bool(rhs);
  }

  //***************************************************************************
  /// Equality operator. cppreference 19
  //**************************************************************************
  template <typename T, typename U>
  TYPHOON_CONSTEXPR14 bool operator ==(const tpn::optional<T>& lhs, const U& rhs)
  {
    return bool(lhs) ? lhs.value() == rhs : false;
  }

  //***************************************************************************
  /// Inequality operator. cppreference 21
  //**************************************************************************
  template <typename T, typename U>
  TYPHOON_CONSTEXPR14 bool operator !=(const tpn::optional<T>& lhs, const U& rhs)
  {
    return !(lhs == rhs);
  }

  //***************************************************************************
  /// Equality operator. cppreference 20
  //**************************************************************************
  template <typename T, typename U>
  TYPHOON_CONSTEXPR14 bool operator ==(const U& lhs, const tpn::optional<T>& rhs)
  {
    return bool(rhs) ? rhs.value() == lhs : false;
  }

  //***************************************************************************
  /// Inequality operator. cppreference 22
  //**************************************************************************
  template <typename T, typename U>
  TYPHOON_CONSTEXPR14 bool operator !=(const U& lhs, const tpn::optional<T>& rhs)
  {
    return !(lhs == rhs);
  }

  //***************************************************************************
  /// Less than operator. cppreference 23
  //***************************************************************************
  template <typename T, typename U>
  TYPHOON_CONSTEXPR14 bool operator <(const tpn::optional<T>& lhs, const U& rhs)
  {
    return bool(lhs) ? lhs.value() < rhs : true;
  }

  //***************************************************************************
  /// Less than operator. cppreference 24
  //***************************************************************************
  template <typename T, typename U>
  TYPHOON_CONSTEXPR14 bool operator <(const U& lhs, const tpn::optional<T>& rhs)
  {
    return bool(rhs) ? lhs < rhs.value() : false;
  }

  //***************************************************************************
  /// Less than equal operator. cppreference 25
  //***************************************************************************
  template <typename T, typename U>
  TYPHOON_CONSTEXPR14 bool operator <=(const tpn::optional<T>& lhs, const U& rhs)
  {
    return bool(lhs) ? lhs.value() <= rhs : true;
  }

  //***************************************************************************
  /// Less than equal operator. cppreference 26
  //***************************************************************************
  template <typename T, typename U>
  TYPHOON_CONSTEXPR14 bool operator <=(const U& lhs, const tpn::optional<T>& rhs)
  {
    return bool(rhs) ? lhs <= rhs.value() : false;
  }

  //***************************************************************************
  /// Greater than operator. cppreference 27
  //***************************************************************************
  template <typename T, typename U>
  TYPHOON_CONSTEXPR14 bool operator >(const tpn::optional<T>& lhs, const U& rhs)
  {
    return bool(lhs) ? lhs.value() > rhs  : false;
  }

  //***************************************************************************
  /// Greater than operator. cppreference 28
  //***************************************************************************
  template <typename T, typename U>
  TYPHOON_CONSTEXPR14 bool operator >(const U& lhs, const tpn::optional<T>& rhs)
  {
    return bool(rhs) ? lhs > rhs.value() : true;
  }

  //***************************************************************************
  /// Greater than equal operator. cppreference 29
  //***************************************************************************
  template <typename T, typename U>
  TYPHOON_CONSTEXPR14 bool operator >=(const tpn::optional<T>& lhs, const U& rhs)
  {
    return bool(lhs) ? lhs.value() >= rhs : false;
  }

  //***************************************************************************
  /// Greater than equal operator. cppreference 30
  //***************************************************************************
  template <typename T, typename U>
  TYPHOON_CONSTEXPR14 bool operator >=(const U& lhs, const tpn::optional<T>& rhs)
  {
    return bool(rhs) ? lhs >= rhs.value() : true;
  }

  //***************************************************************************
  /// Make an optional.
  //***************************************************************************
  template <typename T>
  TYPHOON_CONSTEXPR14 tpn::optional<typename tpn::decay<T>::type> make_optional(T& value)
  {
    return tpn::optional<typename tpn::decay<T>::type>(value);
  }

  //***************************************************************************
  /// Template deduction guides.
  //***************************************************************************
#if TYPHOON_CPP17_SUPPORTED
  template <typename T>
  optional(T) -> optional<T>;
#endif
}

//*************************************************************************
/// Swaps the values.
//*************************************************************************
template <typename T>
TYPHOON_CONSTEXPR14 void swap(tpn::optional<T>& lhs, tpn::optional<T>& rhs)
{
  lhs.swap(rhs);
}

#endif
