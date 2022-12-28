///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tphn
https://www.tphncpp.com

Copyright(c) 2019 John Wellbelove

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

#ifndef TYPHOON_CHOOSE_NAMESPACE_HPP
#define TYPHOON_CHOOSE_NAMESPACE_HPP

#undef TYPHOON_OR_STD
#undef TYPHOON_OR_STD11

#if TYPHOON_NOT_USING_STL && !defined(TYPHOON_IN_UNIT_TEST)
  // If we're not using the STL and we are not unit testing, then use the TYPHOON's definitions under the tphn namespace
  #define TYPHOON_OR_STD tphn
  #define TYPHOON_USING_STD_NAMESPACE 0
  #define TYPHOON_USING_TYPHOON_NAMESPACE 1
#else
  // We will use the STL's definitions under the std namespace
  #define TYPHOON_OR_STD std
  #define TYPHOON_USING_STD_NAMESPACE 1
  #define TYPHOON_USING_TYPHOON_NAMESPACE 0
#endif

#if (TYPHOON_NOT_USING_STL && !defined(TYPHOON_IN_UNIT_TEST)) || TYPHOON_CPP11_NOT_SUPPORTED
  // If we're not using the STL and we are not unit testing or C++11 is not supported, then use the TYPHOON's definitions under the tphn namespace
  #define TYPHOON_OR_STD11 tphn
  #define TYPHOON_USING_STD11_NAMESPACE 0
  #define TYPHOON_USING_TYPHOON11_NAMESPACE 1
#else
  // We will use the STL's definitions under the std namespace
  #define TYPHOON_OR_STD11 std
  #define TYPHOON_USING_STD11_NAMESPACE 1
  #define TYPHOON_USING_TYPHOON11_NAMESPACE 0
#endif

#endif
