///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tphn
https://www.tphncpp.com

Documentation: https://www.tphncpp.com/initializer_list.html

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

#ifndef TYPHOON_INITIALIZER_LIST_HPP
#define TYPHOON_INITIALIZER_LIST_HPP

#include "platform.hpp"

#if TYPHOON_HAS_INITIALIZER_LIST

#if (TYPHOON_USING_CPP11 && !defined(TYPHOON_NO_INITIALIZER_LIST))

#include <stddef.h>

// Use the compiler's std::initializer_list?
#if (TYPHOON_USING_STL && TYPHOON_NOT_USING_STLPORT && !defined(TYPHOON_FORCE_TYPHOON_INITIALIZER_LIST)) || defined(TYPHOON_IN_UNIT_TEST) || defined(TYPHOON_FORCE_STD_INITIALIZER_LIST)
  
	#include <initializer_list>

#else

// Use the TYPHOON's std::initializer_list
namespace std
{
#if defined(TYPHOON_COMPILER_MICROSOFT) 

  ///**************************************************************************
  /// A definition of initializer_list that is compatible with the Microsoft compiler
  ///**************************************************************************
  template <typename T>
  class initializer_list
  {
  public:

    using value_type      = T;
    using reference       = const T&;
    using const_reference = const T&;
    using size_type       = size_t;
    using iterator        = const T*;
    using const_iterator  = const T*;

    //*************************************************************************
    /// Default constructor 
    //*************************************************************************
    constexpr initializer_list() noexcept 
      : pfirst(nullptr), plast(nullptr)
    {
    }

    //*************************************************************************
    /// Constructor 
    //*************************************************************************
    constexpr initializer_list(const T* pfirst_, const T* plast_) noexcept
      : pfirst(pfirst_), plast(plast_) 
    {
    }

    //*************************************************************************
    /// Get the beginning of the list.
    //*************************************************************************
    constexpr const T* begin() const noexcept 
    { 
      return pfirst; 
    }

    //*************************************************************************
    /// Get the end of the list.
    //*************************************************************************
    constexpr const T* end() const noexcept 
    { 
      return plast; 
    }

    //*************************************************************************
    /// Get the size of the list.
    //*************************************************************************
    constexpr size_t size() const noexcept
    {
      return static_cast<size_t>(plast - pfirst);
    }

  private:

    const T* pfirst;
    const T* plast;
  };

  //*************************************************************************
  /// Get the beginning of the list.
  //*************************************************************************
  template<typename T>
  constexpr const T* begin(initializer_list<T> init) noexcept
  {
    return init.begin();
  }

  //*************************************************************************
  /// Get the end of the list.
  //*************************************************************************
  template <typename T>
  constexpr const T* end(initializer_list<T> init) noexcept
  {
    return init.end();
  }

#elif defined(TYPHOON_COMPILER_GCC)  || defined(TYPHOON_COMPILER_CLANG) || defined(TYPHOON_COMPILER_ARM6) || \
      defined(TYPHOON_COMPILER_ARM7) || defined(TYPHOON_COMPILER_IAR)   || defined(TYPHOON_COMPILER_TEXAS_INSTRUMENTS) || \
      defined(TYPHOON_COMPILER_INTEL)

  ///**************************************************************************
  /// A definition of initializer_list that is compatible with Clang, GCC and other compilers.
  ///**************************************************************************
  template<class T>
  class initializer_list
  {
  public:

    using value_type      = T;
    using reference       = const T&;
    using const_reference = const T&;
    using size_type       = size_t;
    using iterator        = const T*;
    using const_iterator  = const T*;

    //*************************************************************************
    /// Default constructor 
    //*************************************************************************
    constexpr initializer_list() noexcept 
      : pfirst(nullptr), length(0)
    {
    }

    //*************************************************************************
    /// Get the beginning of the list.
    //*************************************************************************
    constexpr const T* begin() const noexcept 
    { 
      return pfirst; 
    }

    //*************************************************************************
    /// Get the end of the list.
    //*************************************************************************
    constexpr const T* end() const noexcept 
    { 
      return pfirst + length; 
    }

    //*************************************************************************
    /// Get the size of the list.
    //*************************************************************************
    constexpr size_t size()  const noexcept
    {
      return length;
    }

  private:

    //*************************************************************************
    /// Constructor 
    //*************************************************************************
    constexpr initializer_list(const T* pfirst_, size_t length_) noexcept
      : pfirst(pfirst_)
      , length(length_)
    {
    }

    const T* pfirst;
    size_t   length;
  };

  //*************************************************************************
  /// Get the beginning of the list.
  //*************************************************************************
  template<class T>
  constexpr const T* begin(initializer_list<T> init) noexcept
  {
    return init.begin();
  }

  //*************************************************************************
  /// Get the end of the list.
  //*************************************************************************
  template<class T>
  constexpr const T* end(initializer_list<T> init) noexcept
  {
    return init.end();
  }
#else

  #error No definition for initializer_list is currently available for your compiler. Visit https://github.com/TYPHOONCPP/tphn/issues to request support.

#endif // Compiler tests
}

#endif // (TYPHOON_USING_STL && TYPHOON_NOT_USING_STLPORT && !defined(TYPHOON_FORCE_TYPHOON_INITIALIZER_LIST)) || defined(TYPHOON_IN_UNIT_TEST) || defined(TYPHOON_FORCE_STD_INITIALIZER_LIST)
#endif // TYPHOON_USING_CPP11 && !defined(TYPHOON_NO_INITIALIZER_LIST)
#endif // TYPHOON_HAS_INITIALIZER_LIST
#endif // TYPHOON_INITIALIZER_LIST_HPP
