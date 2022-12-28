///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tphn
https://www.tphncpp.com

Copyright(c) 2021 jwellbelove, Robin S�derholm

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

#include "../platform.hpp"
#include "../utility.hpp"
#include "../largest.hpp"
#include "../exception.hpp"
#include "../type_traits.hpp"
#include "../integral_limits.hpp"
#include "../static_assert.hpp"
#include "../alignment.hpp"
#include "../error_handler.hpp"
#include "../parameter_pack.hpp"
#include "../placement_new.hpp"
#include "../visitor.hpp"
#include "../memory.hpp"
#include "../initializer_list.hpp"

#include <stdint.h>

#if defined(TYPHOON_COMPILER_KEIL)
  #pragma diag_suppress 940
  #pragma diag_suppress 111
#endif

#if TYPHOON_CPP11_NOT_SUPPORTED
  #if !defined(TYPHOON_IN_UNIT_TEST)
    #error NOT SUPPORTED FOR C++03 OR BELOW
  #endif
#else
//*****************************************************************************
///\defgroup variant variant
/// A class that can contain one a several specified types in a type safe manner.
///\ingroup containers
//*****************************************************************************

namespace tphn
{
  namespace private_variant
  {
    //***************************************************************************
    // This is a copy of the normal tphn::parameter_pack, but without the static_assert
    // so that the C++11 versions of do_accept() & do_operator() do not throw a compile time error.
    //***************************************************************************
    template <typename... TTypes>
    class parameter_pack
    {
    public:

      static constexpr size_t size = sizeof...(TTypes);

      //***************************************************************************
      /// index_of_type
      //***************************************************************************
      template <typename T>
      class index_of_type
      {
      private:

        using type = tphn::remove_cvref_t<T>;

        //***********************************
        template <typename Type, typename T1, typename... TRest>
        struct index_of_type_helper
        {
          static constexpr size_t value = tphn::is_same<Type, T1>::value ? 1 : 1 + index_of_type_helper<Type, TRest...>::value;
        };

        //***********************************
        template <typename Type, typename T1>
        struct index_of_type_helper<Type, T1>
        {
          static constexpr size_t value = 1UL;
        };

      public:

        static_assert(tphn::is_one_of<type, TTypes...>::value, "T is not in parameter pack");

        /// The index value.
        static constexpr size_t value = index_of_type_helper<type, TTypes...>::value - 1;
      };

      //***************************************************************************
      /// type_from_index
      //***************************************************************************
      template <size_t I>
      class type_from_index
      {
      private:

        //***********************************
        template <size_t II, size_t N, typename T1, typename... TRest>
        struct type_from_index_helper
        {
          using type = typename tphn::conditional<II == N, T1, typename type_from_index_helper<II, N + 1, TRest...>::type>::type;
        };

        //***********************************
        template <size_t II, size_t N, typename T1>
        struct type_from_index_helper<II, N, T1>
        {
          using type = T1;
        };

      public:

        /// Template alias
        using type = typename type_from_index_helper<I, 0, TTypes...>::type;
      };

      //***********************************
      template <size_t I>
      using type_from_index_t = typename type_from_index<I>::type;
    };

    //*******************************************
    // The traits an object may have.
    //*******************************************
    static constexpr bool Copyable     = true;
    static constexpr bool Non_Copyable = false;
    static constexpr bool Moveable     = true;
    static constexpr bool Non_Moveable = false;

    //*******************************************
    // The types of operations we can perform.
    //*******************************************
    static constexpr int Copy    = 0;
    static constexpr int Move    = 1;
    static constexpr int Destroy = 2;

    //*******************************************
    // operation_type
    //*******************************************
    template <typename T, bool IsCopyable, bool IsMoveable>
    struct operation_type;

    //*******************************************
    // Specialisation for null operation.
    template <>
    struct operation_type<void, Non_Copyable, Non_Moveable>
    {
      static void do_operation(int , char* , const char* )
      {
        // This should never occur.
#if defined(TYPHOON_IN_UNIT_TEST)
        assert(false);
#endif
      }
    };

    //*******************************************
    // Specialisation for no-copyable & non-moveable types.
    template <typename T>
    struct operation_type<T, Non_Copyable, Non_Moveable>
    {
      static void do_operation(int operation, char* pstorage, const char* /*pvalue*/)
      {
        switch (operation)
        {
          case Destroy:
          {
            reinterpret_cast<const T*>(pstorage)->~T();
            break;
          }

          default:
          {
            // This should never occur.
  #if defined(TYPHOON_IN_UNIT_TEST)
            assert(false);
  #endif
            break;
          }
        }
      }
    };

    //*******************************************
    // Specialisation for no-copyable & moveable types.
    template <typename T>
    struct operation_type<T, Non_Copyable, Moveable>
    {
      static void do_operation(int operation, char* pstorage, const char* pvalue)
      {
        switch (operation)
        {
          case Move:
          {
            ::new (pstorage) T(tphn::move(*reinterpret_cast<T*>(const_cast<char*>(pvalue))));
            break;
          }

          case Destroy:
          {
            reinterpret_cast<const T*>(pstorage)->~T();
            break;
          }

          default:
          {
            // This should never occur.
  #if defined(TYPHOON_IN_UNIT_TEST)
            assert(false);
  #endif
            break;
          }
        }
      }
    };

    //*******************************************
    // Specialisation for copyable & non-moveable types.
    template <typename T>
    struct operation_type<T, Copyable, Non_Moveable>
    {
      static void do_operation(int operation, char* pstorage, const char* pvalue)
      {
        switch (operation)
        {
          case Copy:
          {
            ::new (pstorage) T(*reinterpret_cast<const T*>(pvalue));
            break;
          }

          case Destroy:
          {
            reinterpret_cast<const T*>(pstorage)->~T();
            break;
          }

          default:
          {
            // This should never occur.
  #if defined(TYPHOON_IN_UNIT_TEST)
            assert(false);
  #endif
            break;
          }
        }
      }
    };

    //*******************************************
    // Specialisation for copyable & moveable types.
    template <typename T>
    struct operation_type<T, Copyable, Moveable>
    {
      static void do_operation(int operation, char* pstorage, const char* pvalue)
      {
        switch (operation)
        {
          case Copy:
          {
            ::new (pstorage) T(*reinterpret_cast<const T*>(pvalue));
            break;
          }

          case Move:
          {
            ::new (pstorage) T(tphn::move(*reinterpret_cast<T*>(const_cast<char*>(pvalue))));
            break;
          }

          case Destroy:
          {
            reinterpret_cast<const T*>(pstorage)->~T();
            break;
          }

          default:
          {
            // This should never occur.
#if defined(TYPHOON_IN_UNIT_TEST)
            assert(false);
#endif
            break;
          }
        }
      }
    };
  }

  /// Definition of variant_npos.
  constexpr size_t variant_npos = tphn::integral_limits<size_t>::max;

  //***********************************
  // variant. Forward declaration
  template <typename... TTypes>
  class variant;

  //***************************************************************************
  /// variant_alternative
  //***************************************************************************
  template <size_t Index, typename T>
  struct variant_alternative;

  template <size_t Index, typename... TTypes>
  struct variant_alternative<Index, tphn::variant<TTypes...>>
  {
    using type = typename tphn::private_variant::parameter_pack<TTypes...>::template type_from_index<Index>::type;
  };

  template <size_t Index, typename T>
  struct variant_alternative<Index, const T>
  {
    using type = typename variant_alternative<Index, T>::type;
  };

  template <size_t Index, typename T>
  using variant_alternative_t = typename variant_alternative<Index, T>::type;

  //***********************************
  // holds_alternative. Forward declaration
  template <typename T, typename... TTypes>
  TYPHOON_CONSTEXPR14 bool holds_alternative(const tphn::variant<TTypes...>& v) noexcept;

  //***********************************
  // get. Forward declarations
  template <size_t Index, typename... VTypes>
  TYPHOON_CONSTEXPR14 tphn::variant_alternative_t<Index, tphn::variant<VTypes...>>&
    get(tphn::variant<VTypes...>& v);

  template <size_t Index, typename... VTypes>
  TYPHOON_CONSTEXPR14 tphn::variant_alternative_t<Index, tphn::variant<VTypes...>>&&
    get(tphn::variant<VTypes...>&& v);

  template <size_t Index, typename... VTypes>
  TYPHOON_CONSTEXPR14 const tphn::variant_alternative_t<Index, const tphn::variant<VTypes...>>&
    get(const tphn::variant<VTypes...>& v);

  template <size_t Index, typename... VTypes>
  TYPHOON_CONSTEXPR14 const tphn::variant_alternative_t<Index, const tphn::variant<VTypes...>>&&
    get(const tphn::variant<VTypes...>&& v);

  template <typename T, typename... VTypes>
  TYPHOON_CONSTEXPR14 T& get(tphn::variant<VTypes...>& v);

  template <typename T, typename... VTypes>
  TYPHOON_CONSTEXPR14 T&& get(tphn::variant<VTypes...>&& v);

  template <typename T, typename... VTypes>
  TYPHOON_CONSTEXPR14 const T& get(const tphn::variant<VTypes...>& v);

  template <typename T, typename... VTypes>
  TYPHOON_CONSTEXPR14 const T&& get(const tphn::variant<VTypes...>&& v);

  //***************************************************************************
  /// Monostate for variants.
  ///\ingroup variant
  //***************************************************************************
  struct monostate
  {
  };

  constexpr bool operator >(tphn::monostate, tphn::monostate) noexcept { return false; }
	constexpr bool operator <(tphn::monostate, tphn::monostate) noexcept { return false; }
	constexpr bool operator !=(tphn::monostate, tphn::monostate) noexcept { return false; }
	constexpr bool operator <=(tphn::monostate, tphn::monostate) noexcept { return true; }
	constexpr bool operator >=(tphn::monostate, tphn::monostate) noexcept { return true; }
	constexpr bool operator ==(tphn::monostate, tphn::monostate) noexcept { return true; }

#if TYPHOON_NOT_USING_STL && !defined(TYPHOON_USE_TYPE_TRAITS_BUILTINS)
  template <>
  struct is_copy_constructible<tphn::monostate> : public tphn::true_type
  {
  };

  template <>
  struct is_move_constructible<tphn::monostate> : public tphn::true_type
  {
  };
#endif

  //***************************************************************************
  /// Base exception for the variant class.
  ///\ingroup variant
  //***************************************************************************
  class variant_exception : public exception
  {
  public:
    variant_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// 'Unsupported type' exception for the variant class.
  ///\ingroup variant
  //***************************************************************************
  class variant_incorrect_type_exception : public variant_exception
  {
  public:
    variant_incorrect_type_exception(string_type file_name_, numeric_type line_number_)
      : variant_exception(TYPHOON_ERROR_TEXT("variant:unsupported type", TYPHOON_VARIANT_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// 'Bad variant access' exception for the variant class.
  ///\ingroup variant
  //***************************************************************************
  class bad_variant_access : public variant_exception 
  {
  public:
    bad_variant_access(string_type file_name_, numeric_type line_number_)
    : variant_exception(TYPHOON_ERROR_TEXT("variant:bad variant access", TYPHOON_VARIANT_FILE_ID"B"), file_name_, line_number_)
    {}
  };

  //***************************************************************************
  /// A template class that can store any of the types defined in the template parameter list.
  /// Supports up to 8 types.
  ///\ingroup variant
  //***************************************************************************
  template <typename... TTypes>
  class variant
  {
  public:

    //***************************************************************************
    /// The type used for ids.
    //***************************************************************************
    using type_id_t = uint_least8_t ;

    //***************************************************************************
    /// get() is a friend function.
    //***************************************************************************
    template <size_t Index, typename... VTypes>
    friend TYPHOON_CONSTEXPR14 tphn::variant_alternative_t<Index, tphn::variant<VTypes...>>&
      get(tphn::variant<VTypes...>& v);

    template <size_t Index, typename... VTypes>
    friend TYPHOON_CONSTEXPR14 tphn::variant_alternative_t<Index, tphn::variant<VTypes...>>&&
      get(tphn::variant<VTypes...>&& v);

    template <size_t Index, typename... VTypes>
    friend TYPHOON_CONSTEXPR14 const tphn::variant_alternative_t<Index, const tphn::variant<VTypes...>>&
      get(const tphn::variant<VTypes...>& v);

    template <size_t Index, typename... VTypes>
    friend TYPHOON_CONSTEXPR14 const tphn::variant_alternative_t<Index, const tphn::variant<VTypes...>>&&
      get(const tphn::variant<VTypes...>&& v);

    template <typename T, typename... VTypes>
    friend TYPHOON_CONSTEXPR14 T& get(tphn::variant<VTypes...>& v);

    template <typename T, typename... VTypes>
    friend TYPHOON_CONSTEXPR14 T&& get(tphn::variant<VTypes...>&& v);

    template <typename T, typename... VTypes>
    friend TYPHOON_CONSTEXPR14 const T& get(const tphn::variant<VTypes...>& v);

    template <typename T, typename... VTypes>
    friend TYPHOON_CONSTEXPR14 const T&& get(const tphn::variant<VTypes...>&& v);

  private:

    // All types of variant are friends.
    template <typename... UTypes>
    friend class variant;

    //***************************************************************************
    /// The largest type.
    //***************************************************************************
    using largest_t = typename largest_type<TTypes...>::type;

    //***************************************************************************
    /// The largest size.
    //***************************************************************************
    static const size_t Size = sizeof(largest_t);

    //***************************************************************************
    /// The largest alignment.
    //***************************************************************************
    static const size_t Alignment = tphn::largest_alignment<TTypes...>::value;

    //***************************************************************************
    /// The operation templates.
    //***************************************************************************
    template <typename T, bool IsCopyable, bool IsMoveable>
    using operation_type = private_variant::operation_type<T, IsCopyable, IsMoveable>;

    //*******************************************
    // The types of operations we can perform.
    //*******************************************
    static constexpr int Copy    = private_variant::Copy;
    static constexpr int Move    = private_variant::Move;
    static constexpr int Destroy = private_variant::Destroy;

  public:

    //***************************************************************************
    /// Default constructor.
    /// Sets the state of the instance to containing no valid data.
    //***************************************************************************
#include "tphn/private/diagnostic_uninitialized_push.hpp"
    TYPHOON_CONSTEXPR14 variant()
    {
      using type = typename tphn::private_variant::parameter_pack<TTypes...>::template type_from_index<0U>::type;

      default_construct_in_place<type>(data);
      operation = operation_type<type, tphn::is_copy_constructible<type>::value, tphn::is_move_constructible<type>::value>::do_operation;
      type_id   = 0U;
    }
#include "tphn/private/diagnostic_pop.hpp"

    //***************************************************************************
    /// Constructor from a value.
    //***************************************************************************
#include "tphn/private/diagnostic_uninitialized_push.hpp"
    template <typename T, tphn::enable_if_t<!tphn::is_same<tphn::remove_cvref_t<T>, variant>::value, int> = 0>
    TYPHOON_CONSTEXPR14 variant(T&& value)
      : operation(operation_type<tphn::remove_cvref_t<T>, tphn::is_copy_constructible<tphn::remove_cvref_t<T>>::value, tphn::is_move_constructible<tphn::remove_cvref_t<T>>::value>::do_operation)
      , type_id(tphn::private_variant::parameter_pack<TTypes...>::template index_of_type<tphn::remove_cvref_t<T>>::value)
    {
      static_assert(tphn::is_one_of<tphn::remove_cvref_t<T>, TTypes...>::value, "Unsupported type");

      construct_in_place<tphn::remove_cvref_t<T>>(data, tphn::forward<T>(value));
    }
#include "tphn/private/diagnostic_pop.hpp"

    //***************************************************************************
    /// Construct from arguments.
    //***************************************************************************
#include "tphn/private/diagnostic_uninitialized_push.hpp"
    template <typename T, typename... TArgs>
    TYPHOON_CONSTEXPR14 explicit variant(tphn::in_place_type_t<T>, TArgs&&... args)
      : operation(operation_type<tphn::remove_cvref_t<T>, tphn::is_copy_constructible<tphn::remove_cvref_t<T>>::value, tphn::is_move_constructible<tphn::remove_cvref_t<T>>::value>::do_operation)
      , type_id(tphn::private_variant::parameter_pack<TTypes...>::template index_of_type<tphn::remove_cvref_t<T>>::value)
    {
      static_assert(tphn::is_one_of<tphn::remove_cvref_t<T>, TTypes...>::value, "Unsupported type");

      construct_in_place_args<tphn::remove_cvref_t<T>>(data, tphn::forward<TArgs>(args)...);
    }
#include "tphn/private/diagnostic_pop.hpp"

    //***************************************************************************
    /// Construct from arguments.
    //***************************************************************************
#include "tphn/private/diagnostic_uninitialized_push.hpp"
    template <size_t Index, typename... TArgs>
    TYPHOON_CONSTEXPR14 explicit variant(tphn::in_place_index_t<Index>, TArgs&&... args)
      : type_id(Index)
    {
      using type = typename private_variant::parameter_pack<TTypes...>:: template type_from_index_t<Index>;
      static_assert(tphn::is_one_of<type, TTypes...> ::value, "Unsupported type");

      construct_in_place_args<type>(data, tphn::forward<TArgs>(args)...);

      operation = operation_type<type, tphn::is_copy_constructible<type>::value, tphn::is_move_constructible<type>::value>::do_operation;
    }
#include "tphn/private/diagnostic_pop.hpp"

#if TYPHOON_HAS_INITIALIZER_LIST
    //***************************************************************************
    /// Construct from type, initializer_list and arguments.
    //***************************************************************************
#include "tphn/private/diagnostic_uninitialized_push.hpp"
    template <typename T, typename U, typename... TArgs >
    TYPHOON_CONSTEXPR14 explicit variant(tphn::in_place_type_t<T>, std::initializer_list<U> init, TArgs&&... args)
      : operation(operation_type<tphn::remove_cvref_t<T>, tphn::is_copy_constructible<tphn::remove_cvref_t<T>>::value, tphn::is_move_constructible<tphn::remove_cvref_t<T>>::value>::do_operation)
      , type_id(private_variant::parameter_pack<TTypes...>:: template index_of_type<tphn::remove_cvref_t<T>>::value)
    {
      static_assert(tphn::is_one_of<tphn::remove_cvref_t<T>, TTypes...> ::value, "Unsupported type");

      construct_in_place_args<tphn::remove_cvref_t<T>>(data, init, tphn::forward<TArgs>(args)...);
    }
#include "tphn/private/diagnostic_pop.hpp"

    //***************************************************************************
    /// Construct from index, initializer_list and arguments.
    //***************************************************************************
#include "tphn/private/diagnostic_uninitialized_push.hpp"
    template <size_t Index, typename U, typename... TArgs >
    TYPHOON_CONSTEXPR14 explicit variant(tphn::in_place_index_t<Index>, std::initializer_list<U> init, TArgs&&... args)
      : type_id(Index)
    {
      using type = typename private_variant::parameter_pack<TTypes...>:: template type_from_index_t<Index>;
      static_assert(tphn::is_one_of<type, TTypes...> ::value, "Unsupported type");

      construct_in_place_args<type>(data, init, tphn::forward<TArgs>(args)...);

      operation = operation_type<type, tphn::is_copy_constructible<type>::value, tphn::is_move_constructible<type>::value>::do_operation;
    }
#include "tphn/private/diagnostic_pop.hpp"
#endif

    //***************************************************************************
    /// Copy constructor.
    ///\param other The other variant object to copy.
    //***************************************************************************
#include "tphn/private/diagnostic_uninitialized_push.hpp"
    TYPHOON_CONSTEXPR14 variant(const variant& other)
      : operation(other.operation)
      , type_id(other.type_id)
    {
      if (this != &other)
      {
        if (other.index() == variant_npos)
        {
          type_id = variant_npos;
        }
        else
        {
          operation(private_variant::Copy, data, other.data);
        }
      }
    }
#include "tphn/private/diagnostic_pop.hpp"

    //***************************************************************************
    /// Move constructor.
    ///\param other The other variant object to copy.
    //***************************************************************************
#include "tphn/private/diagnostic_uninitialized_push.hpp"
    TYPHOON_CONSTEXPR14 variant(variant&& other)
      : operation(other.operation)
      , type_id(other.type_id)
    {
      if (this != &other)
      {
        if (other.index() == variant_npos)
        {
          type_id = variant_npos;
        }
        else
        {
          operation(private_variant::Move, data, other.data);
        }
      }
      else
      {
        type_id = variant_npos;
      }
    }
#include "tphn/private/diagnostic_pop.hpp"

    //***************************************************************************
    /// Destructor.
    //***************************************************************************
    ~variant()
    {
      if (index() != variant_npos)
      {
        operation(private_variant::Destroy, data, nullptr);
      }

      operation = operation_type<void, false, false>::do_operation; // Null operation.
      type_id = variant_npos;
    }

    //***************************************************************************
    /// Emplace with variadic constructor parameters.
    //***************************************************************************
    template <typename T, typename... TArgs>
    T& emplace(TArgs&&... args)
    {
      static_assert(tphn::is_one_of<T, TTypes...>::value, "Unsupported type");

      using type = tphn::remove_cvref_t<T>;

      operation(private_variant::Destroy, data, nullptr);

      construct_in_place_args<type>(data, tphn::forward<TArgs>(args)...);

      operation = operation_type<type, tphn::is_copy_constructible<type>::value, tphn::is_move_constructible<type>::value>::do_operation;

      type_id = tphn::private_variant::parameter_pack<TTypes...>::template index_of_type<T>::value;

      return *static_cast<T*>(data);
    }

    //***************************************************************************
    /// Move assignment operator for type.
    ///\param value The value to assign.
    //***************************************************************************
    template <typename T, tphn::enable_if_t<!tphn::is_same<tphn::remove_cvref_t<T>, variant>::value, int> = 0>
    variant& operator =(T&& value)
    {
      using type = tphn::remove_cvref_t<T>;

      static_assert(tphn::is_one_of<type, TTypes...>::value, "Unsupported type");

      operation(private_variant::Destroy, data, nullptr);

      construct_in_place<type>(data, tphn::forward<T>(value));

      operation = operation_type<type, tphn::is_copy_constructible<type>::value, tphn::is_move_constructible<type>::value>::do_operation;
      type_id   = tphn::private_variant::parameter_pack<TTypes...>::template index_of_type<type>::value;

      return *this;
    }

    //***************************************************************************
    /// Assignment operator for variant type.
    ///\param other The variant to assign.
    //***************************************************************************
    variant& operator =(const variant& other)
    {
      if (this != &other)
      {
        if (other.index() == variant_npos)
        {
          type_id = variant_npos;
        }
        else
        {
          operation(Destroy, data, nullptr);

          operation = other.operation;
          operation(Copy, data, other.data);

          type_id = other.type_id;
        }
      }

      return *this;
    }

    //***************************************************************************
    /// Assignment operator for variant type.
    ///\param other The variant to assign.
    //***************************************************************************
    variant& operator =(variant&& other)
    {
      if (this != &other)
      {
        if (other.index() == variant_npos)
        {
          type_id = variant_npos;
        }
        else
        {
          operation(Destroy, data, nullptr);

          operation = other.operation;
          operation(Move, data, other.data);

          type_id = other.type_id;
        }
      }

      return *this;
    }

    //***************************************************************************
    /// Checks whether a valid value is currently stored.
    ///\return <b>true</b> if the value is valid, otherwise <b>false</b>.
    //***************************************************************************
    constexpr bool valueless_by_exception() const noexcept
    {
      return type_id == variant_npos;
    }

    //***************************************************************************
    /// Gets the index of the type currently stored or variant_npos
    //***************************************************************************
    constexpr size_t index() const noexcept
    {
      return type_id;
    }

    //***************************************************************************
    /// Swaps this variant with another.
    //***************************************************************************
    void swap(variant& rhs) noexcept
    {
      variant temp(tphn::move(*this));
      *this = tphn::move(rhs);
      rhs = tphn::move(temp);
    }

    //***************************************************************************
    /// Accept an tphn::visitor.
    //***************************************************************************
    void accept_visitor(tphn::visitor<TTypes...>& v)
    {
#if TYPHOON_USING_CPP17 && !defined(TYPHOON_VARIANT_FORCE_CPP11)
      do_accept(v, tphn::make_index_sequence<sizeof...(TTypes)>{});
#else
      do_accept(v);
#endif
    }

    //***************************************************************************
    /// Accept a generic functor.
    //***************************************************************************
    template <typename TVisitor>
    void accept_functor(TVisitor& v)
    {
#if TYPHOON_USING_CPP17 && !defined(TYPHOON_VARIANT_FORCE_CPP11)
      do_operator(v, tphn::make_index_sequence<sizeof...(TTypes)>{});
#else
      do_operator(v);
#endif
    }

  private:

    /// The operation function type.
    using operation_function = void(*)(int, char*, const char*);

    //***************************************************************************
    /// Construct the type in-place. lvalue reference.
    //***************************************************************************
    template <typename T>
    static void construct_in_place(char* pstorage, const T& value)
    {
      using type = tphn::remove_cvref_t<T>;

      ::new (pstorage) type(value);
    }

    //***************************************************************************
    /// Construct the type in-place. rvalue reference.
    //***************************************************************************
    template <typename T>
    static void construct_in_place(char* pstorage, T&& value)
    {
      using type = tphn::remove_cvref_t<T>;

      ::new (pstorage) type(tphn::move(value));
    }

    //***************************************************************************
    /// Construct the type in-place. Variadic args.
    //***************************************************************************
    template <typename T, typename... TArgs>
    static void construct_in_place_args(char* pstorage, TArgs&&... args)
    {
      using type = tphn::remove_cvref_t<T>;

      ::new (pstorage) type(tphn::forward<TArgs>(args)...);
    }

    //***************************************************************************
    /// Default construct the type in-place.
    //***************************************************************************
    template <typename T>
    static void default_construct_in_place(char* pstorage)
    {
      using type = tphn::remove_cvref_t<T>;

      ::new (pstorage) type();
    }

#if TYPHOON_USING_CPP17 && !defined(TYPHOON_VARIANT_FORCE_CPP11)
    //***************************************************************************
    /// Call the relevent visitor by attempting each one.
    //***************************************************************************
    template <size_t... I>
    void do_accept(tphn::visitor<TTypes...>& visitor, tphn::index_sequence<I...>)
    {
      (attempt_visitor<I>(visitor) || ...);
    }
#else
    //***************************************************************************
    /// /// Call the relevent visitor.
    //***************************************************************************
    void do_accept(tphn::visitor<TTypes...>& visitor)
    {
      switch (index())
      {
        case 0: visitor.visit(tphn::get<0>(*this)); break;
        case 1: visitor.visit(tphn::get<1>(*this)); break;
        case 2: visitor.visit(tphn::get<2>(*this)); break;
        case 3: visitor.visit(tphn::get<3>(*this)); break;
        case 4: visitor.visit(tphn::get<4>(*this)); break;
        case 5: visitor.visit(tphn::get<5>(*this)); break;
        case 6: visitor.visit(tphn::get<6>(*this)); break;
        case 7: visitor.visit(tphn::get<7>(*this)); break;
#if !defined(TYPHOON_VARIANT_CPP11_MAX_8_TYPES)
        case 8: visitor.visit(tphn::get<8>(*this)); break;
        case 9: visitor.visit(tphn::get<9>(*this)); break;
        case 10: visitor.visit(tphn::get<10>(*this)); break;
        case 11: visitor.visit(tphn::get<11>(*this)); break;
        case 12: visitor.visit(tphn::get<12>(*this)); break;
        case 13: visitor.visit(tphn::get<13>(*this)); break;
        case 14: visitor.visit(tphn::get<14>(*this)); break;
        case 15: visitor.visit(tphn::get<15>(*this)); break;
#if !defined(TYPHOON_VARIANT_CPP11_MAX_16_TYPES)
        case 16: visitor.visit(tphn::get<16>(*this)); break;
        case 17: visitor.visit(tphn::get<17>(*this)); break;
        case 18: visitor.visit(tphn::get<18>(*this)); break;
        case 19: visitor.visit(tphn::get<19>(*this)); break;
        case 20: visitor.visit(tphn::get<20>(*this)); break;
        case 21: visitor.visit(tphn::get<21>(*this)); break;
        case 22: visitor.visit(tphn::get<22>(*this)); break;
        case 23: visitor.visit(tphn::get<23>(*this)); break;
#if !defined(TYPHOON_VARIANT_CPP11_MAX_24_TYPES)
        case 24: visitor.visit(tphn::get<24>(*this)); break;
        case 25: visitor.visit(tphn::get<25>(*this)); break;
        case 26: visitor.visit(tphn::get<26>(*this)); break;
        case 27: visitor.visit(tphn::get<27>(*this)); break;
        case 28: visitor.visit(tphn::get<28>(*this)); break;
        case 29: visitor.visit(tphn::get<29>(*this)); break;
        case 30: visitor.visit(tphn::get<30>(*this)); break;
        case 31: visitor.visit(tphn::get<31>(*this)); break;
#endif
#endif
#endif
        default: break;
      }
    }
#endif

    //***************************************************************************
    /// Attempt to call a visitor.
    //***************************************************************************
    template <size_t Index>
    bool attempt_visitor(tphn::visitor<TTypes...>& visitor)
    {
      if (Index == index())
      {
        visitor.visit(tphn::get<Index>(*this));
        return true;
      }
      else
      {
        return false;
      }
    }

#if TYPHOON_USING_CPP17 && !defined(TYPHOON_VARIANT_FORCE_CPP11)
    //***************************************************************************
    /// Call the relevent visitor by attempting each one.
    //***************************************************************************
    template <typename TVisitor, size_t... I>
    void do_operator(TVisitor& visitor, tphn::index_sequence<I...>)
    {
      (attempt_operator<I>(visitor) || ...);
    }
#else
    //***************************************************************************
    /// Call the relevent visitor.
    //***************************************************************************
    template <typename TVisitor>
    void do_operator(TVisitor& visitor)
    {
#if defined(TYPHOON_VARIANT_CPP11_MAX_8_TYPES)
      TYPHOON_STATIC_ASSERT(sizeof...(TTypes) <= 8U, "TYPHOON_VARIANT_CPP11_MAX_8_TYPES - Only a maximum of 8 types are allowed in this variant");
#endif

#if defined(TYPHOON_VARIANT_CPP11_MAX_16_TYPES)
      TYPHOON_STATIC_ASSERT(sizeof...(TTypes) <= 16U, "TYPHOON_VARIANT_CPP11_MAX_16_TYPES - Only a maximum of 16 types are allowed in this variant");
#endif

#if defined(TYPHOON_VARIANT_CPP11_MAX_24_TYPES)
      TYPHOON_STATIC_ASSERT(sizeof...(TTypes) <= 24U, "TYPHOON_VARIANT_CPP11_MAX_24_TYPES - Only a maximum of 24 types are allowed in this variant");
#endif

      TYPHOON_STATIC_ASSERT(sizeof...(TTypes) <= 32U, "A maximum of 32 types are allowed in this variant");

      switch (index())
      {
        case 0: visitor(tphn::get<0>(*this)); break;
        case 1: visitor(tphn::get<1>(*this)); break;
        case 2: visitor(tphn::get<2>(*this)); break;
        case 3: visitor(tphn::get<3>(*this)); break;
        case 4: visitor(tphn::get<4>(*this)); break;
        case 5: visitor(tphn::get<5>(*this)); break;
        case 6: visitor(tphn::get<6>(*this)); break;
        case 7: visitor(tphn::get<7>(*this)); break;
#if !defined(TYPHOON_VARIANT_CPP11_MAX_8_TYPES)
        case 8: visitor(tphn::get<8>(*this)); break;
        case 9: visitor(tphn::get<9>(*this)); break;
        case 10: visitor(tphn::get<10>(*this)); break;
        case 11: visitor(tphn::get<11>(*this)); break;
        case 12: visitor(tphn::get<12>(*this)); break;
        case 13: visitor(tphn::get<13>(*this)); break;
        case 14: visitor(tphn::get<14>(*this)); break;
        case 15: visitor(tphn::get<15>(*this)); break;
#if !defined(TYPHOON_VARIANT_CPP11_MAX_16_TYPES)
        case 16: visitor(tphn::get<16>(*this)); break;
        case 17: visitor(tphn::get<17>(*this)); break;
        case 18: visitor(tphn::get<18>(*this)); break;
        case 19: visitor(tphn::get<19>(*this)); break;
        case 20: visitor(tphn::get<20>(*this)); break;
        case 21: visitor(tphn::get<21>(*this)); break;
        case 22: visitor(tphn::get<22>(*this)); break;
        case 23: visitor(tphn::get<23>(*this)); break;
#if !defined(TYPHOON_VARIANT_CPP11_MAX_24_TYPES)
        case 24: visitor(tphn::get<24>(*this)); break;
        case 25: visitor(tphn::get<25>(*this)); break;
        case 26: visitor(tphn::get<26>(*this)); break;
        case 27: visitor(tphn::get<27>(*this)); break;
        case 28: visitor(tphn::get<28>(*this)); break;
        case 29: visitor(tphn::get<29>(*this)); break;
        case 30: visitor(tphn::get<30>(*this)); break;
        case 31: visitor(tphn::get<31>(*this)); break;
#endif
#endif
#endif
        default: break;
      }
    }
#endif

    //***************************************************************************
    /// Attempt to call a visitor.
    //***************************************************************************
    template <size_t Index, typename TVisitor>
    bool attempt_operator(TVisitor& visitor)
    {
      if (Index == index())
      {
        visitor(tphn::get<Index>(*this));
        return true;
      }
      else
      {
        return false;
      }
    }

    //***************************************************************************
    /// The internal storage.
    /// Aligned on a suitable boundary, which should be good for all types.
    //***************************************************************************
    tphn::uninitialized_buffer<Size, 1U, Alignment> data;

    //***************************************************************************
    /// The operation function.
    //***************************************************************************
    operation_function operation;

    //***************************************************************************
    /// The id of the current stored type.
    //***************************************************************************
    size_t type_id;
  };

  //***************************************************************************
  /// Checks if the variant v holds the alternative T.
  //***************************************************************************
	template <typename T, typename... TTypes>
	TYPHOON_CONSTEXPR14 bool holds_alternative(const tphn::variant<TTypes...>& v) noexcept
	{
    constexpr size_t Index = tphn::private_variant::parameter_pack<TTypes...>::template index_of_type<T>::value;

    return (Index == variant_npos) ? false : (v.index() == Index);
	}

  //***************************************************************************
  /// Checks if the variant v holds the alternative Index.
  //***************************************************************************
  template <size_t Index, typename... TTypes>
  TYPHOON_CONSTEXPR14 bool holds_alternative(const tphn::variant<TTypes...>& v) noexcept
  {
    return (Index == v.index());
  }

  //***************************************************************************
  /// Checks if the variant v holds the alternative Index. (Runtime)
  //***************************************************************************
  template <typename... TTypes>
  TYPHOON_CONSTEXPR14 bool holds_alternative(size_t index, const tphn::variant<TTypes...>& v) noexcept
  {
    return (index == v.index());
  }

  //***************************************************************************
  /// get
  //***************************************************************************
  template <size_t Index, typename... TTypes>
  TYPHOON_CONSTEXPR14 tphn::variant_alternative_t<Index, tphn::variant<TTypes...>>&
    get(tphn::variant<TTypes...>& v)
  {
#if TYPHOON_USING_CPP17 && !defined(TYPHOON_VARIANT_FORCE_CPP11)
    static_assert(Index < sizeof...(TTypes), "Index out of range");
#endif

    TYPHOON_ASSERT(Index == v.index(), TYPHOON_ERROR(tphn::variant_incorrect_type_exception));

		using type = tphn::variant_alternative_t<Index, tphn::variant<TTypes...>>;

    return *static_cast<type*>(v.data);
  }

  //***********************************
  template <size_t Index, typename... TTypes>
  TYPHOON_CONSTEXPR14 tphn::variant_alternative_t<Index, tphn::variant<TTypes...>>&&
    get(tphn::variant<TTypes...>&& v)
  {
#if TYPHOON_USING_CPP17 && !defined(TYPHOON_VARIANT_FORCE_CPP11)
    static_assert(Index < sizeof...(TTypes), "Index out of range");
#endif

		using type = tphn::variant_alternative_t<Index, tphn::variant<TTypes...>>;

    return tphn::move(*static_cast<type*>(v.data));
  }

  //***********************************
  template <size_t Index, typename... TTypes>
  TYPHOON_CONSTEXPR14 const tphn::variant_alternative_t<Index, const tphn::variant<TTypes...>>&
    get(const tphn::variant<TTypes...>& v)
  {
#if TYPHOON_USING_CPP17 && !defined(TYPHOON_VARIANT_FORCE_CPP11)
    static_assert(Index < sizeof...(TTypes), "Index out of range");
#endif

    TYPHOON_ASSERT(Index == v.index(), TYPHOON_ERROR(tphn::variant_incorrect_type_exception));

		using type = tphn::variant_alternative_t<Index, tphn::variant<TTypes...>>;

    return *static_cast<const type*>(v.data);
  }

  //***********************************
  template <size_t Index, typename... TTypes>
  TYPHOON_CONSTEXPR14 const tphn::variant_alternative_t<Index, const tphn::variant<TTypes...>>&&
    get(const tphn::variant<TTypes...>&& v)
  {
#if TYPHOON_USING_CPP17 & !defined(TYPHOON_VARIANT_FORCE_CPP11)
    static_assert(Index < sizeof...(TTypes), "Index out of range");
#endif

    TYPHOON_ASSERT(Index == v.index(), TYPHOON_ERROR(tphn::variant_incorrect_type_exception));

		using type = tphn::variant_alternative_t<Index, tphn::variant<TTypes...>>;

    return tphn::move(*static_cast<const type*>(v.data));
  }

  //***********************************
  template <typename T, typename... TTypes>
  TYPHOON_CONSTEXPR14 T& get(tphn::variant<TTypes...>& v)
  {
    constexpr size_t Index = tphn::private_variant::parameter_pack<TTypes...>::template index_of_type<T>::value;

    return get<Index>(v);
  }

  //***********************************
  template <typename T, typename... TTypes>
  TYPHOON_CONSTEXPR14 T&& get(tphn::variant<TTypes...>&& v)
  {
    constexpr size_t Index = tphn::private_variant::parameter_pack<TTypes...>::template index_of_type<T>::value;

    return get<Index>(tphn::move(v));
  }

  //***********************************
  template <typename T, typename... TTypes>
  TYPHOON_CONSTEXPR14 const T& get(const tphn::variant<TTypes...>& v)
  {
    constexpr size_t Index = tphn::private_variant::parameter_pack<TTypes...>::template index_of_type<T>::value;

    return get<Index>(v);
  }

  //***********************************
  template <typename T, typename... TTypes>
  TYPHOON_CONSTEXPR14 const T&& get(const tphn::variant<TTypes...>&& v)
  {
    constexpr size_t Index = tphn::private_variant::parameter_pack<TTypes...>::template index_of_type<T>::value;

    return get<Index>(tphn::move(v));
  }

  //***************************************************************************
  /// get_if
  //***************************************************************************
  template< size_t Index, typename... TTypes >
  TYPHOON_CONSTEXPR14 tphn::add_pointer_t<tphn::variant_alternative_t<Index, tphn::variant<TTypes...>>>
    get_if(tphn::variant<TTypes...>* pv) noexcept
  {
    if ((pv != nullptr) && (pv->index() == Index))
    {
      return &tphn::get<Index>(*pv);
    }
    else
    {
      return nullptr;
    }
  }

  //***********************************
  template< size_t Index, typename... TTypes >
  TYPHOON_CONSTEXPR14 tphn::add_pointer_t<const tphn::variant_alternative_t<Index, tphn::variant<TTypes...>>>
    get_if(const tphn::variant<TTypes...>* pv) noexcept
  {
    if ((pv != nullptr) && (pv->index() == Index))
    {
      return &tphn::get<Index>(*pv);
    }
    else
    {
      return nullptr;
    }
  }

  //***********************************
  template< class T, typename... TTypes >
  TYPHOON_CONSTEXPR14 tphn::add_pointer_t<T> get_if(tphn::variant<TTypes...>* pv) noexcept
  {
    constexpr size_t Index = tphn::private_variant::parameter_pack<TTypes...>::template index_of_type<T>::value;

    if ((pv != nullptr) && (pv->index() == Index))
    {
      return &tphn::get<Index>(*pv);
    }
    else
    {
      return nullptr;
    }
  }

  //***********************************
  template< typename T, typename... TTypes >
  TYPHOON_CONSTEXPR14 tphn::add_pointer_t<const T> get_if(const tphn::variant<TTypes...>* pv) noexcept
  {
    constexpr size_t Index = tphn::private_variant::parameter_pack<TTypes...>::template index_of_type<T>::value;

    if ((pv != nullptr) && (pv->index() == Index))
    {
      return &tphn::get<Index>(*pv);
    }
    else
    {
      return nullptr;
    }
  }

  //***************************************************************************
  /// swap
  //***************************************************************************
  template <typename... TTypes>
  void swap(tphn::variant<TTypes...>& lhs, tphn::variant<TTypes...>& rhs)
  {
    lhs.swap(rhs);
  }

  //***************************************************************************
  /// variant_size
  //***************************************************************************
  template <typename T>
  struct variant_size;

  template <typename... TTypes>
  struct variant_size<tphn::variant<TTypes...>>
    : tphn::integral_constant<size_t, sizeof...(TTypes)>
  {
  };

  template <typename T>
  struct variant_size<const T>
    : tphn::integral_constant<size_t, variant_size<T>::value>
  {
  };

#if TYPHOON_USING_CPP17
  template <typename... TTypes>
  inline constexpr size_t variant_size_v = variant_size<TTypes...>::value;
#endif

  //***************************************************************************
  /// visit
  //***************************************************************************
  namespace private_variant
  {
    template <typename TRet, typename TCallable, typename TVariant, size_t tIndex, typename TNext, typename... TVariants>
    static TYPHOON_CONSTEXPR14 TRet do_visit_single(TCallable&& f, TVariant&& v, TNext&&, TVariants&&... vs);

    //***************************************************************************
    /// Dummy-struct used to indicate that the return type should be auto-deduced
    /// from the callable object and the alternatives in the variants passed to
    /// a visit. Should never explicitly be used by an user.
    //***************************************************************************
    struct visit_auto_return
    {
    };

    //***************************************************************************
    /// Deduces return type of a call to TCallable with arguments Ts.
    /// A lite version of std::invoke_result.
    //***************************************************************************
    template <typename TCallable, typename... Ts>
    struct single_visit_result_type
    {
      using type = decltype(declval<TCallable>()(declval<Ts>()...));
    };

    template <typename TCallable, typename... Ts>
    using single_visit_result_type_t = typename single_visit_result_type<TCallable, Ts...>::type;

    //***************************************************************************
    /// Used to copy r/l value reference qualifier from a variant type to an
    /// element.
    //***************************************************************************
    template <typename TVar, typename T>
    using rlref_copy = conditional_t<is_reference<TVar>::value, T&, T&&>;

    //***************************************************************************
    /// Evaluates all permutations of calls to a callable object that can be done
    /// based upon the variants input. Need a `index_sequence<...>` as second
    /// argument that contains all possible indices of the first following variant.
    /// The first argument is essentially a `single_visit_result_type`-prototype
    /// in which every recursive instantiation of `visit_result_helper` appends
    /// more elements and give it a pass through `common_type_t`.
    //***************************************************************************
    template <template <typename...> typename, typename...>
    struct visit_result_helper;

    template <template <typename...> typename TToInject, size_t... tAltIndices, typename TCur>
    struct visit_result_helper<TToInject, index_sequence<tAltIndices...>, TCur>
    {
      template <size_t tIndex>
      using var_type = rlref_copy<TCur,
                                  variant_alternative_t<tIndex, remove_reference_t<TCur> > >;

      using type = common_type_t<TToInject<var_type<tAltIndices> >...>;
    };

    template <template <typename...> typename TToInject, size_t... tAltIndices, typename TCur, typename TNext, typename... TVs>
    struct visit_result_helper<TToInject, index_sequence<tAltIndices...>, TCur, TNext, TVs...>
    {
      template <size_t tIndex>
      using var_type = rlref_copy<TCur, variant_alternative_t<tIndex, remove_reference_t<TCur> > >;

      template <size_t tIndex>
      struct next_inject_wrap
      {
        template <typename... TNextInj>
        using next_inject = TToInject<var_type<tIndex>, TNextInj...>;
        using recursive_result = typename visit_result_helper<next_inject, make_index_sequence<variant_size<remove_reference_t<TNext> >::value>, TNext, TVs...>::type;
      };

      using type = common_type_t<typename next_inject_wrap<tAltIndices>::recursive_result...>;
    };

    //***************************************************************************
    /// Generates the result type for visit by applying 'common_type' on the return
    /// type from calls to function object with all possible permutations of variant
    /// alternatives. Shortcuts to first argument unless it is 'visit_auto_return'.
    //***************************************************************************
    template <typename TRet, typename...>
    struct visit_result
    {
      using type = TRet;
    };

    template <typename TCallable, typename T1, typename... Ts>
    struct visit_result<visit_auto_return, TCallable, T1, Ts...>
    {
      // bind TCallable to the first argument in this variadic alias.
      template <typename... Ts2>
      using single_res = single_visit_result_type_t<TCallable, Ts2...>;
      using type = typename visit_result_helper<single_res, make_index_sequence<variant_size<remove_reference_t<T1> >::value>, T1, Ts...>::type;
    };

    template <typename... Ts>
    using visit_result_t = typename visit_result<Ts...>::type;

    //***************************************************************************
    /// Makes a call to TCallable using tIndex alternative to the variant.
    /// Instantiated as function pointer in the `do_visit` function.
    //***************************************************************************
    template <typename TRet, typename TCallable, typename TVariant, size_t tIndex>
    constexpr TRet do_visit_single(TCallable&& f, TVariant&& v)
    {
      return static_cast<TCallable&&>(f)(tphn::get<tIndex>(static_cast<TVariant&&>(v)));
    }

    //***************************************************************************
    /// Helper to instantiate the function pointers needed for the "jump table".
    /// Embedds the 'TVarRest' (remaining variants) into its type to come around
    /// the "double expansion" otherwise needed in "do_visit".
    //***************************************************************************
    template <typename TRet, typename TCallable, typename TCurVariant, typename... TVarRest>
    struct do_visit_helper
    {
      using function_pointer = add_pointer_t<TRet(TCallable&&, TCurVariant&&, TVarRest&&...)>;
      
      template <size_t tIndex>
      static constexpr function_pointer fptr() noexcept
      {
        return &do_visit_single<TRet, TCallable, TCurVariant, tIndex, TVarRest...>;
      }
    };

    //***************************************************************************
    /// Dispatch current variant into recursive calls to dispatch the rest.
    //***************************************************************************
    template <typename TRet, typename TCallable, typename TVariant, size_t... tIndices, typename... TVarRest>
    static TYPHOON_CONSTEXPR14 TRet do_visit(TCallable&& f, TVariant&& v, index_sequence<tIndices...>, TVarRest&&... variants)
    {
      TYPHOON_ASSERT(!v.valueless_by_exception(), TYPHOON_ERROR(bad_variant_access));
      
      using helper_t = do_visit_helper<TRet, TCallable, TVariant, TVarRest...>;
      using func_ptr = typename helper_t::function_pointer;
      
      constexpr func_ptr jmp_table[]
      {
        helper_t::template fptr<tIndices>()...
      };

      return jmp_table[v.index()](static_cast<TCallable&&>(f), static_cast<TVariant&&>(v), static_cast<TVarRest&&>(variants)...);
    }

    template <typename TRet, typename TCallable, typename TVariant, typename... TVs>
    static TYPHOON_CONSTEXPR14 TRet visit(TCallable&& f, TVariant&& v, TVs&&... vs)
    {
      constexpr size_t variants = tphn::variant_size<typename remove_reference<TVariant>::type>::value;
      return private_variant::do_visit<TRet>(static_cast<TCallable&&>(f),
                                             static_cast<TVariant&&>(v),
                                             make_index_sequence<variants>{},
                                             static_cast<TVs&&>(vs)...);
    }

    //***************************************************************************
    /// Allows constexpr operation in c++14, otherwise acts like a lambda to
    /// bind a variant "get" to an argument for "TCallable".
    //***************************************************************************
    template <typename TRet, typename TCallable, typename TVariant, size_t tIndex>
    class constexpr_visit_closure
    {
      add_pointer_t<TCallable> callable_;
      add_pointer_t<TVariant>  variant_;

    public:
      constexpr constexpr_visit_closure(TCallable&& c, TVariant&& v)
        : callable_(&c), variant_(&v)
      {
      }

      template <typename... Ts>
      TYPHOON_CONSTEXPR14 TRet operator()(Ts&&... args) const
      {
        return static_cast<TCallable&&>(*callable_)(get<tIndex>(static_cast<TVariant&&>(*variant_)), static_cast<Ts&&>(args)...);
      }
    };

    template <typename TRet, typename TCallable, typename TVariant, size_t tIndex, typename TNext, typename... TVariants>
    static TYPHOON_CONSTEXPR14 TRet do_visit_single(TCallable&& f, TVariant&& v, TNext&& next, TVariants&&... vs)
    {
      return private_variant::visit<TRet>(constexpr_visit_closure<TRet, TCallable, TVariant, tIndex>(static_cast<TCallable&&>(f), static_cast<TVariant&&>(v)),
                                          static_cast<TNext&&>(next), static_cast<TVariants&&>(vs)...);
    }

  }  // namespace private_variant

  //***************************************************************************
  /// c++11/14 compatible tphn::visit for tphn::variant. Supports both c++17
  /// "auto return type" signature and c++20 explicit template return type.
  //***************************************************************************
  template <typename TRet = private_variant::visit_auto_return, typename... TVariants, typename TCallable, typename TDeducedReturn = private_variant::visit_result_t<TRet, TCallable, TVariants...> >
  static TYPHOON_CONSTEXPR14 TDeducedReturn visit(TCallable&& f, TVariants&&... vs)
  {
    return private_variant::visit<TDeducedReturn>(static_cast<TCallable&&>(f), static_cast<TVariants&&>(vs)...);
  }
}
#endif
