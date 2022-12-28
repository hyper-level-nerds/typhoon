///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tphn
https://www.tphncpp.com

Copyright(c) 2020 John Wellbelove

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

#ifndef TYPHOON_SHARED_MESSAGE_HPP
#define TYPHOON_SHARED_MESSAGE_HPP

#include "platform.hpp"
#include "utility.hpp"
#include "reference_counted_message.hpp"
#include "ireference_counted_message_pool.hpp"
#include "message.hpp"
#include "type_traits.hpp"
#include "static_assert.hpp"

//*****************************************************************************
/// A wrapper for reference counted messages.
/// Contains pointers to a pool owner and a message defined with a ref count type.
//*****************************************************************************
namespace tphn
{
  class shared_message
  {
  public:

    //*************************************************************************
    /// Constructor
    //*************************************************************************
    template <typename TPool, typename TMessage>
    shared_message(TPool& owner, const TMessage& message)
    {
      TYPHOON_STATIC_ASSERT((tphn::is_base_of<tphn::ireference_counted_message_pool, TPool>::value), "TPool not derived from tphn::ireference_counted_message_pool");
      TYPHOON_STATIC_ASSERT((tphn::is_base_of<tphn::imessage, TMessage>::value), "TMessage not derived from tphn::imessage");

      p_rcmessage = owner.allocate(message);
      
      if (p_rcmessage != TYPHOON_NULLPTR)
      {
        p_rcmessage->get_reference_counter().set_reference_count(1U);
      }
    }

    //*************************************************************************
    /// Constructor
    //*************************************************************************
    shared_message(tphn::ireference_counted_message& rcm)
    {
      p_rcmessage = &rcm;

      p_rcmessage->get_reference_counter().set_reference_count(1U);
    }

    //*************************************************************************
    /// Copy constructor
    //*************************************************************************
    shared_message(const tphn::shared_message& other)
      : p_rcmessage(other.p_rcmessage)
    {
      p_rcmessage->get_reference_counter().increment_reference_count();
    }

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Move constructor
    //*************************************************************************
    shared_message(tphn::shared_message&& other)
      : p_rcmessage(tphn::move(other.p_rcmessage))
    {
      other.p_rcmessage = TYPHOON_NULLPTR;
    }
#endif

    //*************************************************************************
    /// Copy assignment operator
    //*************************************************************************
    shared_message& operator =(const tphn::shared_message& other)
    {
      if (&other != this)
      {
        // Deal with the current message.
        if (p_rcmessage->get_reference_counter().decrement_reference_count() == 0U)
        {
          p_rcmessage->release();
        }

        // Copy over the new one.
        p_rcmessage = other.p_rcmessage;
        p_rcmessage->get_reference_counter().increment_reference_count();
       }

      return *this;
    }

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Move assignment operator
    //*************************************************************************
    shared_message& operator =(tphn::shared_message&& other)
    {
      if (&other != this)
      {
        // Deal with the current message.
        if (p_rcmessage->get_reference_counter().decrement_reference_count() == 0U)
        {
          p_rcmessage->release();
        }

        // Move over the new one.
        p_rcmessage = tphn::move(other.p_rcmessage);
        other.p_rcmessage = TYPHOON_NULLPTR;
      }

      return *this;
    }
#endif

    //*************************************************************************
    /// Destructor
    /// Returns the message back to the pool it it is the last copy.
    //*************************************************************************
    ~shared_message()
    {
      if ((p_rcmessage != TYPHOON_NULLPTR) &&
          (p_rcmessage->get_reference_counter().decrement_reference_count() == 0U))
      {       
        p_rcmessage->release();
      }
    }

    //*************************************************************************
    /// Get a reference to the contained message.
    //***********************************************************************
    TYPHOON_NODISCARD tphn::imessage& get_message()
    {
      return p_rcmessage->get_message();
    }

    //*************************************************************************
    /// Get a const reference to the contained message.
    //*************************************************************************
    TYPHOON_NODISCARD const tphn::imessage& get_message() const
    {
      return p_rcmessage->get_message();
    }

    //*************************************************************************
    /// Get the current reference count for this shared message.
    //*************************************************************************
    TYPHOON_NODISCARD uint32_t get_reference_count() const
    {
      return p_rcmessage->get_reference_counter().get_reference_count();
    }

    //*************************************************************************
    /// Checks if the shared message is valid.
    //*************************************************************************
    TYPHOON_NODISCARD bool is_valid() const
    {
      return p_rcmessage != TYPHOON_NULLPTR;
    }

  private:

    shared_message() TYPHOON_DELETE;

    tphn::ireference_counted_message* p_rcmessage; ///< A pointer to the reference  counted message.
  };
}

#endif

