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

#ifndef TYPHOON_CRC64_ECMA_HPP
#define TYPHOON_CRC64_ECMA_HPP

#include "platform.hpp"
#include "private/crc_implementation.hpp"

///\defgroup crc64_ecma 64 bit ECMA CRC calculation
///\ingroup crc

namespace tpn
{
#if TYPHOON_USING_CPP11
  template <size_t Table_Size>
  using crc64_ecma_t = tpn::crc_type<tpn::private_crc::crc64_ecma_parameters, Table_Size>;
#else
  template <size_t Table_Size>
  class crc64_ecma_t : public tpn::crc_type<tpn::private_crc::crc64_ecma_parameters, Table_Size>
  {
  public:

    //*************************************************************************
    /// Default constructor.
    //*************************************************************************
    crc64_ecma_t()
    {
      this->reset();
    }

    //*************************************************************************
    /// Constructor from range.
    /// \param begin Start of the range.
    /// \param end   End of the range.
    //*************************************************************************
    template<typename TIterator>
    crc64_ecma_t(TIterator begin, const TIterator end)
    {
      this->reset();
      this->add(begin, end);
    }
  };
#endif

  typedef tpn::crc64_ecma_t<256U> crc64_ecma_t256;
  typedef tpn::crc64_ecma_t<16U>  crc64_ecma_t16;
  typedef tpn::crc64_ecma_t<4U>   crc64_ecma_t4;
  typedef crc64_ecma_t256         crc64_ecma;
}
#endif
