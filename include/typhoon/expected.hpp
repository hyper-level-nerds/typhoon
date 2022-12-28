///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tphn
https://www.tphncpp.com

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

#ifndef TYPHOON_EXPECTED_HPP
#define TYPHOON_EXPECTED_HPP

#include "platform.hpp"
#include "type_traits.hpp"
#include "utility.hpp"

namespace tphn
{
  //***************************************************************************
  /// Unexpected type.
  /// tphn::unexpected represents an unexpected value stored in tphn::expected.
  //***************************************************************************
  template <typename TError>
  class unexpected
  {
  public:

    //*******************************************
    /// Copy constructor.
    //*******************************************
    TYPHOON_CONSTEXPR14 unexpected(const unexpected& other)
      : error_value(other.error_value)
    {
    }

#if TYPHOON_USING_CPP11
    //*******************************************
    /// Move constructor.
    //*******************************************
    TYPHOON_CONSTEXPR unexpected(unexpected&& other)
      : error_value(tphn::move(other.error_value))
    {
    }
#endif

#if TYPHOON_USING_CPP11
    //*******************************************
    /// Construct from argument.
    //*******************************************
    template <typename TErr = TError, typename = typename tphn::enable_if<!tphn::is_same<typename tphn::remove_cvref<TErr>::type, tphn::unexpected<TErr>>::value &&
                                                                         !tphn::is_same<typename tphn::remove_cvref<TErr>::type, tphn::in_place_t>::value, int>::type>
    constexpr explicit unexpected(TErr&& e)
      : error_value(tphn::forward<TErr>(e))
    {
    }
#else
    //*******************************************
    /// Construct from argument.
    //*******************************************
    template <typename TErr>
    explicit unexpected(const TErr& e, typename tphn::enable_if<!tphn::is_same<typename tphn::remove_cvref<TErr>::type, tphn::unexpected<TErr> >::value &&
                                                               !tphn::is_same<typename tphn::remove_cvref<TErr>::type, tphn::in_place_t>::value, int>::type = 0)
      : error_value(e)
    {
    }
#endif

#if TYPHOON_USING_CPP11
    //*******************************************
    /// Construct from arguments.
    //*******************************************
    template <typename... Args >
    constexpr explicit unexpected(tphn::in_place_t, Args&&... args)
      : error_value(tphn::forward<Args>(args)...)
    {
    }
#endif

#if TYPHOON_HAS_INITIALIZER_LIST
    //*******************************************
    /// Construct from initializer_list and arguments.
    //*******************************************
    template <typename U, typename... Args>
    constexpr explicit unexpected(tphn::in_place_t, std::initializer_list<U> init, Args&&... args)
      : error_value(init, tphn::forward<Args>(args)...)
    {
    }
#endif

    //*******************************************
    /// Assign from tphn::unexpected.
    //*******************************************
    TYPHOON_CONSTEXPR14
    tphn::unexpected<TError>& operator =(const tphn::unexpected<TError>& rhs)
    {
      error_value = rhs.error_value;

      return *this;
    }

#if TYPHOON_USING_CPP11
    //*******************************************
    /// Move assign from tphn::unexpected.
    //*******************************************
    TYPHOON_CONSTEXPR14
    tphn::unexpected<TError>& operator =(tphn::unexpected<TError>&& rhs)
    {
      error_value = tphn::move(rhs.error_value);

      return *this;
    }
#endif

#if TYPHOON_USING_CPP11
    //*******************************************
    /// Get the error.
    //*******************************************
    constexpr const TError& error() const& noexcept
    {
      return error_value;
    }

    //*******************************************
    /// Get the error.
    //*******************************************
    constexpr const TError&& error() const&& noexcept
    {
      return tphn::move(error_value);
    }

#if TYPHOON_USING_CPP14
    //*******************************************
    /// Get the error.
    //*******************************************
    constexpr TError& error() & noexcept
    {
      return error_value;
    }

    //*******************************************
    /// Get the error.
    //*******************************************    
    constexpr TError&& error() && noexcept
    {
      return tphn::move(error_value);
    }
#else
    //*******************************************
    /// Get the error.
    //*******************************************
    TError& error() & noexcept
    {
      return error_value;
    }

    //*******************************************
    /// Get the error.
    //*******************************************    
    TError&& error() && noexcept
    {
      return tphn::move(error_value);
    }
#endif

#else
    //*******************************************
    /// Get the error.
    //*******************************************
    const TError& error() const
    {
      return error_value;
    }
#endif

    //*******************************************
    /// Swap with another tphn::unexpected.
    //*******************************************
    void swap(tphn::unexpected<TError>& other)
    {
      using TYPHOON_OR_STD::swap;

      swap(error_value, other.error_value);
    }

  private:

    TError error_value;
  };

#if TYPHOON_USING_CPP17
  template <typename TError>
  unexpected(TError) -> unexpected<TError>;
#endif
}

//*******************************************
/// Equivalence operator.
//*******************************************
template <typename TError>
TYPHOON_CONSTEXPR14
bool operator ==(const tphn::unexpected<TError>& lhs, const tphn::unexpected<TError>& rhs)
{
  return lhs.error_value == rhs.error_value;
}

//*******************************************
/// Swap tphn::unexpected.
//*******************************************
template <typename TError>
TYPHOON_CONSTEXPR14
void swap(tphn::unexpected<TError>& lhs, tphn::unexpected<TError>& rhs)
{
  lhs.swap(rhs);
}

//*****************************************************************************
/// unexpect_t
//*****************************************************************************
struct unexpect_t
{
  TYPHOON_CONSTEXPR14 explicit unexpect_t()
  {
  }
};

#if TYPHOON_CPP17_SUPPORTED
inline constexpr unexpect_t unexpect{};
#else
static const unexpect_t unexpect;
#endif

#endif
