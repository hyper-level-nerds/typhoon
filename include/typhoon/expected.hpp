///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tpn
https://www.tpncpp.com

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

namespace tpn
{
  //***************************************************************************
  /// Unexpected type.
  /// tpn::unexpected represents an unexpected value stored in tpn::expected.
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
      : error_value(tpn::move(other.error_value))
    {
    }
#endif

#if TYPHOON_USING_CPP11
    //*******************************************
    /// Construct from argument.
    //*******************************************
    template <typename TErr = TError, typename = typename tpn::enable_if<!tpn::is_same<typename tpn::remove_cvref<TErr>::type, tpn::unexpected<TErr>>::value &&
                                                                         !tpn::is_same<typename tpn::remove_cvref<TErr>::type, tpn::in_place_t>::value, int>::type>
    constexpr explicit unexpected(TErr&& e)
      : error_value(tpn::forward<TErr>(e))
    {
    }
#else
    //*******************************************
    /// Construct from argument.
    //*******************************************
    template <typename TErr>
    explicit unexpected(const TErr& e, typename tpn::enable_if<!tpn::is_same<typename tpn::remove_cvref<TErr>::type, tpn::unexpected<TErr> >::value &&
                                                               !tpn::is_same<typename tpn::remove_cvref<TErr>::type, tpn::in_place_t>::value, int>::type = 0)
      : error_value(e)
    {
    }
#endif

#if TYPHOON_USING_CPP11
    //*******************************************
    /// Construct from arguments.
    //*******************************************
    template <typename... Args >
    constexpr explicit unexpected(tpn::in_place_t, Args&&... args)
      : error_value(tpn::forward<Args>(args)...)
    {
    }
#endif

#if TYPHOON_HAS_INITIALIZER_LIST
    //*******************************************
    /// Construct from initializer_list and arguments.
    //*******************************************
    template <typename U, typename... Args>
    constexpr explicit unexpected(tpn::in_place_t, std::initializer_list<U> init, Args&&... args)
      : error_value(init, tpn::forward<Args>(args)...)
    {
    }
#endif

    //*******************************************
    /// Assign from tpn::unexpected.
    //*******************************************
    TYPHOON_CONSTEXPR14
    tpn::unexpected<TError>& operator =(const tpn::unexpected<TError>& rhs)
    {
      error_value = rhs.error_value;

      return *this;
    }

#if TYPHOON_USING_CPP11
    //*******************************************
    /// Move assign from tpn::unexpected.
    //*******************************************
    TYPHOON_CONSTEXPR14
    tpn::unexpected<TError>& operator =(tpn::unexpected<TError>&& rhs)
    {
      error_value = tpn::move(rhs.error_value);

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
      return tpn::move(error_value);
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
      return tpn::move(error_value);
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
      return tpn::move(error_value);
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
    /// Swap with another tpn::unexpected.
    //*******************************************
    void swap(tpn::unexpected<TError>& other)
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
bool operator ==(const tpn::unexpected<TError>& lhs, const tpn::unexpected<TError>& rhs)
{
  return lhs.error_value == rhs.error_value;
}

//*******************************************
/// Swap tpn::unexpected.
//*******************************************
template <typename TError>
TYPHOON_CONSTEXPR14
void swap(tpn::unexpected<TError>& lhs, tpn::unexpected<TError>& rhs)
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
