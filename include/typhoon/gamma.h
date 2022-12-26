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

#ifndef TYPHOON_GAMMA_INCLUDED
#define TYPHOON_GAMMA_INCLUDED

#include "platform.h"
#include "functional.h"
#include "type_traits.h"

#include <math.h>
#include <stdint.h>

namespace tphn
{
  //***************************************************************************
  /// Gamma encode function.
  //***************************************************************************
  template <typename TInput>
  class gamma_encode : public tphn::unary_function<TInput, TInput>
  {
  public:

    //*********************************
    /// Constructor.
    //*********************************
    gamma_encode(double gamma_, TInput maximum_)
      : one_over_gamma(1.0 / gamma_)
      , maximum(maximum_)
    {      
    }

    //*********************************
    /// operator ()
    /// Get the gamma.
    //*********************************
    TInput operator ()(TInput value) const
    {
      return TInput(TInput(maximum * pow(double(value) / maximum, one_over_gamma)));
    }

  private:

    const double one_over_gamma;
    const double maximum;
  };

  //***************************************************************************
  /// Gamma decode function.
  //***************************************************************************
  template <typename TInput>
  class gamma_decode : public tphn::unary_function<TInput, TInput>
  {
  public:

    //*********************************
    /// Constructor.
    //*********************************
    gamma_decode(double gamma_, TInput maximum_)
      : gamma(gamma_)
      , maximum(maximum_)
    {      
    }

    //*********************************
    /// operator ()
    /// Get the gamma.
    //*********************************
    TInput operator ()(TInput value) const
    {
      return TInput(TInput(maximum * pow(double(value) / maximum, gamma)));
    }

  private:

    const double gamma;
    const double maximum;
  };
}

#endif
