/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tphn
https://www.tphncpp.com

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

#ifndef TYPHOON_TYPE_LOOKUP_HPP
#define TYPHOON_TYPE_LOOKUP_HPP

#include "platform.hpp"
#include "type_traits.hpp"
#include "static_assert.hpp"
#include "integral_limits.hpp"
#include "null_type.hpp"

#include <limits.h>

#if 0
#error THIS HEADER IS A GENERATOR. DO NOT INCLUDE.
#endif

//***************************************************************************
// THIS FILE HAS BEEN AUTO GENERATED. DO NOT EDIT THIS FILE.
//***************************************************************************

namespace tphn
{
  //***************************************************************************
  /// The type/id pair type to use for type/id lookup template parameters.
  //***************************************************************************
  template <typename T, int ID_>
  struct type_id_pair
  {
    typedef T type;

    enum
    {
      ID = ID_
    };
  };

  //***************************************************************************
  /// The type/type pair type to use for type/type lookup template parameters.
  //***************************************************************************
  template <typename T1, typename T2>
  struct type_type_pair
  {
    typedef T1 type1;
    typedef T2 type2;
  };

#if TYPHOON_USING_CPP11 && !defined(TYPHOON_TYPE_SELECT_FORCE_CPP03_IMPLEMENTATION)
  //***************************************************************************
  // type_id_lookup 
  //***************************************************************************
  template <typename... TTypes>
  struct type_id_lookup
  {
  private:

    // The type for no match.
    struct nulltype {};

    // For N type pairs.
    template <size_t ID, typename T1, typename... TRest>
    struct type_from_id_helper
    {
      using type = typename tphn::conditional<ID == T1::ID,
                                             typename T1::type,
                                             typename type_from_id_helper<ID, TRest...>::type>::type;
    };

    // Specialisation for 1 type pair.
    template <size_t ID, typename T1>
    struct type_from_id_helper<ID, T1>
    {
      using type = typename tphn::conditional<ID == T1::ID,
                                             typename T1::type,
                                             nulltype>::type;
    };

  public:

    //************************************
    // type_from_id
    //************************************
    template <int ID>
    struct type_from_id
    {
      using type = typename type_from_id_helper<ID, TTypes...>::type;

      static_assert(!(tphn::is_same<nulltype, type>::value), "Invalid id");
    };

    template <int ID>
    using type_from_id_t = typename type_from_id<ID>::type;

  private:

    static constexpr size_t UNKNOWN = tphn::integral_limits<size_t>::max;

    // For N type pairs.
    template <typename T, typename T1, typename... TRest>
    struct id_from_type_helper
    {
      static constexpr size_t value = tphn::is_same<T, typename T1::type>::value ? T1::ID : id_from_type_helper<T, TRest...>::value;
    };

    // Specialisation for 1 type pair.
    template <typename T, typename T1>
    struct id_from_type_helper<T, T1>
    {
      static constexpr size_t value = tphn::is_same<T, typename T1::type>::value ? T1::ID : UNKNOWN;
    };

  public:

    //************************************
    // id_from_type
    //************************************
    template <typename T>
    struct id_from_type
    {
      static constexpr size_t value = id_from_type_helper<T, TTypes...>::value;

      static_assert(value != UNKNOWN, "Invalid type");
    };

#if TYPHOON_USING_CPP17
    template <typename T>
    static constexpr size_t id_from_type_v = id_from_type<T>::value;
#endif

    //************************************
    template <typename T>
    static unsigned int get_id_from_type(const T&)
    {
      return get_id_from_type<T>();
    }
     
    //************************************
    template <typename T>
    static unsigned int get_id_from_type()
    {
      return id_from_type<T>::value;
    }
  };

  //***************************************************************************
  // type_type_lookup
  //***************************************************************************
  template <typename... TTypes>
  class type_type_lookup
  {
  private:

    // The type for no match.
    struct nulltype {};

    template <typename T, typename T1, typename... TRest>
    struct type_from_type_helper
    {
      using type = typename tphn::conditional<tphn::is_same<T, typename T1::type1>::value,
                                             typename T1::type2,
                                             typename type_from_type_helper<T, TRest...>::type>::type;
    };

    template <typename T, typename T1>
    struct type_from_type_helper<T, T1>
    {
      using type = typename tphn::conditional<tphn::is_same<T, typename T1::type1>::value,
                                             typename T1::type2,
                                             nulltype>::type;
    };

  public:

    template <typename T>
    class type_from_type
    {
    public:

      // The matched type or nulltype
      using type = typename type_from_type_helper<T, TTypes...>::type;

      static_assert(!tphn::is_same<type, nulltype>::value, "Type match not found");
    };

    // Template alias.
    template <typename T>
    using type_from_type_t = typename type_from_type<T>::type;
  };

#else

  //***************************************************************************
  // For 16 types.
  //***************************************************************************
  template <typename T1,
            typename T2 = tphn::type_id_pair<tphn::null_type<0>, -2>,
            typename T3 = tphn::type_id_pair<tphn::null_type<0>, -3>,
            typename T4 = tphn::type_id_pair<tphn::null_type<0>, -4>,
            typename T5 = tphn::type_id_pair<tphn::null_type<0>, -5>,
            typename T6 = tphn::type_id_pair<tphn::null_type<0>, -6>,
            typename T7 = tphn::type_id_pair<tphn::null_type<0>, -7>,
            typename T8 = tphn::type_id_pair<tphn::null_type<0>, -8>,
            typename T9 = tphn::type_id_pair<tphn::null_type<0>, -9>,
            typename T10 = tphn::type_id_pair<tphn::null_type<0>, -10>,
            typename T11 = tphn::type_id_pair<tphn::null_type<0>, -11>,
            typename T12 = tphn::type_id_pair<tphn::null_type<0>, -12>,
            typename T13 = tphn::type_id_pair<tphn::null_type<0>, -13>,
            typename T14 = tphn::type_id_pair<tphn::null_type<0>, -14>,
            typename T15 = tphn::type_id_pair<tphn::null_type<0>, -15>,
            typename T16 = tphn::type_id_pair<tphn::null_type<0>, -16> >
  struct type_id_lookup
  {
  public:

    //************************************
    template <int ID>
    struct type_from_id
    {
      typedef 
            typename tphn::conditional<ID == T1::ID, typename T1::type,
            typename tphn::conditional<ID == T2::ID, typename T2::type,
            typename tphn::conditional<ID == T3::ID, typename T3::type,
            typename tphn::conditional<ID == T4::ID, typename T4::type,
            typename tphn::conditional<ID == T5::ID, typename T5::type,
            typename tphn::conditional<ID == T6::ID, typename T6::type,
            typename tphn::conditional<ID == T7::ID, typename T7::type,
            typename tphn::conditional<ID == T8::ID, typename T8::type,
            typename tphn::conditional<ID == T9::ID, typename T9::type,
            typename tphn::conditional<ID == T10::ID, typename T10::type,
            typename tphn::conditional<ID == T11::ID, typename T11::type,
            typename tphn::conditional<ID == T12::ID, typename T12::type,
            typename tphn::conditional<ID == T13::ID, typename T13::type,
            typename tphn::conditional<ID == T14::ID, typename T14::type,
            typename tphn::conditional<ID == T15::ID, typename T15::type,
            typename tphn::conditional<ID == T16::ID, typename T16::type,
            tphn::null_type<0> >::type>::type>::type>::type>
                              ::type>::type>::type>::type>
                              ::type>::type>::type>::type>
                              ::type>::type>::type>::type type;

      TYPHOON_STATIC_ASSERT(!(tphn::is_same<tphn::null_type<0>, type>::value), "Invalid id");
    };

    //************************************
    enum
    {
      UNKNOWN = UINT_MAX
    };

    template <typename T>
    struct id_from_type
    {
      enum
      {
        value =
          (unsigned int) tphn::is_same<T, typename T1::type>::value ? (unsigned int)T1::ID :
          (unsigned int) tphn::is_same<T, typename T2::type>::value ? (unsigned int)T2::ID :
          (unsigned int) tphn::is_same<T, typename T3::type>::value ? (unsigned int)T3::ID :
          (unsigned int) tphn::is_same<T, typename T4::type>::value ? (unsigned int)T4::ID :
          (unsigned int) tphn::is_same<T, typename T5::type>::value ? (unsigned int)T5::ID :
          (unsigned int) tphn::is_same<T, typename T6::type>::value ? (unsigned int)T6::ID :
          (unsigned int) tphn::is_same<T, typename T7::type>::value ? (unsigned int)T7::ID :
          (unsigned int) tphn::is_same<T, typename T8::type>::value ? (unsigned int)T8::ID :
          (unsigned int) tphn::is_same<T, typename T9::type>::value ? (unsigned int)T9::ID :
          (unsigned int) tphn::is_same<T, typename T10::type>::value ? (unsigned int)T10::ID :
          (unsigned int) tphn::is_same<T, typename T11::type>::value ? (unsigned int)T11::ID :
          (unsigned int) tphn::is_same<T, typename T12::type>::value ? (unsigned int)T12::ID :
          (unsigned int) tphn::is_same<T, typename T13::type>::value ? (unsigned int)T13::ID :
          (unsigned int) tphn::is_same<T, typename T14::type>::value ? (unsigned int)T14::ID :
          (unsigned int) tphn::is_same<T, typename T15::type>::value ? (unsigned int)T15::ID :
          (unsigned int) tphn::is_same<T, typename T16::type>::value ? (unsigned int)T16::ID :
          (unsigned int) UNKNOWN
      };

      TYPHOON_STATIC_ASSERT(((unsigned int)value != (unsigned int)UNKNOWN), "Invalid type");
    };

    //************************************
    template <typename T>
    static unsigned int get_id_from_type(const T&)
    {
      return get_id_from_type<T>();
    }

    //************************************
    template <typename T>
    static unsigned int get_id_from_type()
    {
      return id_from_type<T>::value;
    }
  };

  //***************************************************************************
  // For 16 types.
  //***************************************************************************
  template <typename T1,
            typename T2 = tphn::type_type_pair<tphn::null_type<0>, tphn::null_type<0> >,
            typename T3 = tphn::type_type_pair<tphn::null_type<0>, tphn::null_type<0> >,
            typename T4 = tphn::type_type_pair<tphn::null_type<0>, tphn::null_type<0> >,
            typename T5 = tphn::type_type_pair<tphn::null_type<0>, tphn::null_type<0> >,
            typename T6 = tphn::type_type_pair<tphn::null_type<0>, tphn::null_type<0> >,
            typename T7 = tphn::type_type_pair<tphn::null_type<0>, tphn::null_type<0> >,
            typename T8 = tphn::type_type_pair<tphn::null_type<0>, tphn::null_type<0> >,
            typename T9 = tphn::type_type_pair<tphn::null_type<0>, tphn::null_type<0> >,
            typename T10 = tphn::type_type_pair<tphn::null_type<0>, tphn::null_type<0> >,
            typename T11 = tphn::type_type_pair<tphn::null_type<0>, tphn::null_type<0> >,
            typename T12 = tphn::type_type_pair<tphn::null_type<0>, tphn::null_type<0> >,
            typename T13 = tphn::type_type_pair<tphn::null_type<0>, tphn::null_type<0> >,
            typename T14 = tphn::type_type_pair<tphn::null_type<0>, tphn::null_type<0> >,
            typename T15 = tphn::type_type_pair<tphn::null_type<0>, tphn::null_type<0> >,
            typename T16 = tphn::type_type_pair<tphn::null_type<0>, tphn::null_type<0> > >
  struct type_type_lookup
  {
  public:

    //************************************
    template <typename T>
    struct type_from_type
    {
      typedef 
            typename tphn::conditional<tphn::is_same<T, typename T1::type1>::value, typename T1::type2,
            typename tphn::conditional<tphn::is_same<T, typename T2::type1>::value, typename T2::type2,
            typename tphn::conditional<tphn::is_same<T, typename T3::type1>::value, typename T3::type2,
            typename tphn::conditional<tphn::is_same<T, typename T4::type1>::value, typename T4::type2,
            typename tphn::conditional<tphn::is_same<T, typename T5::type1>::value, typename T5::type2,
            typename tphn::conditional<tphn::is_same<T, typename T6::type1>::value, typename T6::type2,
            typename tphn::conditional<tphn::is_same<T, typename T7::type1>::value, typename T7::type2,
            typename tphn::conditional<tphn::is_same<T, typename T8::type1>::value, typename T8::type2,
            typename tphn::conditional<tphn::is_same<T, typename T9::type1>::value, typename T9::type2,
            typename tphn::conditional<tphn::is_same<T, typename T10::type1>::value, typename T10::type2,
            typename tphn::conditional<tphn::is_same<T, typename T11::type1>::value, typename T11::type2,
            typename tphn::conditional<tphn::is_same<T, typename T12::type1>::value, typename T12::type2,
            typename tphn::conditional<tphn::is_same<T, typename T13::type1>::value, typename T13::type2,
            typename tphn::conditional<tphn::is_same<T, typename T14::type1>::value, typename T14::type2,
            typename tphn::conditional<tphn::is_same<T, typename T15::type1>::value, typename T15::type2,
            typename tphn::conditional<tphn::is_same<T, typename T16::type1>::value, typename T16::type2,
            tphn::null_type<0> >::type>::type>::type>::type>::type>::type>::type>::type>
                              ::type>::type>::type>::type>::type>::type>::type>::type type;

      TYPHOON_STATIC_ASSERT(!(tphn::is_same<tphn::null_type<0>, type>::value), "Invalid type");
    };
  };

#endif
}

#endif