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

#ifndef TYPHOON_VERSION_INCLUDED
#define TYPHOON_VERSION_INCLUDED

#include "platform.h"
#include "macros.h"

///\defgroup version version
/// Definitions of the TYPHOON version
///\ingroup utilities

#define TYPHOON_VERSION_MAJOR 20
#define TYPHOON_VERSION_MINOR 35
#define TYPHOON_VERSION_PATCH 8

#define TYPHOON_VERSION       TYPHOON_STRING(TYPHOON_VERSION_MAJOR) "." TYPHOON_STRING(TYPHOON_VERSION_MINOR) "." TYPHOON_STRING(TYPHOON_VERSION_PATCH)
#define TYPHOON_VERSION_W     TYPHOON_WIDE_STRING(TYPHOON_VERSION_MAJOR) L"." TYPHOON_WIDE_STRING(TYPHOON_VERSION_MINOR) L"." TYPHOON_WIDE_STRING(TYPHOON_VERSION_PATCH)
#if TYPHOON_HAS_CHAR8_T
  #define TYPHOON_VERSION_U8  TYPHOON_U8_STRING(TYPHOON_VERSION_MAJOR) u8"." TYPHOON_U8_STRING(TYPHOON_VERSION_MINOR) u8"." TYPHOON_U8_STRING(TYPHOON_VERSION_PATCH)
#endif
  #define TYPHOON_VERSION_U16 TYPHOON_U16_STRING(TYPHOON_VERSION_MAJOR) u"." TYPHOON_U16_STRING(TYPHOON_VERSION_MINOR) u"." TYPHOON_U16_STRING(TYPHOON_VERSION_PATCH)
  #define TYPHOON_VERSION_U32 TYPHOON_U32_STRING(TYPHOON_VERSION_MAJOR) U"." TYPHOON_U32_STRING(TYPHOON_VERSION_MINOR) U"." TYPHOON_U32_STRING(TYPHOON_VERSION_PATCH)
#define TYPHOON_VERSION_VALUE ((TYPHOON_VERSION_MAJOR * 10000) + (TYPHOON_VERSION_MINOR * 100) + TYPHOON_VERSION_PATCH)

namespace tphn
{
  namespace traits
  {
    static TYPHOON_CONSTANT long version                   = TYPHOON_VERSION_VALUE;
    static TYPHOON_CONSTANT long version_major             = TYPHOON_VERSION_MAJOR;
    static TYPHOON_CONSTANT long version_minor             = TYPHOON_VERSION_MINOR;
    static TYPHOON_CONSTANT long version_patch             = TYPHOON_VERSION_PATCH;

#if TYPHOON_USING_CPP11
    static constexpr const char* version_string        = TYPHOON_VERSION;
    static constexpr const wchar_t*  version_wstring   = TYPHOON_VERSION_W;
  #if TYPHOON_HAS_NATIVE_CHAR8_T
    static constexpr const char8_t*  version_u8string  = TYPHOON_VERSION_U8;
  #endif
    static constexpr const char16_t* version_u16string = TYPHOON_VERSION_U16;
    static constexpr const char32_t* version_u32string = TYPHOON_VERSION_U32;
#else
    static const char*               version_string    = TYPHOON_VERSION;
    static const wchar_t*            version_wstring   = TYPHOON_VERSION_W;
#endif
  }
}

#endif

