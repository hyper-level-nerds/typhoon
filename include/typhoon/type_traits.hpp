//
// Copyright (c) 2016-2019 Jin (jayjintheprogrammer@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/hyper-level-nerds/typhoon
//

#ifndef TYPHOON_TYPE_TRAITS_HPP
#define TYPHOON_TYPE_TRAITS_HPP

#include <typhoon/cstdint.hpp>
#include <typhoon/cstddef.hpp>

namespace tpn {

template <class...>
using void_t = void;

template <class Type, Type Value>
struct integral_constant
{
    using value_type = Type;
    static constexpr value_type value = Value;
    using type = integral_constant<Type, Value>;
    constexpr operator value_type() const
    {
        return value;
    }
};

using false_type = integral_constant<bool, false>;
using true_type = integral_constant<bool, true>;

template <class Type, Type Value>
constexpr Type integral_constant<Type, Value>::value;

template <bool Value>
using bool_constant = integral_constant<bool, Value>;

template <class Type>
struct negation : bool_constant<!bool(Type::value)> { };
template <class Type>
constexpr inline bool negation_v = negation<Type>::value;

template <class Type>
struct remove_reference { using type = Type; };
template <class Type>
struct remove_reference<Type &> { using type = Type; };
template <class Type>
struct remove_reference<Type &&> { using type = Type; };
template <class Type>
using remove_reference_t = typename remove_reference<Type>::type;

template <class Type>
struct remove_pointer { using type = Type; };
template <class Type>
struct remove_pointer<Type*>
{ using type = Type; };
template <class Type>
struct remove_pointer<const Type*> { using type = const Type; };
template <class Type>
struct remove_pointer<volatile Type*> { using type = volatile Type; };
template <class Type>
struct remove_pointer<const volatile Type*> { using type = const volatile Type; };
template <class Type>
struct remove_pointer<Type* const> { using type = Type; };
template <class Type>
struct remove_pointer<const Type* const> { using type = const Type; };
template <class Type>
struct remove_pointer<volatile Type* const> { using type = volatile Type; };
template <class Type>
struct remove_pointer<const volatile Type* const> { using type = const volatile Type; };
template <class Type>
using remove_pointer_t = typename remove_pointer<Type>::type;

template <class Type>
struct add_pointer { using type = typename remove_reference<Type>::type*; };
template <class Type>
using add_pointer_t = typename add_pointer<Type>::type;

template <class Type>
struct is_const : false_type { };
template <class Type>
struct is_const<const Type> : true_type { };
template <class Type>
struct is_const<const volatile Type> : true_type { };
template <class Type>
constexpr inline bool is_const_v = is_const<Type>::value;

template <class Type>
struct remove_const { using type = Type; };
template <class Type>
struct remove_const<const Type> { using type = Type; };
template <class Type>
using remove_const_t = typename remove_const<Type>::type;

template <class Type>
struct add_const { using type = const Type; };
template <class Type>
struct add_const<const Type> { using type = const Type; };
template <class Type>
using add_const_t = typename add_const<Type>::type;

template <class Type>
struct is_volatile : false_type { };
template <class Type>
struct is_volatile<volatile Type> : true_type { };
template <class Type>
struct is_volatile<const volatile Type> : true_type { };
template <class Type>
inline constexpr bool is_volatile_v = is_volatile<Type>::value;

template <class Type>
struct remove_volatile { using type = Type; };
template <class Type>
struct remove_volatile<volatile Type> { using type = Type; };
template <class Type>
using remove_volatile_t = typename remove_volatile<Type>::type;

template <class Type>
struct add_volatile { using type = volatile Type; };
template <class Type>
struct add_volatile<volatile Type> { using type = volatile Type; };
template <class Type>
using add_volatile_t = typename add_volatile<Type>::type;

template <class Type>
struct remove_cv
{
    using type =
        remove_volatile_t<
            remove_const_t<Type>>;
};
template <class Type>
using remove_cv_t = typename remove_cv<Type>::type;

template <class Type>
struct add_cv
{
    using type =
        add_volatile_t<
            add_const_t<Type>>;
};
template <class Type>
using add_cv_t = typename add_cv<Type>::type;

template <class Type>
struct remove_cvref
{
    using type =
        remove_cv_t<
            remove_reference_t<Type>>;
};
template <class Type>
using remove_cvref_t = typename remove_cvref<Type>::type;

template <class Type>
struct is_integral : false_type { };
template <>
struct is_integral<bool> : true_type { };
template <>
struct is_integral<char> : true_type { };
template <>
struct is_integral<unsigned char> : true_type { };
template <>
struct is_integral<signed char> : true_type { };
template <>
struct is_integral<wchar_t> : true_type { };
template <>
struct is_integral<short> : true_type { };
template <>
struct is_integral<unsigned short> : true_type { };
template <>
struct is_integral<int> : true_type { };
template <>
struct is_integral<unsigned int> : true_type { };
template <>
struct is_integral<long> : true_type { };
template <>
struct is_integral<unsigned long> : true_type { };
template <>
struct is_integral<long long> : true_type { };
template <>
struct is_integral<unsigned long long> : true_type { };
template <class Type>
struct is_integral<const Type> : is_integral<Type> { };
template <class Type>
struct is_integral<volatile Type> : is_integral<Type> { };
template <class Type>
struct is_integral<const volatile Type> : is_integral<Type> { };
template <class Type>
inline constexpr bool is_integral_v = is_integral<Type>::value;

template <class Type>
struct is_signed : false_type { };
template <>
struct is_signed<char> : bool_constant<(char(255) < 0)> { };
template <>
struct is_signed<wchar_t> : bool_constant<static_cast<bool>(wchar_t(-1) < wchar_t(0))> { };
template <>
struct is_signed<signed char> : true_type { };
template <>
struct is_signed<short> : true_type { };
template <>
struct is_signed<int> : true_type { };
template <>
struct is_signed<long> : true_type { };
template <>
struct is_signed<long long> : true_type { };
template <>
struct is_signed<float> : true_type { };
template <>
struct is_signed<double> : true_type { };
template <>
struct is_signed<long double> : true_type { };
template <class Type>
struct is_signed<const Type> : is_signed<Type> { };
template <class Type>
struct is_signed<volatile Type> : is_signed<Type> { };
template <class Type>
struct is_signed<const volatile Type> : is_signed<Type> { };
template <class Type>
inline constexpr bool is_signed_v = is_signed<Type>::value;

template <class Type>
struct is_unsigned : false_type { };
template <>
struct is_unsigned<bool> : true_type { };
template <>
struct is_unsigned<char> : bool_constant<(char(255) > 0)> { };
template <>
struct is_unsigned<unsigned char> : true_type { };
template <>
struct is_unsigned<wchar_t> : bool_constant<(wchar_t(-1) > wchar_t(0))> { };
template <>
struct is_unsigned<unsigned short> : true_type { };
template <>
struct is_unsigned<unsigned int> : true_type { };
template <>
struct is_unsigned<unsigned long> : true_type { };
template <>
struct is_unsigned<unsigned long long> : true_type { };
template <class Type>
struct is_unsigned<const Type> : is_unsigned<Type> { };
template <class Type>
struct is_unsigned<volatile Type> : is_unsigned<Type> { };
template <class Type>
struct is_unsigned<const volatile Type> : is_unsigned<Type> { };
template <class Type>
constexpr inline bool is_unsigned_v = is_unsigned<Type>::value;

template <class Type>
struct is_floating_point : false_type { };
template <>
struct is_floating_point<float> : true_type { };
template <>
struct is_floating_point<double> : true_type { };
template <>
struct is_floating_point<long double> : true_type { };
template <class Type>
struct is_floating_point<const Type> : is_floating_point<Type> { };
template <class Type>
struct is_floating_point<volatile Type> : is_floating_point<Type> { };
template <class Type>
struct is_floating_point<const volatile Type> : is_floating_point<Type> { };
template <class Type>
inline constexpr bool is_floating_point_v = is_floating_point<Type>::value;

template <class Type1, class Type2>
struct is_same : public false_type { };
template <typename Type>
struct is_same<Type, Type> : public true_type { };
template <class Type1, class Type2>
constexpr inline bool is_same_v = is_same<Type1, Type2>::value;

template <class Type>
struct is_void : false_type { };
template <>
struct is_void<void> : true_type { };
template <class Type>
constexpr inline bool is_void_v = is_void<Type>::value;

template <class Type>
struct is_arithmetic : bool_constant<is_integral_v<Type> || is_floating_point_v<Type>> { };
template <class Type>
constexpr inline bool is_arithmetic_v = is_arithmetic<Type>::value;

template <class Type>
struct is_fundamental : bool_constant<is_arithmetic_v<Type> || is_void_v<Type>> { };
template <class Type>
constexpr inline bool is_fundamental_v = is_fundamental<Type>::value;

template <class Type>
struct is_compound : bool_constant<!is_fundamental_v<Type>> { };
template <class Type>
constexpr inline bool is_compound_v = is_compound<Type>::value;

template <class Type>
struct is_array : false_type { };
template <class Type>
struct is_array<Type[]> : true_type { };
template <class Type, size_t Size>
struct is_array<Type[Size]> : true_type { };
template <class Type>
constexpr inline bool is_array_v = is_array<Type>::value;

namespace detail {
template <class Type> struct is_pointer : false_type { };
template <class Type> struct is_pointer<Type*> : true_type { };
template <class Type> struct is_lvalue_reference : false_type { };
template <class Type> struct is_lvalue_reference<Type &> : true_type { };
template <class Type> struct is_rvalue_reference : false_type { };
template <class Type> struct is_rvalue_reference<Type &&> : true_type { };
}

template <class Type>
struct is_pointer : detail::is_pointer<remove_cv_t<Type>> { };
template <class Type>
constexpr inline bool is_pointer_v = is_pointer<Type>::value;

template <class Type>
struct is_lvalue_reference : detail::is_lvalue_reference<remove_cv_t<Type>> { };
template <class Type>
constexpr inline bool is_lvalue_reference_v = is_lvalue_reference<Type>::value;

template <class Type>
struct is_rvalue_reference : detail::is_rvalue_reference<remove_cv_t<Type>> { };
template <class Type>
constexpr inline bool is_rvalue_reference_v = is_rvalue_reference<Type>::value;

template <class Type>
struct is_reference : integral_constant<
                          bool,
                          is_lvalue_reference_v<Type> ||
                              is_rvalue_reference_v<Type>> { };
template <class Type>
constexpr inline bool is_reference_v = is_reference<Type>::value;

template <class Type>
struct is_pod : bool_constant<is_fundamental_v<Type> || is_pointer_v<Type>> { };
template <class Type>
constexpr inline bool is_pod_v = is_pod<Type>::value;

template <bool Cond, class True, class False>
struct conditional { using type = True; };
template <class True, class False>
struct conditional<false, True, False> { using type = False; };
template <bool Cond, class True, class False>
using conditional_t = typename conditional<Cond, True, False>::type;

template <class Type>
struct make_signed { using type = Type; };
template <>
struct make_signed<char> { using type = signed char; };
template <>
struct make_signed<unsigned char> { using type = signed char; };
template <>
struct make_signed<wchar_t>
{
    using type =
        conditional_t<
            sizeof(wchar_t) == sizeof(int16_t),
            int16_t,
            conditional_t<
                sizeof(wchar_t) == sizeof(int32_t),
                int32_t,
                void>>;
};
template <>
struct make_signed<unsigned short> { using type = short; };
template <>
struct make_signed<unsigned int> { using type = int; };
template <>
struct make_signed<unsigned long> { using type = long; };
template <>
struct make_signed<unsigned long long> { using type = long long; };
template <class Type>
struct make_signed<const Type> : add_const<typename make_signed<Type>::type> { };
template <class Type>
struct make_signed<volatile Type> : add_volatile<typename make_signed<Type>::type> { };
template <class Type>
struct make_signed<const volatile Type> : add_const_t<add_volatile_t<typename make_signed<Type>::type>> { };
template <class Type>
using make_signed_t = typename make_signed<Type>::type;

template <typename Type>
struct make_unsigned { using type = Type; };
template <>
struct make_unsigned<char> { using type = unsigned char; };
template <>
struct make_unsigned<signed char> { using type = unsigned char; };
template <>
struct make_unsigned<short> { using type = unsigned short; };
template <>
struct make_unsigned<wchar_t>
{
    using type =
        conditional_t<
            sizeof(wchar_t) == sizeof(uint16_t),
            uint16_t,
            conditional_t<sizeof(wchar_t) == sizeof(uint32_t),
                          uint32_t,
                          void>>;
};
template <>
struct make_unsigned<int> { using type = unsigned int; };
template <>
struct make_unsigned<long> { using type = unsigned long; };
template <>
struct make_unsigned<long long> { using type = unsigned long long; };
template <class Type>
struct make_unsigned<const Type> : add_const<typename make_unsigned<Type>::type> { };
template <class Type>
struct make_unsigned<volatile Type> : add_volatile<typename make_unsigned<Type>::type> { };
template <class Type>
struct make_unsigned<const volatile Type> : add_const_t<add_volatile_t<typename make_unsigned<Type>::type>> { };
template <class Type>
using make_unsigned_t = typename make_unsigned<Type>::type;

template <bool Cond, class Type = void>
struct enable_if { };
template <class Type>
struct enable_if<true, Type> { using type = Type; };
template <bool Cond, typename Type = void>
using enable_if_t = typename enable_if<Cond, Type>::type;

template <class Type, unsigned Size = 0U>
struct extent : integral_constant<size_t, 0U> { };
template <class Type>
struct extent<Type[], 0> : integral_constant<size_t, 0U> { };
template <class Type, unsigned Size>
struct extent<Type[], Size> : integral_constant<size_t, extent<Type, Size - 1>::value> { };
template <class Type, unsigned Size>
struct extent<Type[Size], 0> : integral_constant<size_t, Size> { };
template <class Type, unsigned Idx, unsigned Size>
struct extent<Type[Idx], Size> : integral_constant<size_t, extent<Type, Size - 1>::value> { };
template <class Type, unsigned Size = 0U>
constexpr inline size_t extent_v = extent<Type, Size>::value;

template <class Type>
struct remove_extent { using type = Type; };
template <class Type>
struct remove_extent<Type[]> { using type = Type; };
template <class Type, size_t Size>
struct remove_extent<Type[Size]> { using type = Type; };
template <class Type>
using remove_extent_t = typename remove_extent<Type>::type;

template <class Type>
struct remove_all_extents { using type = Type; };
template <class Type>
struct remove_all_extents<Type[]> { using type = typename remove_all_extents<Type>::type; };
template <class Type, size_t Size>
struct remove_all_extents<Type[Size]> { using type = typename remove_all_extents<Type>::type; };
template <class Type>
using remove_all_extents_t = typename remove_all_extents<Type>::type;

template <class Type>
struct rank : integral_constant<size_t, 0> { };
template <class Type>
struct rank<Type[]> : public integral_constant<size_t, rank<Type>::value + 1> { };
template <class Type, size_t Size>
struct rank<Type[Size]> : public integral_constant<size_t, rank<Type>::value + 1> { };
template <class Type>
constexpr inline size_t rank_v = rank<Type>::value;

template <class Type>
struct decay
{
    using type =
        conditional_t<
            is_array_v<remove_reference_t<Type>>,
            remove_extent_t<remove_reference_t<Type>> *,
            remove_cv_t<remove_reference_t<Type>>>;
};
template <class Type>
using decay_t = typename decay<Type>::type;

template <
    typename BaseType,
    typename DerivedType,
    const bool IsFundamental =
        is_fundamental_v<BaseType> ||
        is_fundamental_v<DerivedType> ||
        is_array_v<DerivedType>
>
struct is_base_of
{
private:
    template <class Type>
    struct dummy { };
    struct internal : DerivedType, dummy<int> { };

    static BaseType* check(BaseType*) { return (BaseType*)0; }

    template <class Type>
    static char check(dummy<Type>*) { return 0; }

public:
    static constexpr bool value =
        (sizeof(check((internal*)0)) == sizeof(BaseType*));
};
template <class BaseType, class DerivedType>
struct is_base_of<BaseType, DerivedType, true> { static constexpr bool value = false; };
template <class BaseType, class DerivedType>
constexpr inline bool is_base_of_v = is_base_of<BaseType, DerivedType>::value;

namespace detail {
template <class Type>
char is_class(int Type::*);
struct is_class_dummy { char c[2]; };
template <class Type>
is_class_dummy is_class(...);
}

template <class Type>
struct is_class : bool_constant<sizeof(detail::is_class<Type>(0)) == 1U> { };
template <class Type>
constexpr inline bool is_class_v = is_class<Type>::value;

template <class Type>
struct add_lvalue_reference { using type = Type&; };
template <class Type>
struct add_lvalue_reference<Type&> { using type = Type&; };
template <>
struct add_lvalue_reference<void> { using type = void; };
template <>
struct add_lvalue_reference<const void> { using type = const void; };
template <>
struct add_lvalue_reference<volatile void> { using type = volatile void; };
template <>
struct add_lvalue_reference<const volatile void> { using type = const volatile void; };
template <class Type>
using add_lvalue_reference_t = typename add_lvalue_reference<Type>::type;

template <class Type>
struct add_rvalue_reference { using type = Type&&; };
template <class Type>
struct add_rvalue_reference<Type&> { using type = Type&&; };
template <>
struct add_rvalue_reference<void> { using type = void; };
template <>
struct add_rvalue_reference<const void> { using type = const void; };
template <>
struct add_rvalue_reference<volatile void> { using type = volatile void; };
template <>
struct add_rvalue_reference<const volatile void> { using type = const volatile void; };
template <class Type>
using add_rvalue_reference_t = typename add_rvalue_reference<Type>::type;

template <class Type>
typename add_rvalue_reference<Type>::type declval() noexcept;

namespace detail {
template <class Type, class = int>
struct is_enum : false_type { };
template <class Type>
struct is_enum<Type, decltype(static_cast<int>(declval<Type>()))> : true_type { };
}
template <class Type>
struct is_enum : integral_constant<bool, detail::is_enum<Type>::value &&
                                         is_class_v<Type> == false &&
                                         is_arithmetic_v<Type> == false &&
                                         is_reference_v<Type> == false
> { };
template <class Type>
constexpr inline bool is_enum_v = is_enum<Type>::value;

namespace detail {
template <class>
using true_type_for = true_type;
template <class Type>
auto returnable(int) -> true_type_for<Type()>;
template <class>
auto returnable(...) -> false_type;
template <class FromType, class ToType>
auto nonvoid_convertible(int) -> true_type_for<decltype(tpn::declval<void (&)(ToType)>()(tpn::declval<FromType>()))>;
template <class, class>
auto nonvoid_convertible(...) -> false_type;
}

template <class FromType, class ToType>
struct is_convertible : bool_constant<
    (decltype(detail::returnable<ToType>(0))::value &&
     decltype(detail::nonvoid_convertible<FromType, ToType>(0))::value) ||
    (is_void_v<FromType> && is_void_v<ToType>)
> { };
template <class FromType, class ToType>
constexpr inline bool is_convertible_v = is_convertible<FromType, ToType>::value;

template <class Type>
struct alignment_of : integral_constant<size_t, size_t(__alignof__(Type))> {};
template <>
struct alignment_of<void> : integral_constant<size_t, 0> { };
template <>
struct alignment_of<const void> : integral_constant<size_t, 0> { };
template <class Type>
constexpr inline size_t alignment_of_v = alignment_of<Type>::value;


template <bool Cond, class Type, Type TrueValue, Type FalseValue>
struct conditional_integral_constant;
template <class Type, Type TrueValue, Type FalseValue>
struct conditional_integral_constant<true, Type, TrueValue, FalseValue>
{
    static_assert(is_integral_v<Type>, "Not an integral type");
    static constexpr Type value = TrueValue;
};
template <class Type, Type TrueValue, Type FalseValue>
struct conditional_integral_constant<false, Type, TrueValue, FalseValue>
{
    static_assert(is_integral_v<Type>, "Not an integral type");
    static constexpr Type value = FalseValue;
};

#if TYPHOON_USING_CPP11
    //***************************************************************************
    /// Template to determine if a type is one of a specified list.
    ///\ingroup types
    template <typename T, typename T1, typename... TRest>
    struct is_one_of
    {
        static const bool value = tpn::is_same<T, T1>::value ||
                                  tpn::is_one_of<T, TRest...>::value;
    };

    template <typename T, typename T1>
    struct is_one_of<T, T1>
    {
        static const bool value = tpn::is_same<T, T1>::value;
    };
#else
//***************************************************************************
/// Template to determine if a type is one of a specified list.
///\ingroup types
template <typename T,
          typename T1, typename T2 = void, typename T3 = void, typename T4 = void,
          typename T5 = void, typename T6 = void, typename T7 = void, typename T8 = void,
          typename T9 = void, typename T10 = void, typename T11 = void, typename T12 = void,
          typename T13 = void, typename T14 = void, typename T15 = void, typename T16 = void>
struct is_one_of
{
    static const bool value =
        tpn::is_same<T, T1>::value ||
        tpn::is_same<T, T2>::value ||
        tpn::is_same<T, T3>::value ||
        tpn::is_same<T, T4>::value ||
        tpn::is_same<T, T5>::value ||
        tpn::is_same<T, T6>::value ||
        tpn::is_same<T, T7>::value ||
        tpn::is_same<T, T8>::value ||
        tpn::is_same<T, T9>::value ||
        tpn::is_same<T, T10>::value ||
        tpn::is_same<T, T11>::value ||
        tpn::is_same<T, T12>::value ||
        tpn::is_same<T, T13>::value ||
        tpn::is_same<T, T14>::value ||
        tpn::is_same<T, T15>::value ||
        tpn::is_same<T, T16>::value;
};
#endif

#if TYPHOON_USING_CPP17
    template <typename T, typename... TRest>
    inline constexpr bool is_one_of_v = tpn::is_one_of<T, TRest...>::value;
#endif

    //***************************************************************************
    /// A set of templates to allow related types to be derived.
    ///\ingroup types

    // Default.
    template <typename T>
    struct types
    {
    private:
        typedef typename tpn::remove_reference<typename tpn::remove_cv<T>::type>::type type_t;

    public:
        typedef type_t type;
        typedef type_t &reference;
        typedef const type_t &const_reference;
        typedef type_t *pointer;
        typedef const type_t *const_pointer;
        typedef const type_t *const const_pointer_const;

#if TYPHOON_USING_CPP11
        typedef type_t &&rvalue_reference;
#endif
    };

    // Pointers.
    template <typename T>
    struct types<T *>
    {
    private:
        typedef typename tpn::remove_reference<typename tpn::remove_cv<T>::type>::type type_t;

    public:
        typedef type_t type;
        typedef type_t &reference;
        typedef const type_t &const_reference;
        typedef type_t *pointer;
        typedef const type_t *const_pointer;
        typedef const type_t *const const_pointer_const;

#if TYPHOON_USING_CPP11
        typedef type_t &&rvalue_reference;
#endif
    };

    // Pointers.
    template <typename T>
    struct types<T *const>
    {
    private:
        typedef typename tpn::remove_reference<typename tpn::remove_cv<T>::type>::type type_t;

    public:
        typedef type_t type;
        typedef type_t &reference;
        typedef const type_t &const_reference;
        typedef type_t *pointer;
        typedef const type_t *const_pointer;
        typedef const type_t *const const_pointer_const;

#if TYPHOON_USING_CPP11
        typedef type_t &&rvalue_reference;
#endif
    };

    // References.
    template <typename T>
    struct types<T &>
    {
    private:
        typedef typename tpn::remove_reference<typename tpn::remove_cv<T>::type>::type type_t;

    public:
        typedef type_t type;
        typedef type_t &reference;
        typedef const type_t &const_reference;
        typedef type_t *pointer;
        typedef const type_t *const_pointer;
        typedef const type_t *const const_pointer_const;

#if TYPHOON_USING_CPP11
        typedef type_t &&rvalue_reference;
#endif
    };

#if TYPHOON_USING_CPP11
    // rvalue References.
    template <typename T>
    struct types<T &&>
    {
    private:
        typedef typename tpn::remove_reference<typename tpn::remove_cv<T>::type>::type type_t;

    public:
        typedef type_t type;
        typedef type_t &reference;
        typedef const type_t &const_reference;
        typedef type_t *pointer;
        typedef const type_t *const_pointer;
        typedef const type_t *const const_pointer_const;

#if TYPHOON_USING_CPP11
        typedef type_t &&rvalue_reference;
#endif
    };
#endif

#if TYPHOON_USING_CPP11
    template <typename T>
    using types_t = typename types<T>::type;

    template <typename T>
    using types_r = typename types<T>::reference;

    template <typename T>
    using types_cr = typename types<T>::const_reference;

    template <typename T>
    using types_rr = typename types<T>::rvalue_reference;

    template <typename T>
    using types_p = typename types<T>::pointer;

    template <typename T>
    using types_cp = typename types<T>::const_pointer;

    template <typename T>
    using types_cpc = typename types<T>::const_pointer_const;
#endif

    //***************************************************************************
    /// size_of
    ///\ingroup types
    template <typename T>
    struct size_of : tpn::integral_constant<size_t, sizeof(T)>
    {
    };
    template <>
    struct size_of<void> : tpn::integral_constant<size_t, 1U>
    {
    };

#if TYPHOON_USING_CPP17
    template <typename T>
    inline constexpr size_t size_of_v = tpn::size_of<T>::value;
#endif

#if TYPHOON_USING_CPP11
    //***************************************************************************
    /// are_all_same
    template <typename T, typename T1, typename... TRest>
    struct are_all_same
    {
        static const bool value = tpn::is_same<T, T1>::value &&
                                  tpn::are_all_same<T, TRest...>::value;
    };

    template <typename T, typename T1>
    struct are_all_same<T, T1>
    {
        static const bool value = tpn::is_same<T, T1>::value;
    };
#endif

#if TYPHOON_USING_CPP17
    template <typename T, typename T1, typename... TRest>
    inline constexpr bool are_all_same_v = are_all_same<T, T1, TRest...>::value;
#endif

    //***************************************************************************
    /// conjunction
#if TYPHOON_USING_CPP11
    template <typename...>
    struct conjunction : public tpn::true_type
    {
    };

    template <typename T1, typename... Tn>
    struct conjunction<T1, Tn...> : public tpn::conditional_t<bool(T1::value), tpn::conjunction<Tn...>, T1>
    {
    };

    template <typename T>
    struct conjunction<T> : public T
    {
    };
#endif

#if TYPHOON_USING_CPP17
    template <typename... T>
    inline constexpr bool conjunction_v = conjunction<T...>::value;
#endif

    template <typename...>
    struct disjunction : public tpn::false_type
    {
    };

    template <typename T1, typename... Tn>
    struct disjunction<T1, Tn...> : public tpn::conditional_t<bool(T1::value), T1, disjunction<Tn...>>
    {
    };

    template <typename T1>
    struct disjunction<T1> : public T1
    {
    };

#if TYPHOON_USING_CPP17
    template <typename... T>
    inline constexpr bool disjunction_v = tpn::disjunction<T...>::value;
#endif

    //***************************************************************************
#if TYPHOON_USING_STL && TYPHOON_USING_CPP11 && !defined(TYPHOON_USE_TYPE_TRAITS_BUILTINS) && !defined(TYPHOON_USER_DEFINED_TYPE_TRAITS) && ((!defined(ARDUINO) && TYPHOON_NOT_USING_STLPORT) || defined(TYPHOON_GCC_V5_TYPE_TRAITS_SUPPORTED))

    //*********************************************
    // Use the STL's definitions.
    //*********************************************

    //*********************************************
    // is_assignable
    template <typename T1, typename T2>
    struct is_assignable : public std::is_assignable<T1, T2>
    {
    };

    //*********************************************
    // is_constructible
    template <typename T, typename... TArgs>
    struct is_constructible : public std::is_constructible<T, TArgs...>
    {
    };

    //*********************************************
    // is_copy_constructible
    template <typename T>
    struct is_copy_constructible : public std::is_copy_constructible<T>
    {
    };

    //*********************************************
    // is_move_constructible
    template <typename T>
    struct is_move_constructible : public std::is_move_constructible<T>
    {
    };

    //*********************************************
    // is_trivially_constructible
#if TYPHOON_CPP11_TYPE_TRAITS_IS_TRIVIAL_SUPPORTED
    template <typename T>
    struct is_trivially_constructible : public std::is_trivially_constructible<T>
    {
    };
#else
    template <typename T>
    struct is_trivially_constructible : public tpn::bool_constant<tpn::is_arithmetic<T>::value || tpn::is_pointer<T>::value>
    {
    };
#endif

    //*********************************************
    // is_trivially_copy_constructible
#if TYPHOON_CPP11_TYPE_TRAITS_IS_TRIVIAL_SUPPORTED
    template <typename T>
    struct is_trivially_copy_constructible : public std::is_trivially_copy_constructible<T>
    {
    };
#else
    template <typename T>
    struct is_trivially_copy_constructible : public tpn::bool_constant<tpn::is_arithmetic<T>::value || tpn::is_pointer<T>::value>
    {
    };
#endif

    //*********************************************
    // is_trivially_destructible
#if TYPHOON_CPP11_TYPE_TRAITS_IS_TRIVIAL_SUPPORTED
    template <typename T>
    struct is_trivially_destructible : public std::is_trivially_destructible<T>
    {
    };
#else
    template <typename T>
    struct is_trivially_destructible : public tpn::bool_constant<tpn::is_arithmetic<T>::value || tpn::is_pointer<T>::value>
    {
    };
#endif

    //*********************************************
    // is_trivially_copy_assignable
#if TYPHOON_CPP11_TYPE_TRAITS_IS_TRIVIAL_SUPPORTED
    template <typename T>
    struct is_trivially_copy_assignable : public std::is_trivially_copy_assignable<T>
    {
    };
#else
    template <typename T>
    struct is_trivially_copy_assignable : public tpn::bool_constant<tpn::is_arithmetic<T>::value || tpn::is_pointer<T>::value>
    {
    };
#endif

    //*********************************************
    // is_trivially_copyable
#if TYPHOON_CPP11_TYPE_TRAITS_IS_TRIVIAL_SUPPORTED
    template <typename T>
    struct is_trivially_copyable : public std::is_trivially_copyable<T>
    {
    };
#else
    template <typename T>
    struct is_trivially_copyable : public tpn::bool_constant<tpn::is_arithmetic<T>::value || tpn::is_pointer<T>::value>
    {
    };
#endif

#elif defined(TYPHOON_USE_TYPE_TRAITS_BUILTINS) && !defined(TYPHOON_USER_DEFINED_TYPE_TRAITS)

//*********************************************
// Use the compiler's builtins.
//*********************************************

//*********************************************
// is_assignable
template <typename T1, typename T2>
struct is_assignable
{
    static TYPHOON_CONSTANT bool value = __is_assignable(T1, T2);
};

#if TYPHOON_USING_CPP11
//*********************************************
// is_constructible
template <typename T, typename... TArgs>
struct is_constructible
{
    static TYPHOON_CONSTANT bool value = __is_constructible(T, TArgs...);
};
#else
//*********************************************
// is_constructible
template <typename T, typename TArgs = void>
struct is_constructible
{
    static TYPHOON_CONSTANT bool value = __is_constructible(T, TArgs);
};

//*********************************************
// is_constructible
template <typename T>
struct is_constructible<T, void>
{
    static TYPHOON_CONSTANT bool value = __is_constructible(T);
};
#endif

//*********************************************
// is_copy_constructible
template <typename T>
struct is_copy_constructible : public tpn::is_constructible<T, typename tpn::add_lvalue_reference<const T>::type>
{
};

//*********************************************
// is_move_constructible
template <typename T>
struct is_move_constructible : public tpn::is_constructible<T, T>
{
};

#if TYPHOON_USING_CPP11
//*********************************************
// is_trivially_constructible
template <typename T, typename... TArgs>
struct is_trivially_constructible
{
#if defined(TYPHOON_COMPILER_GCC)
    static TYPHOON_CONSTANT bool value = __has_trivial_constructor(T);
#else
    static TYPHOON_CONSTANT bool value = __is_trivially_constructible(T, TArgs...);
#endif
};
#else
//*********************************************
// is_trivially_constructible
template <typename T, typename TArgs = void>
struct is_trivially_constructible
{
#if defined(TYPHOON_COMPILER_GCC)
    static TYPHOON_CONSTANT bool value = __has_trivial_constructor(T);
#else
    static TYPHOON_CONSTANT bool value = __is_trivially_constructible(T, TArgs);
#endif
};

//*********************************************
// is_trivially_constructible
template <typename T>
struct is_trivially_constructible<T, void>
{
#if defined(TYPHOON_COMPILER_GCC)
    static TYPHOON_CONSTANT bool value = __has_trivial_constructor(T);
#else
    static TYPHOON_CONSTANT bool value = __is_trivially_constructible(T);
#endif
};
#endif

//*********************************************
// is_trivially_copy_constructible
template <typename T>
struct is_trivially_copy_constructible : public is_trivially_constructible<T, typename add_lvalue_reference<const T>::type>
{
};

//*********************************************
// is_trivially_destructible
template <typename T>
struct is_trivially_destructible
{
#if defined(TYPHOON_COMPILER_GCC)
    static TYPHOON_CONSTANT bool value = __has_trivial_destructor(T);
#else
    static TYPHOON_CONSTANT bool value = __is_trivially_destructible(T);
#endif
};

//*********************************************
// is_trivially_copy_assignable
template <typename T>
struct is_trivially_copy_assignable
{
#if defined(TYPHOON_COMPILER_GCC)
    static TYPHOON_CONSTANT bool value = __has_trivial_copy(T);
#else
    static TYPHOON_CONSTANT bool value = __is_trivially_copyable(T);
#endif
};

//*********************************************
// is_trivially_copyable
template <typename T>
struct is_trivially_copyable
{
#if defined(TYPHOON_COMPILER_GCC)
    static TYPHOON_CONSTANT bool value = __has_trivial_copy(T);
#else
    static TYPHOON_CONSTANT bool value = __is_trivially_copyable(T);
#endif
};

#elif defined(TYPHOON_USER_DEFINED_TYPE_TRAITS) && !defined(TYPHOON_USE_TYPE_TRAITS_BUILTINS)

//*********************************************
// Force the user to provide specialisations for
// anything other than arithmetics and pointers.
//*********************************************

//*********************************************
// is_assignable
template <typename T1,
          typename T2,
          bool B = (tpn::is_arithmetic<T1>::value || tpn::is_pointer<T1>::value) && (tpn::is_arithmetic<T2>::value || tpn::is_pointer<T2>::value)>
struct is_assignable;

template <typename T1, typename T2>
struct is_assignable<T1, T2, true> : public tpn::true_type
{
};

template <typename T1, typename T2>
struct is_assignable<T1, T2, false>;

#if TYPHOON_USING_CPP11
//*********************************************
// is_constructible
template <typename T, bool B, typename... TArgs>
struct is_constructible_helper;

template <typename T, typename... TArgs>
struct is_constructible_helper<T, true, TArgs...> : public tpn::true_type
{
};

template <typename T, typename... TArgs>
struct is_constructible_helper<T, false, TArgs...>;

template <typename T, typename... TArgs>
struct is_constructible : public is_constructible_helper<T, tpn::is_arithmetic<T>::value || tpn::is_pointer<T>::value, TArgs...>
{
};
#endif

//*********************************************
// is_copy_constructible
template <typename T, bool B = tpn::is_arithmetic<T>::value || tpn::is_pointer<T>::value>
struct is_copy_constructible;

template <typename T>
struct is_copy_constructible<T, true> : public tpn::true_type
{
};

template <typename T>
struct is_copy_constructible<T, false>;

//*********************************************
// is_move_constructible
template <typename T, bool B = tpn::is_arithmetic<T>::value || tpn::is_pointer<T>::value>
struct is_move_constructible;

template <typename T>
struct is_move_constructible<T, true> : public tpn::true_type
{
};

template <typename T>
struct is_move_constructible<T, false>;

//*********************************************
// is_trivially_constructible
template <typename T, bool B = tpn::is_arithmetic<T>::value || tpn::is_pointer<T>::value>
struct is_trivially_constructible;

template <typename T>
struct is_trivially_constructible<T, true> : public tpn::true_type
{
};

template <typename T>
struct is_trivially_constructible<T, false>;

//*********************************************
// is_trivially_copy_constructible
template <typename T, bool B = tpn::is_arithmetic<T>::value || tpn::is_pointer<T>::value>
struct is_trivially_copy_constructible;

template <typename T>
struct is_trivially_copy_constructible<T, true> : public tpn::true_type
{
};

template <typename T>
struct is_trivially_copy_constructible<T, false>;

//*********************************************
// is_trivially_destructible
template <typename T, bool B = tpn::is_arithmetic<T>::value || tpn::is_pointer<T>::value>
struct is_trivially_destructible;

template <typename T>
struct is_trivially_destructible<T, true> : public tpn::true_type
{
};

template <typename T>
struct is_trivially_destructible<T, false>;

//*********************************************
// is_trivially_copy_assignable
template <typename T, bool B = tpn::is_arithmetic<T>::value || tpn::is_pointer<T>::value>
struct is_trivially_copy_assignable;

template <typename T>
struct is_trivially_copy_assignable<T, true> : public tpn::true_type
{
};

template <typename T>
struct is_trivially_copy_assignable<T, false>;

//*********************************************
// is_trivially_copyable
template <typename T, bool B = tpn::is_arithmetic<T>::value || tpn::is_pointer<T>::value>
struct is_trivially_copyable;

template <typename T>
struct is_trivially_copyable<T, true> : public tpn::true_type
{
};

template <typename T>
struct is_trivially_copyable<T, false>;

#else

//*********************************************
// Assume that anything other than arithmetics
// and pointers return false for the traits.
//*********************************************

//*********************************************
// is_assignable
template <typename T1, typename T2>
struct is_assignable : public tpn::bool_constant<(tpn::is_arithmetic<T1>::value || tpn::is_pointer<T1>::value) && (tpn::is_arithmetic<T2>::value || tpn::is_pointer<T2>::value)>
{
};

#if TYPHOON_USING_CPP11
//***************************************************************************
/// is_constructible
namespace private_type_traits
{
    template <class, class T, class... Args>
    struct is_constructible_ : tpn::false_type
    {
    };

    template <class T, class... Args>
    struct is_constructible_<void_t<decltype(T(tpn::declval<Args>()...))>, T, Args...> : tpn::true_type
    {
    };
};

//*********************************************
// is_constructible
template <class T, class... Args>
using is_constructible = private_type_traits::is_constructible_<void_t<>, T, Args...>;

//*********************************************
// is_copy_constructible
template <class T>
struct is_copy_constructible : public is_constructible<T, typename tpn::add_lvalue_reference<typename tpn::add_const<T>::type>::type>
{
};
template <>
struct is_copy_constructible<void> : public false_type
{
};
template <>
struct is_copy_constructible<void const> : public false_type
{
};
template <>
struct is_copy_constructible<void volatile> : public false_type
{
};
template <>
struct is_copy_constructible<void const volatile> : public false_type
{
};

//*********************************************
// is_move_constructible
template <typename T>
struct is_move_constructible : public is_constructible<T, typename tpn::add_rvalue_reference<T>::type>
{
};
template <>
struct is_move_constructible<void> : public false_type
{
};
template <>
struct is_move_constructible<void const> : public false_type
{
};
template <>
struct is_move_constructible<void volatile> : public false_type
{
};
template <>
struct is_move_constructible<void const volatile> : public false_type
{
};

#else

//*********************************************
// is_copy_constructible
template <typename T>
struct is_copy_constructible : public tpn::bool_constant<tpn::is_arithmetic<T>::value || tpn::is_pointer<T>::value>
{
};

//*********************************************
// is_move_constructible
template <typename T>
struct is_move_constructible : public tpn::bool_constant<tpn::is_arithmetic<T>::value || tpn::is_pointer<T>::value>
{
};
#endif

//*********************************************
// is_trivially_constructible
template <typename T>
struct is_trivially_constructible : public tpn::bool_constant<tpn::is_arithmetic<T>::value || tpn::is_pointer<T>::value>
{
};

//*********************************************
// is_trivially_copy_constructible
template <typename T>
struct is_trivially_copy_constructible : public tpn::bool_constant<tpn::is_arithmetic<T>::value || tpn::is_pointer<T>::value>
{
};

//*********************************************
// is_trivially_destructible
template <typename T>
struct is_trivially_destructible : public tpn::bool_constant<tpn::is_arithmetic<T>::value || tpn::is_pointer<T>::value>
{
};

//*********************************************
// is_trivially_copy_assignable
template <typename T>
struct is_trivially_copy_assignable : public tpn::bool_constant<tpn::is_arithmetic<T>::value || tpn::is_pointer<T>::value>
{
};

//*********************************************
// is_trivially_copyable
template <typename T>
struct is_trivially_copyable : public tpn::bool_constant<tpn::is_arithmetic<T>::value || tpn::is_pointer<T>::value>
{
};

#endif

    template <typename T1, typename T2>
    struct is_lvalue_assignable : public tpn::is_assignable<typename tpn::add_lvalue_reference<T1>::type,
                                                            typename tpn::add_lvalue_reference<typename tpn::add_const<T2>::type>::type>
    {
    };

#if TYPHOON_USING_CPP17

    template <typename T1, typename T2>
    inline constexpr bool is_assignable_v = tpn::is_assignable<T1, T2>::value;

    template <typename T1, typename T2>
    inline constexpr bool is_lvalue_assignable_v = tpn::is_lvalue_assignable<T1, T2>::value;

    template <typename T, typename... TArgs>
    inline constexpr bool is_constructible_v = tpn::is_constructible<T, TArgs...>::value;

    template <typename T>
    inline constexpr bool is_copy_constructible_v = tpn::is_copy_constructible<T>::value;

    template <typename T>
    inline constexpr bool is_move_constructible_v = tpn::is_move_constructible<T>::value;

    template <typename T>
    inline constexpr bool is_trivially_constructible_v = tpn::is_trivially_constructible<T>::value;

    template <typename T>
    inline constexpr bool is_trivially_copy_constructible_v = tpn::is_trivially_copy_constructible<T>::value;

    template <typename T>
    inline constexpr bool is_trivially_destructible_v = tpn::is_trivially_destructible<T>::value;

    template <typename T>
    inline constexpr bool is_trivially_copy_assignable_v = tpn::is_trivially_copy_assignable<T>::value;

    template <typename T>
    inline constexpr bool is_trivially_copyable_v = tpn::is_trivially_copyable<T>::value;

#endif

#if TYPHOON_USING_CPP11
    //*********************************************
    // common_type
    // Based on the sample implementation detailed on
    // https://en.cppreference.com/w/cpp/types/common_type
    //*********************************************
    //***********************************
    // Primary template
    template <typename...>
    struct common_type
    {
    };

    //***********************************
    // One type
    template <typename T>
    struct common_type<T> : common_type<T, T>
    {
    };

    namespace private_common_type
    {
        template <typename T1, typename T2>
        using conditional_result_t = decltype(false ? declval<T1>() : declval<T2>());

        template <typename, typename, typename = void>
        struct decay_conditional_result
        {
        };

        template <typename T1, typename T2>
        struct decay_conditional_result<T1, T2, void_t<conditional_result_t<T1, T2>>>
            : tpn::decay<conditional_result_t<T1, T2>>
        {
        };

        template <typename T1, typename T2, typename = void>
        struct common_type_2_impl : decay_conditional_result<const T1 &, const T2 &>
        {
        };

        template <typename T1, typename T2>
        struct common_type_2_impl<T1, T2, void_t<conditional_result_t<T1, T2>>>
            : decay_conditional_result<T1, T2>
        {
        };
    }

    //***********************************
    // Two types
    template <typename T1, typename T2>
    struct common_type<T1, T2>
        : tpn::conditional<tpn::is_same<T1, typename tpn::decay<T1>::type>::value && tpn::is_same<T2, typename tpn::decay<T2>::type>::value,
                           private_common_type::common_type_2_impl<T1, T2>,
                           common_type<typename tpn::decay<T2>::type,
                                       typename tpn::decay<T2>::type>>::type
    {
    };

    //***********************************
    // Three or more types
    namespace private_common_type
    {
        template <typename AlwaysVoid, typename T1, typename T2, typename... TRest>
        struct common_type_multi_impl
        {
        };

        template <typename T1, typename T2, typename... TRest>
        struct common_type_multi_impl<void_t<typename common_type<T1, T2>::type>, T1, T2, TRest...>
            : common_type<typename common_type<T1, T2>::type, TRest...>
        {
        };
    }

    template <typename T1, typename T2, typename... TRest>
    struct common_type<T1, T2, TRest...>
        : private_common_type::common_type_multi_impl<void, T1, T2, TRest...>
    {
    };

    template <typename... T>
    using common_type_t = typename common_type<T...>::type;
#endif

    //***************************************************************************
    /// Defines one of five unsigned types that has the same size as T.
    //***************************************************************************
    template <typename T>
    struct unsigned_type
    {
        typedef typename tpn::conditional<sizeof(T) == sizeof(unsigned char), unsigned char,
                                          typename tpn::conditional<sizeof(T) == sizeof(unsigned short), unsigned short,
                                                                    typename tpn::conditional<sizeof(T) == sizeof(unsigned int), unsigned int,
                                                                                              typename tpn::conditional<sizeof(T) == sizeof(unsigned long), unsigned long,
                                                                                                                        unsigned long long>::type>::type>::type>::type type;
    };

#if TYPHOON_USING_CPP11
    template <typename T>
    using unsigned_type_t = typename unsigned_type<T>::type;
#endif

    //***************************************************************************
    /// Defines one of five signed types that has the same size as T.
    //***************************************************************************
    template <typename T>
    struct signed_type
    {
        typedef typename tpn::conditional<sizeof(T) == sizeof(char), char,
                                          typename tpn::conditional<sizeof(T) == sizeof(short), short,
                                                                    typename tpn::conditional<sizeof(T) == sizeof(int), int,
                                                                                              typename tpn::conditional<sizeof(T) == sizeof(long), long,
                                                                                                                        long long>::type>::type>::type>::type type;
    };

#if TYPHOON_USING_CPP11
    template <typename T>
    using signed_type_t = typename signed_type<T>::type;
#endif
}

// Helper macros
#define TYPHOON_IS_CHAR_TYPE(type) (tpn::is_same<char, type>::value || tpn::is_same<signed char, type>::value || tpn::is_same<unsigned char, type>::value)
#define TYPHOON_IS_NOT_CHAR_TYPE(type) (!TYPHOON_IS_CHAR_TYPE(type))

#define TYPHOON_IS_POINTER_TYPE(type) (tpn::is_pointer<type>::value)
#define TYPHOON_IS_NOT_POINTER_TYPE(type) (!TYPHOON_IS_POINTER_TYPE(type))

#define TYPHOON_TARGET_IS_TRIVIALLY_COPYABLE(type) (tpn::is_trivially_copyable<typename tpn::iterator_traits<type>::value_type>::value)
#define TYPHOON_TARGET_IS_NOT_TRIVIALLY_COPYABLE(type) (!TYPHOON_TARGET_IS_TRIVIALLY_COPYABLE(type))

#endif // TYPHOON_TYPE_TRAITS_HPP
