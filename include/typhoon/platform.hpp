///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tpn
https://www.tpncpp.com

Copyright(c) 2016 John Wellbelove

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

#ifndef TYPHOON_PLATFORM_HPP
#define TYPHOON_PLATFORM_HPP

//*************************************
// Enable all limit macros
// Note: This macro must be defined before the first include of stdint.h
#if !defined(__STDC_LIMIT_MACROS)
  #define __STDC_LIMIT_MACROS
#endif

//*************************************
// Enable all constant macros
// Note: This macro must be defined before the first include of stdint.h
#if !defined(__STDC_CONSTANT_MACROS)
  #define __STDC_CONSTANT_MACROS
#endif

#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#include "file_error_numbers.hpp"

//*************************************
// Include the user's profile definition.
#if !defined(TYPHOON_NO_PROFILE_HEADER) && defined(__has_include)
  #if !__has_include("tpn_profile.hpp")
    #define TYPHOON_NO_PROFILE_HEADER
  #endif
#endif

#if !defined(TYPHOON_NO_PROFILE_HEADER)
  #include "tpn_profile.hpp"
#endif

// Determine the bit width of the platform.
#define TYPHOON_PLATFORM_16BIT (UINT16_MAX == UINTPTR_MAX)
#define TYPHOON_PLATFORM_32BIT (UINT32_MAX == UINTPTR_MAX)
#define TYPHOON_PLATFORM_64BIT (UINT64_MAX == UINTPTR_MAX)

//*************************************
// Define debug macros.
#if (defined(_DEBUG) || defined(DEBUG)) && !defined(TYPHOON_DEBUG) 
  #define TYPHOON_DEBUG
#endif

#if defined(TYPHOON_DEBUG)
  #define TYPHOON_IS_DEBUG_BUILD 1
#else
  #define TYPHOON_IS_DEBUG_BUILD 0
#endif

//*************************************
// Helper macros, so we don't have to use double negatives.
// The TYPHOON will use the STL, unless TYPHOON_NO_STL is defined.
// With this macro we can use '#if TYPHOON_USING_STL' instead of '#if !TYPHOON_NO_STL' in the code.
#if defined(TYPHOON_NO_STL)
  #define TYPHOON_USING_STL     0
  #define TYPHOON_NOT_USING_STL 1
#else
  #define TYPHOON_USING_STL     1
  #define TYPHOON_NOT_USING_STL 0
#endif

//*************************************
// Helper macros for TYPHOON_STLPORT.
#if defined(TYPHOON_STLPORT)
  #define TYPHOON_USING_STLPORT     1
  #define TYPHOON_NOT_USING_STLPORT 0
#else
  #define TYPHOON_USING_STLPORT     0
  #define TYPHOON_NOT_USING_STLPORT 1
#endif

//*************************************
// Some targets do not support 8bit types.
#if (CHAR_BIT == 8)
  #define TYPHOON_USING_8BIT_TYPES     1
  #define TYPHOON_NOT_USING_8BIT_TYPES 0
#else
  #define TYPHOON_USING_8BIT_TYPES     0
  #define TYPHOON_NOT_USING_8BIT_TYPES 1
#endif

#define TYPHOON_8BIT_SUPPORT (CHAR_BIT == 8) // Deprecated

//*************************************
// Helper macro for TYPHOON_NO_64BIT_TYPES.
#if defined(TYPHOON_NO_64BIT_TYPES)
  #define TYPHOON_USING_64BIT_TYPES     0
  #define TYPHOON_NOT_USING_64BIT_TYPES 1
#else
  #define TYPHOON_USING_64BIT_TYPES     1
  #define TYPHOON_NOT_USING_64BIT_TYPES 0
#endif

//*************************************
// Figure out things about the compiler, if haven't already done so in tpn_profile.h
#include "profiles/determine_compiler_version.hpp"
#include "profiles/determine_compiler_language_support.hpp"

//*************************************
// See if we can determine the OS we're compiling on, if haven't already done so in tpn_profile.h
#include "profiles/determine_development_os.hpp"

//*************************************
// Check WCHAR_MIN and WCHAR_MAX
#if !defined(WCHAR_MIN)
  #define WCHAR_MIN 0x0000
#endif

#if !defined(WCHAR_MAX)
  #define WCHAR_MAX 0xFFFF
#endif

//*************************************
// Option to force string construction from a character pointer to be explicit.
#if defined(TYPHOON_FORCE_EXPLICIT_STRING_CONVERSION_FROM_CHAR)
  #define TYPHOON_EXPLICIT_STRING_FROM_CHAR explicit
#else
  #define TYPHOON_EXPLICIT_STRING_FROM_CHAR
#endif

//*************************************
// Option to disable truncation checks for strings.
#if defined(TYPHOON_DISABLE_STRING_TRUNCATION_CHECKS)
  #define TYPHOON_HAS_STRING_TRUNCATION_CHECKS 0
#else
  #define TYPHOON_HAS_STRING_TRUNCATION_CHECKS 1
#endif

//*************************************
// Option to disable clear-after-use functionality for strings.
#if defined(TYPHOON_DISABLE_STRING_CLEAR_AFTER_USE)
  #define TYPHOON_HAS_STRING_CLEAR_AFTER_USE 0
#else
  #define TYPHOON_HAS_STRING_CLEAR_AFTER_USE 1
#endif

//*************************************
// Option to make string truncation an error.
#if defined(TYPHOON_ENABLE_ERROR_ON_STRING_TRUNCATION)
  #define TYPHOON_HAS_ERROR_ON_STRING_TRUNCATION 1
#else
  #define TYPHOON_HAS_ERROR_ON_STRING_TRUNCATION 0
#endif

//*************************************
// Option to enable repair-after-memcpy for istrings.
#if defined(TYPHOON_ISTRING_REPAIR_ENABLE)
  #define TYPHOON_HAS_ISTRING_REPAIR 1
#else
  #define TYPHOON_HAS_ISTRING_REPAIR 0
#endif

//*************************************
// Option to enable repair-after-memcpy for ivector.
#if defined(TYPHOON_IVECTOR_REPAIR_ENABLE)
  #define TYPHOON_HAS_IVECTOR_REPAIR 1
#else
  #define TYPHOON_HAS_IVECTOR_REPAIR 0
#endif

//*************************************
// Option to enable repair-after-memcpy for ideque.
#if defined(TYPHOON_IDEQUE_REPAIR_ENABLE)
  #define TYPHOON_HAS_IDEQUE_REPAIR 1
#else
  #define TYPHOON_HAS_IDEQUE_REPAIR 0
#endif

//*************************************
// Indicate if C++ exceptions are enabled.
#if defined(TYPHOON_THROW_EXCEPTIONS)
  #define TYPHOON_USING_EXCEPTIONS 1
#else
  #define TYPHOON_USING_EXCEPTIONS 0
#endif

//*************************************
// Indicate if nullptr is used.
#if TYPHOON_NO_NULLPTR_SUPPORT
  #define TYPHOON_HAS_NULLPTR 0
#else
  #define TYPHOON_HAS_NULLPTR 1
#endif

//*************************************
// Indicate if legacy bitset is used.
#if defined(TYPHOON_USE_LEGACY_BITSET)
  #define TYPHOON_USING_LEGACY_BITSET 1
#else
  #define TYPHOON_USING_LEGACY_BITSET 0
#endif

//*************************************
// Indicate if array_view is mutable.
#if defined(TYPHOON_ARRAY_VIEW_IS_MUTABLE)
  #define TYPHOON_HAS_MUTABLE_ARRAY_VIEW 1
#else
  #define TYPHOON_HAS_MUTABLE_ARRAY_VIEW 0
#endif

//*************************************
// The macros below are dependent on the profile.
// C++11
#if TYPHOON_USING_CPP11 && !defined(TYPHOON_FORCE_NO_ADVANCED_CPP)
  #define TYPHOON_CONSTEXPR                   constexpr
  #define TYPHOON_CONSTANT                    constexpr
  #define TYPHOON_STATIC_CONSTANT             constexpr
  #define TYPHOON_DELETE                      = delete
  #define TYPHOON_EXPLICIT                    explicit
  #define TYPHOON_OVERRIDE                    override
  #define TYPHOON_FINAL                       final
  #define TYPHOON_NORETURN                    [[noreturn]]
  #define TYPHOON_MOVE(x)                     tpn::move(x)
  #define TYPHOON_ENUM_CLASS(name)            enum class name
  #define TYPHOON_ENUM_CLASS_TYPE(name, type) enum class name : type

  #if TYPHOON_USING_EXCEPTIONS
    #define TYPHOON_NOEXCEPT                  noexcept
    #define TYPHOON_NOEXCEPT_EXPR(expression) noexcept(expression)
  #else
    #define TYPHOON_NOEXCEPT
    #define TYPHOON_NOEXCEPT_EXPR(expression)
  #endif
#else
  #define TYPHOON_CONSTEXPR
  #define TYPHOON_CONSTANT                    const
  #define TYPHOON_STATIC_CONSTANT             static const
  #define TYPHOON_DELETE
  #define TYPHOON_EXPLICIT
  #define TYPHOON_OVERRIDE
  #define TYPHOON_FINAL
  #define TYPHOON_NORETURN
  #define TYPHOON_NOEXCEPT
  #define TYPHOON_NOEXCEPT_EXPR(expression)
  #define TYPHOON_MOVE(x) x
  #define TYPHOON_ENUM_CLASS(name)            enum name
  #define TYPHOON_ENUM_CLASS_TYPE(name, type) enum name
#endif

//*************************************
// C++14
#if TYPHOON_USING_CPP14 && !defined(TYPHOON_FORCE_NO_ADVANCED_CPP)
  #define TYPHOON_CONSTEXPR14               constexpr
  #define TYPHOON_DEPRECATED                [[deprecated]]
  #define TYPHOON_DEPRECATED_REASON(reason) [[deprecated(reason)]]
#else
  #define TYPHOON_CONSTEXPR14
  #define TYPHOON_DEPRECATED
  #define TYPHOON_DEPRECATED_REASON(reason)
#endif

//*************************************
// C++17
#if TYPHOON_USING_CPP17 && !defined(TYPHOON_FORCE_NO_ADVANCED_CPP)
  #define TYPHOON_CONSTEXPR17  constexpr
  #define TYPHOON_IF_CONSTEXPR constexpr
  #define TYPHOON_NODISCARD    [[nodiscard]]
  #define TYPHOON_MAYBE_UNUSED [[maybe_unused]]
  #define TYPHOON_FALLTHROUGH  [[fallthrough]]
  #define TYPHOON_INLINE_VAR   inline
#else
  #define TYPHOON_CONSTEXPR17
  #define TYPHOON_IF_CONSTEXPR
  #define TYPHOON_NODISCARD
  #define TYPHOON_MAYBE_UNUSED
  #define TYPHOON_FALLTHROUGH
  #define TYPHOON_INLINE_VAR
#endif

//*************************************
// C++20
#if TYPHOON_USING_CPP20 && !defined(TYPHOON_FORCE_NO_ADVANCED_CPP)
  #define TYPHOON_LIKELY            [[likely]]
  #define TYPHOON_UNLIKELY          [[unlikely]]
  #define TYPHOON_CONSTEXPR20       constexpr
  #define TYPHOON_CONSTEVAL         consteval
  #define TYPHOON_CONSTINIT         constinit
  #define TYPHOON_NO_UNIQUE_ADDRESS [[no_unique_address]]
#else
  #define TYPHOON_LIKELY
  #define TYPHOON_UNLIKELY
  #define TYPHOON_CONSTEXPR20
  #define TYPHOON_CONSTEVAL
  #define TYPHOON_CONSTINIT
  #define TYPHOON_NO_UNIQUE_ADDRESS
#endif

//*************************************
// Determine if the TYPHOON can use char8_t type.
#if TYPHOON_USING_8BIT_TYPES
  #if TYPHOON_NO_SMALL_CHAR_SUPPORT
    typedef int8_t char8_t;
    #define TYPHOON_HAS_CHAR8_T 1
    #define TYPHOON_HAS_NATIVE_CHAR8_T 0
  #else
    #define TYPHOON_HAS_CHAR8_T 1
    #define TYPHOON_HAS_NATIVE_CHAR8_T 1
  #endif
#else
  #define TYPHOON_HAS_CHAR8_T 0
  #define TYPHOON_HAS_NATIVE_CHAR8_T 0
#endif

//*************************************
// Define the large character types if necessary.
#if TYPHOON_NO_LARGE_CHAR_SUPPORT
  typedef int16_t char16_t;
  typedef int32_t char32_t;
  #define TYPHOON_HAS_NATIVE_CHAR16_T 0
  #define TYPHOON_HAS_NATIVE_CHAR32_T 0
#else
  #define TYPHOON_HAS_NATIVE_CHAR16_T 1
  #define TYPHOON_HAS_NATIVE_CHAR32_T 1
#endif

//*************************************
// Determine if the TYPHOON can use std::array
#if !defined(TYPHOON_HAS_STD_ARRAY)
  #if TYPHOON_USING_STL && TYPHOON_USING_CPP11
    #define TYPHOON_HAS_STD_ARRAY 1
  #else
    #define TYPHOON_HAS_STD_ARRAY 0
  #endif
#endif

//*************************************
// Determine if the TYPHOON should support atomics.
#if defined(TYPHOON_NO_ATOMICS) || \
    defined(TYPHOON_TARGET_DEVICE_ARM_CORTEX_M0) || \
    defined(TYPHOON_TARGET_DEVICE_ARM_CORTEX_M0_PLUS) || \
    defined(__STDC_NO_ATOMICS__)
  #define TYPHOON_HAS_ATOMIC 0
#else
  #if ((TYPHOON_USING_CPP11 && (TYPHOON_USING_STL || defined(TYPHOON_IN_UNIT_TEST))) || \
        defined(TYPHOON_COMPILER_ARM5)  || \
        defined(TYPHOON_COMPILER_ARM6)  || \
        defined(TYPHOON_COMPILER_GCC)   || \
        defined(TYPHOON_COMPILER_CLANG))
    #define TYPHOON_HAS_ATOMIC 1
  #else
    #define TYPHOON_HAS_ATOMIC 0
  #endif
#endif

//*************************************
// Determine if the TYPHOON should use std::initializer_list.
#if (defined(TYPHOON_FORCE_TYPHOON_INITIALIZER_LIST) && defined(TYPHOON_FORCE_STD_INITIALIZER_LIST))
  #error TYPHOON_FORCE_TYPHOON_INITIALIZER_LIST and TYPHOON_FORCE_STD_INITIALIZER_LIST both been defined. Choose one or neither.
#endif

#if (TYPHOON_USING_CPP11 && !defined(TYPHOON_NO_INITIALIZER_LIST))
  // Use the compiler's std::initializer_list?
  #if (TYPHOON_USING_STL && TYPHOON_NOT_USING_STLPORT && !defined(TYPHOON_FORCE_TYPHOON_INITIALIZER_LIST)) || defined(TYPHOON_IN_UNIT_TEST) || defined(TYPHOON_FORCE_STD_INITIALIZER_LIST)
    #define TYPHOON_HAS_INITIALIZER_LIST 1
  #else
    // Use the TYPHOON's compatible version?
    #if defined(TYPHOON_COMPILER_MICROSOFT) || defined(TYPHOON_COMPILER_GCC)  || defined(TYPHOON_COMPILER_CLANG) || \
        defined(TYPHOON_COMPILER_ARM6) || defined(TYPHOON_COMPILER_ARM7) || defined(TYPHOON_COMPILER_IAR)   || \
        defined(TYPHOON_COMPILER_TEXAS_INSTRUMENTS) || defined(TYPHOON_COMPILER_INTEL)
      #define TYPHOON_HAS_INITIALIZER_LIST 1
    #else
      #define TYPHOON_HAS_INITIALIZER_LIST 0
    #endif
  #endif
#else
  #define TYPHOON_HAS_INITIALIZER_LIST 0
#endif

//*************************************
// Check for availability of certain builtins
#include "profiles/determine_builtin_support.hpp"

//*************************************
// Sort out namespaces for STL/No STL options.
#include "private/choose_namespace.hpp"

namespace tpn
{
  namespace traits
  {
    // Documentation: https://www.tpncpp.com/tpn_traits.html
    // General
    static TYPHOON_CONSTANT long cplusplus                        = __cplusplus;
    static TYPHOON_CONSTANT int  language_standard                = TYPHOON_LANGUAGE_STANDARD;

    // Using...
    static TYPHOON_CONSTANT bool using_stl                        = (TYPHOON_USING_STL == 1);
    static TYPHOON_CONSTANT bool using_stlport                    = (TYPHOON_USING_STLPORT == 1);
    static TYPHOON_CONSTANT bool using_cpp11                      = (TYPHOON_USING_CPP11 == 1);
    static TYPHOON_CONSTANT bool using_cpp14                      = (TYPHOON_USING_CPP14 == 1);
    static TYPHOON_CONSTANT bool using_cpp17                      = (TYPHOON_USING_CPP17 == 1);
    static TYPHOON_CONSTANT bool using_cpp20                      = (TYPHOON_USING_CPP20 == 1);
    static TYPHOON_CONSTANT bool using_cpp23                      = (TYPHOON_USING_CPP23 == 1);
    static TYPHOON_CONSTANT bool using_gcc_compiler               = (TYPHOON_USING_GCC_COMPILER == 1);
    static TYPHOON_CONSTANT bool using_microsoft_compiler         = (TYPHOON_USING_MICROSOFT_COMPILER == 1);
    static TYPHOON_CONSTANT bool using_arm5_compiler              = (TYPHOON_USING_ARM5_COMPILER == 1);
    static TYPHOON_CONSTANT bool using_arm6_compiler              = (TYPHOON_USING_ARM6_COMPILER == 1);
    static TYPHOON_CONSTANT bool using_arm7_compiler              = (TYPHOON_USING_ARM7_COMPILER == 1);
    static TYPHOON_CONSTANT bool using_clang_compiler             = (TYPHOON_USING_CLANG_COMPILER == 1);
    static TYPHOON_CONSTANT bool using_green_hills_compiler       = (TYPHOON_USING_GREEN_HILLS_COMPILER == 1);
    static TYPHOON_CONSTANT bool using_iar_compiler               = (TYPHOON_USING_IAR_COMPILER == 1);
    static TYPHOON_CONSTANT bool using_intel_compiler             = (TYPHOON_USING_INTEL_COMPILER == 1);
    static TYPHOON_CONSTANT bool using_texas_instruments_compiler = (TYPHOON_USING_TEXAS_INSTRUMENTS_COMPILER == 1);
    static TYPHOON_CONSTANT bool using_generic_compiler           = (TYPHOON_USING_GENERIC_COMPILER == 1);
    static TYPHOON_CONSTANT bool using_legacy_bitset              = (TYPHOON_USING_LEGACY_BITSET == 1);
    static TYPHOON_CONSTANT bool using_exceptions                 = (TYPHOON_USING_EXCEPTIONS == 1);
    
    // Has...
    static TYPHOON_CONSTANT bool has_initializer_list             = (TYPHOON_HAS_INITIALIZER_LIST == 1);
    static TYPHOON_CONSTANT bool has_8bit_types                   = (TYPHOON_USING_8BIT_TYPES == 1);
    static TYPHOON_CONSTANT bool has_64bit_types                  = (TYPHOON_USING_64BIT_TYPES == 1);
    static TYPHOON_CONSTANT bool has_atomic                       = (TYPHOON_HAS_ATOMIC == 1);
    static TYPHOON_CONSTANT bool has_nullptr                      = (TYPHOON_HAS_NULLPTR == 1);
    static TYPHOON_CONSTANT bool has_char8_t                      = (TYPHOON_HAS_CHAR8_T == 1);
    static TYPHOON_CONSTANT bool has_native_char8_t               = (TYPHOON_HAS_NATIVE_CHAR8_T == 1);
    static TYPHOON_CONSTANT bool has_native_char16_t              = (TYPHOON_HAS_NATIVE_CHAR16_T == 1);
    static TYPHOON_CONSTANT bool has_native_char32_t              = (TYPHOON_HAS_NATIVE_CHAR32_T == 1);
    static TYPHOON_CONSTANT bool has_string_truncation_checks     = (TYPHOON_HAS_STRING_TRUNCATION_CHECKS == 1);
    static TYPHOON_CONSTANT bool has_error_on_string_truncation   = (TYPHOON_HAS_ERROR_ON_STRING_TRUNCATION == 1);
    static TYPHOON_CONSTANT bool has_string_clear_after_use       = (TYPHOON_HAS_STRING_CLEAR_AFTER_USE == 1);
    static TYPHOON_CONSTANT bool has_istring_repair               = (TYPHOON_HAS_ISTRING_REPAIR == 1);
    static TYPHOON_CONSTANT bool has_ivector_repair               = (TYPHOON_HAS_IVECTOR_REPAIR == 1);
    static TYPHOON_CONSTANT bool has_mutable_array_view           = (TYPHOON_HAS_MUTABLE_ARRAY_VIEW == 1);
    static TYPHOON_CONSTANT bool has_ideque_repair                = (TYPHOON_HAS_IDEQUE_REPAIR == 1);

    // Is...
    static TYPHOON_CONSTANT bool is_debug_build                   = (TYPHOON_IS_DEBUG_BUILD == 1);
   
  }
}

#endif
