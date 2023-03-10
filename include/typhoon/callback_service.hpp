///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tpn
https://www.tpncpp.com

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

#ifndef TYPHOON_CALLBACK_SERVICE_HPP
#define TYPHOON_CALLBACK_SERVICE_HPP

#include "platform.hpp"
#include "nullptr.hpp"
#include "static_assert.hpp"
#include "function.hpp"
#include "array.hpp"

namespace tpn
{
  //***************************************************************************
  /// An indexed callback service.
  /// \tparam RANGE  The number of callbacks to handle.
  /// \tparam OFFSET The lowest callback id value.
  /// The callback ids must range between OFFSET and OFFSET + RANGE - 1.
  //***************************************************************************
  template <const size_t RANGE, const size_t OFFSET = 0U>
  class callback_service
  {
  public:

    //*************************************************************************
    /// Reset the callback service.
    /// Sets all callbacks to the internal default.
    //*************************************************************************
    callback_service()
      : unhandled_callback(*this),
        p_unhandled(TYPHOON_NULLPTR)
    {
      lookup.fill(&unhandled_callback);
    }

    //*************************************************************************
    /// Registers a callback for the specified id.
    /// Compile time assert if the id is out of range.
    /// \tparam ID The id of the callback.
    /// \param callback Reference to the callback.
    //*************************************************************************
    template <const size_t ID>
    void register_callback(tpn::ifunction<size_t>& callback)
    {
      TYPHOON_STATIC_ASSERT(ID < (OFFSET + RANGE), "Callback Id out of range");
      TYPHOON_STATIC_ASSERT(ID >= OFFSET,          "Callback Id out of range");

      lookup[ID - OFFSET] = &callback;
    }

    //*************************************************************************
    /// Registers a callback for the specified id.
    /// No action if the id is out of range.
    /// \param id       Id of the callback.
    /// \param callback Reference to the callback.
    //*************************************************************************
    void register_callback(const size_t id, tpn::ifunction<size_t>& callback)
    {
      if ((id >= OFFSET) && (id < (OFFSET + RANGE)))
      {
        lookup[id - OFFSET] = &callback;
      }
    }

    //*************************************************************************
    /// Registers an alternative callback for unhandled ids.
    /// \param callback A reference to the user supplied 'unhandled' callback.
    //*************************************************************************
    void register_unhandled_callback(tpn::ifunction<size_t>& callback)
    {
      p_unhandled = &callback;
    }

    //*************************************************************************
    /// Executes the callback function for the index.
    /// Compile time assert if the id is out of range.
    /// \tparam ID The id of the callback.
    //*************************************************************************
    template <const size_t ID>
    void callback()
    {
      TYPHOON_STATIC_ASSERT(ID < (OFFSET + RANGE), "Callback Id out of range");
      TYPHOON_STATIC_ASSERT(ID >= OFFSET,          "Callback Id out of range");

      (*lookup[ID - OFFSET])(ID);
    }

    //*************************************************************************
    /// Executes the callback function for the index.
    /// \param id Id of the callback.
    //*************************************************************************
    void callback(const size_t id)
    {
      if ((id >= OFFSET) && (id < (OFFSET + RANGE)))
      {
        (*lookup[id - OFFSET])(id);
      }
      else
      {
        unhandled(id);
      }
    }

  private:

    //*************************************************************************
    /// The default callback function.
    /// Calls the user defined 'unhandled' callback if it exists.
    //*************************************************************************
    void unhandled(size_t id)
    {
      if (p_unhandled != TYPHOON_NULLPTR)
      {
        (*p_unhandled)(id);
      }
    }

    /// The default callback for unhandled ids.
    tpn::function_mp<callback_service<RANGE, OFFSET>,
                     size_t,
                     &callback_service<RANGE, OFFSET>::unhandled> unhandled_callback;

    /// Pointer to the user defined 'unhandled' callback.
    tpn::ifunction<size_t>* p_unhandled;

    /// Lookup table of callbacks.
    tpn::array<tpn::ifunction<size_t>*, RANGE> lookup;
  };
}

#endif
