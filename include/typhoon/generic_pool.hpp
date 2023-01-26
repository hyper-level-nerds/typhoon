///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tpn
https://www.tpncpp.com

Copyright(c) 2014 John Wellbelove

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

#ifndef TYPHOON_GENERIC_POOL_HPP
#define TYPHOON_GENERIC_POOL_HPP

#include "platform.hpp"
#include "ipool.hpp"
#include "type_traits.hpp"
#include "static_assert.hpp"
#include "alignment.hpp"

#define TYPHOON_POOL_CPP03_CODE 0

//*****************************************************************************
///\defgroup pool pool
/// A fixed capacity pool.
///\ingroup containers
//*****************************************************************************

namespace tpn
{
  //*************************************************************************
  /// A templated abstract pool implementation that uses a fixed size pool.
  ///\ingroup pool
  //*************************************************************************
  template <const size_t VTypeSize, const size_t VAlignment, const size_t VSize>
  class generic_pool : public tpn::ipool
  {
  public:

    static TYPHOON_CONSTANT size_t SIZE      = VSize;
    static TYPHOON_CONSTANT size_t ALIGNMENT = VAlignment;
    static TYPHOON_CONSTANT size_t TYPE_SIZE = VTypeSize;

    //*************************************************************************
    /// Constructor
    //*************************************************************************
    generic_pool()
      : tpn::ipool(reinterpret_cast<char*>(&buffer[0]), Element_Size, VSize)
    {
    }

    //*************************************************************************
    /// Allocate an object from the pool.
    /// If asserts or exceptions are enabled and there are no more free items an
    /// tpn::pool_no_allocation if thrown, otherwise a null pointer is returned.
    /// Static asserts if the specified type is too large for the pool.
    //*************************************************************************
    template <typename U>
    U* allocate()
    {
      TYPHOON_STATIC_ASSERT(tpn::alignment_of<U>::value <= VAlignment, "Type has incompatible alignment");
      TYPHOON_STATIC_ASSERT(sizeof(U) <= VTypeSize, "Type too large for pool");
      return ipool::allocate<U>();
    }

#if TYPHOON_CPP11_NOT_SUPPORTED || TYPHOON_POOL_CPP03_CODE || TYPHOON_USING_STLPORT
    //*************************************************************************
    /// Allocate storage for an object from the pool and create with default.
    /// If asserts or exceptions are enabled and there are no more free items an
    /// tpn::pool_no_allocation if thrown, otherwise a null pointer is returned.
    //*************************************************************************
    template <typename U>
    U* create()
    {
      TYPHOON_STATIC_ASSERT(tpn::alignment_of<U>::value <= VAlignment, "Type has incompatible alignment");
      TYPHOON_STATIC_ASSERT(sizeof(U) <= VTypeSize, "Type too large for pool");
      return ipool::create<U>();
    }

    //*************************************************************************
    /// Allocate storage for an object from the pool and create with 1 parameter.
    /// If asserts or exceptions are enabled and there are no more free items an
    /// tpn::pool_no_allocation if thrown, otherwise a null pointer is returned.
    //*************************************************************************
    template <typename U, typename T1>
    U* create(const T1& value1)
    {
      TYPHOON_STATIC_ASSERT(tpn::alignment_of<U>::value <= VAlignment, "Type has incompatible alignment");
      TYPHOON_STATIC_ASSERT(sizeof(U) <= VTypeSize, "Type too large for pool");
      return ipool::create<U>(value1);
    }

    //*************************************************************************
    /// Allocate storage for an object from the pool and create with 2 parameters.
    /// If asserts or exceptions are enabled and there are no more free items an
    /// tpn::pool_no_allocation if thrown, otherwise a null pointer is returned.
    //*************************************************************************
    template <typename U, typename T1, typename T2>
    U* create(const T1& value1, const T2& value2)
    {
      TYPHOON_STATIC_ASSERT(tpn::alignment_of<U>::value <= VAlignment, "Type has incompatible alignment");
      TYPHOON_STATIC_ASSERT(sizeof(U) <= VTypeSize, "Type too large for pool");
      return ipool::create<U>(value1, value2);
    }

    //*************************************************************************
    /// Allocate storage for an object from the pool and create with 3 parameters.
    /// If asserts or exceptions are enabled and there are no more free items an
    /// tpn::pool_no_allocation if thrown, otherwise a null pointer is returned.
    //*************************************************************************
    template <typename U, typename T1, typename T2, typename T3>
    U* create(const T1& value1, const T2& value2, const T3& value3)
    {
      TYPHOON_STATIC_ASSERT(tpn::alignment_of<U>::value <= VAlignment, "Type has incompatible alignment");
      TYPHOON_STATIC_ASSERT(sizeof(U) <= VTypeSize, "Type too large for pool");
      return ipool::create<U>(value1, value2, value3);
    }

    //*************************************************************************
    /// Allocate storage for an object from the pool and create with 4 parameters.
    /// If asserts or exceptions are enabled and there are no more free items an
    /// tpn::pool_no_allocation if thrown, otherwise a null pointer is returned.
    //*************************************************************************
    template <typename U, typename T1, typename T2, typename T3, typename T4>
    U* create(const T1& value1, const T2& value2, const T3& value3, const T4& value4)
    {
      TYPHOON_STATIC_ASSERT(tpn::alignment_of<U>::value <= VAlignment, "Type has incompatible alignment");
      TYPHOON_STATIC_ASSERT(sizeof(U) <= VTypeSize, "Type too large for pool");
      return ipool::create<U>(value1, value2, value3, value4);
    }
#else
    //*************************************************************************
    /// Emplace with variadic constructor parameters.
    //*************************************************************************
    template <typename U, typename... Args>
    U* create(Args&&... args)
    {
      TYPHOON_STATIC_ASSERT(tpn::alignment_of<U>::value <= VAlignment, "Type has incompatible alignment");
      TYPHOON_STATIC_ASSERT(sizeof(U) <= VTypeSize, "Type too large for pool");
      return ipool::create<U>(tpn::forward<Args>(args)...);
    }
#endif

    //*************************************************************************
    /// Destroys the object.
    /// Undefined behaviour if the pool does not contain a 'U'.
    /// \param p_object A pointer to the object to be destroyed.
    //*************************************************************************
    template <typename U>
    void destroy(const U* const p_object)
    {
      TYPHOON_STATIC_ASSERT(tpn::alignment_of<U>::value <= VAlignment, "Type has incompatible alignment");
      TYPHOON_STATIC_ASSERT(sizeof(U) <= VTypeSize, "Type too large for pool");
      p_object->~U();
      ipool::release(p_object);
    }

  private:

    // The pool element.
    union Element
    {
      char* next;              ///< Pointer to the next free element.
      char      value[VTypeSize]; ///< Storage for value type.
      typename  tpn::type_with_alignment<VAlignment>::type dummy; ///< Dummy item to get correct alignment.
    };

    ///< The memory for the pool of objects.
    typename tpn::aligned_storage<sizeof(Element), tpn::alignment_of<Element>::value>::type buffer[VSize];

    static TYPHOON_CONSTANT uint32_t Element_Size = sizeof(Element);

    // Should not be copied.
    generic_pool(const generic_pool&) TYPHOON_DELETE;
    generic_pool& operator =(const generic_pool&) TYPHOON_DELETE;
  };

  //*************************************************************************
  /// A templated abstract pool implementation that uses a fixed size pool.
  /// The storage for the pool is supplied externally.
  ///\ingroup pool
  //*************************************************************************
  template <const size_t VTypeSize, const size_t VAlignment>
  class generic_pool_ext : public tpn::ipool 
  {
  private:
    // The pool element.
    union element_internal 
    {
      char* next;                                                 ///< Pointer to the next free element.
      char value[VTypeSize];                                      ///< Storage for value type.
      typename tpn::type_with_alignment<VAlignment>::type dummy;  ///< Dummy item to get correct alignment.
    };

    static const size_t ELEMENT_INTERNAL_SIZE = sizeof(element_internal);

  public:
    static TYPHOON_CONSTANT size_t ALIGNMENT = VAlignment;
    static TYPHOON_CONSTANT size_t TYPE_SIZE = VTypeSize;

    typedef typename tpn::aligned_storage<sizeof(element_internal), tpn::alignment_of<element_internal>::value>::type element;

    //*************************************************************************
    /// Constructor
    //*************************************************************************
    generic_pool_ext(element* buffer, size_t size) 
      : tpn::ipool(reinterpret_cast<char*>(&buffer[0]), ELEMENT_INTERNAL_SIZE, size) 
    {
    }

    //*************************************************************************
    /// Allocate an object from the pool.
    /// If asserts or exceptions are enabled and there are no more free items an
    /// tpn::pool_no_allocation if thrown, otherwise a null pointer is returned.
    /// Static asserts if the specified type is too large for the pool.
    //*************************************************************************
    template <typename U>
    U* allocate()
    {
      TYPHOON_STATIC_ASSERT(tpn::alignment_of<U>::value <= VAlignment, "Type has incompatible alignment");
      TYPHOON_STATIC_ASSERT(sizeof(U) <= VTypeSize, "Type too large for pool");
      return ipool::allocate<U>();
    }

#if TYPHOON_CPP11_NOT_SUPPORTED || TYPHOON_POOL_CPP03_CODE || TYPHOON_USING_STLPORT
    //*************************************************************************
    /// Allocate storage for an object from the pool and create with default.
    /// If asserts or exceptions are enabled and there are no more free items an
    /// tpn::pool_no_allocation if thrown, otherwise a null pointer is returned.
    //*************************************************************************
    template <typename U>
    U* create()
    {
      TYPHOON_STATIC_ASSERT(tpn::alignment_of<U>::value <= VAlignment, "Type has incompatible alignment");
      TYPHOON_STATIC_ASSERT(sizeof(U) <= VTypeSize, "Type too large for pool");
      return ipool::create<U>();
    }

    //*************************************************************************
    /// Allocate storage for an object from the pool and create with 1 parameter.
    /// If asserts or exceptions are enabled and there are no more free items an
    /// tpn::pool_no_allocation if thrown, otherwise a null pointer is returned.
    //*************************************************************************
    template <typename U, typename T1>
    U* create(const T1& value1)
    {
      TYPHOON_STATIC_ASSERT(tpn::alignment_of<U>::value <= VAlignment, "Type has incompatible alignment");
      TYPHOON_STATIC_ASSERT(sizeof(U) <= VTypeSize, "Type too large for pool");
      return ipool::create<U>(value1);
    }

    //*************************************************************************
    /// Allocate storage for an object from the pool and create with 2 parameters.
    /// If asserts or exceptions are enabled and there are no more free items an
    /// tpn::pool_no_allocation if thrown, otherwise a null pointer is returned.
    //*************************************************************************
    template <typename U, typename T1, typename T2>
    U* create(const T1& value1, const T2& value2)
    {
      TYPHOON_STATIC_ASSERT(tpn::alignment_of<U>::value <= VAlignment, "Type has incompatible alignment");
      TYPHOON_STATIC_ASSERT(sizeof(U) <= VTypeSize, "Type too large for pool");
      return ipool::create<U>(value1, value2);
    }

    //*************************************************************************
    /// Allocate storage for an object from the pool and create with 3 parameters.
    /// If asserts or exceptions are enabled and there are no more free items an
    /// tpn::pool_no_allocation if thrown, otherwise a null pointer is returned.
    //*************************************************************************
    template <typename U, typename T1, typename T2, typename T3>
    U* create(const T1& value1, const T2& value2, const T3& value3)
    {
      TYPHOON_STATIC_ASSERT(tpn::alignment_of<U>::value <= VAlignment, "Type has incompatible alignment");
      TYPHOON_STATIC_ASSERT(sizeof(U) <= VTypeSize, "Type too large for pool");
      return ipool::create<U>(value1, value2, value3);
    }

    //*************************************************************************
    /// Allocate storage for an object from the pool and create with 4 parameters.
    /// If asserts or exceptions are enabled and there are no more free items an
    /// tpn::pool_no_allocation if thrown, otherwise a null pointer is returned.
    //*************************************************************************
    template <typename U, typename T1, typename T2, typename T3, typename T4>
    U* create(const T1& value1, const T2& value2, const T3& value3, const T4& value4)
    {
      TYPHOON_STATIC_ASSERT(tpn::alignment_of<U>::value <= VAlignment, "Type has incompatible alignment");
      TYPHOON_STATIC_ASSERT(sizeof(U) <= VTypeSize, "Type too large for pool");
      return ipool::create<U>(value1, value2, value3, value4);
    }
#else
    //*************************************************************************
    /// Emplace with variadic constructor parameters.
    //*************************************************************************
    template <typename U, typename... Args>
    U* create(Args&&... args)
    {
      TYPHOON_STATIC_ASSERT(tpn::alignment_of<U>::value <= VAlignment, "Type has incompatible alignment");
      TYPHOON_STATIC_ASSERT(sizeof(U) <= VTypeSize, "Type too large for pool");
      return ipool::create<U>(tpn::forward<Args>(args)...);
    }
#endif

    //*************************************************************************
    /// Destroys the object.
    /// Undefined behaviour if the pool does not contain a 'U'.
    /// \param p_object A pointer to the object to be destroyed.
    //*************************************************************************
    template <typename U>
    void destroy(const U* const p_object)
    {
      TYPHOON_STATIC_ASSERT(tpn::alignment_of<U>::value <= VAlignment, "Type has incompatible alignment");
      TYPHOON_STATIC_ASSERT(sizeof(U) <= VTypeSize, "Type too large for pool");
      p_object->~U();
      ipool::release(p_object);
    }

  private:
    // Should not be copied.
    generic_pool_ext(const generic_pool_ext&) TYPHOON_DELETE;
    generic_pool_ext& operator=(const generic_pool_ext&) TYPHOON_DELETE;
  };
}

#endif

