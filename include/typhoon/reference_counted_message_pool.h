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

#ifndef TYPHOON_REFERENCE_COUNTED_MESSAGE_POOL_INCLUDED
#define TYPHOON_REFERENCE_COUNTED_MESSAGE_POOL_INCLUDED

#include "platform.h"
#include "message.h"
#include "imemory_block_allocator.h"
#include "ireference_counted_message_pool.h"
#include "reference_counted_message.h"
#include "static_assert.h"
#include "error_handler.h"
#include "utility.h"
#include "atomic.h"
#include "memory.h"
#include "largest.h"

namespace tphn
{
  //***************************************************************************
  /// Exception type for tphn::reference_counted_message_pool
  //***************************************************************************
  class reference_counted_message_pool_exception : public tphn::exception
  {
  public:

    reference_counted_message_pool_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Exception if the allocation failed.
  //***************************************************************************
  class reference_counted_message_pool_allocation_failure : public tphn::reference_counted_message_pool_exception
  {
  public:

    reference_counted_message_pool_allocation_failure(string_type file_name_, numeric_type line_number_)
      : reference_counted_message_pool_exception(TYPHOON_ERROR_TEXT("reference_counted_message_pool:allocation failure", TYPHOON_REFERENCE_COUNTER_MESSAGE_POOL_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Exception if the release failed.
  //***************************************************************************
  class reference_counted_message_pool_release_failure : public tphn::reference_counted_message_pool_exception
  {
  public:

    reference_counted_message_pool_release_failure(string_type file_name_, numeric_type line_number_)
      : reference_counted_message_pool_exception(TYPHOON_ERROR_TEXT("reference_counted_message_pool:release failure", TYPHOON_REFERENCE_COUNTER_MESSAGE_POOL_FILE_ID"B"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// A pool for allocating reference counted messages.
  //***************************************************************************
  template <typename TCounter>
  class reference_counted_message_pool : public tphn::ireference_counted_message_pool
  {
  public:

    //*************************************************************************
    /// Constructor
    //*************************************************************************
    reference_counted_message_pool(tphn::imemory_block_allocator& memory_block_allocator_)
      : memory_block_allocator(memory_block_allocator_)
    {
    }

    //*************************************************************************
    /// Allocate a reference counted message from the pool.
    //*************************************************************************
    template <typename TMessage>
    tphn::reference_counted_message<TMessage, TCounter>* allocate(const TMessage& message)
    {
      TYPHOON_STATIC_ASSERT((tphn::is_base_of<tphn::imessage, TMessage>::value), "Not a message type");

      typedef tphn::reference_counted_message<TMessage, TCounter> rcm_t;
      typedef rcm_t* prcm_t;

      prcm_t p = TYPHOON_NULLPTR;

      lock();
      p = static_cast<prcm_t>(memory_block_allocator.allocate(sizeof(rcm_t), tphn::alignment_of<rcm_t>::value));
      unlock();

      if (p != TYPHOON_NULLPTR)
      {
        ::new(p) rcm_t(message, *this);
      }

      TYPHOON_ASSERT((p != TYPHOON_NULLPTR), TYPHOON_ERROR(tphn::reference_counted_message_pool_allocation_failure));

      return p;
    }

    //*************************************************************************
    /// Allocate a reference counted message from the pool.
    //*************************************************************************
    template <typename TMessage>
    tphn::reference_counted_message<TMessage, TCounter>* allocate()
    {
      TYPHOON_STATIC_ASSERT((tphn::is_base_of<tphn::imessage, TMessage>::value), "Not a message type");

      typedef tphn::reference_counted_message<TMessage, TCounter> rcm_t;
      typedef rcm_t* prcm_t;

      prcm_t p = TYPHOON_NULLPTR;

      lock();
      p = static_cast<prcm_t>(memory_block_allocator.allocate(sizeof(rcm_t), tphn::alignment_of<rcm_t>::value));
      unlock();

      if (p != TYPHOON_NULLPTR)
      {
        ::new(p) rcm_t(*this);
      }

      TYPHOON_ASSERT((p != TYPHOON_NULLPTR), TYPHOON_ERROR(tphn::reference_counted_message_pool_allocation_failure));

      return p;
    }

    //*************************************************************************
    /// Destruct a message and send it back to the allocator.
    //*************************************************************************
    void release(const tphn::ireference_counted_message& rcmessage)
    {
      bool released = false;

      lock();
      if (memory_block_allocator.is_owner_of(&rcmessage))
      {
        rcmessage.~ireference_counted_message();
        released = memory_block_allocator.release(&rcmessage);
      }
      unlock();

      TYPHOON_ASSERT(released, TYPHOON_ERROR(tphn::reference_counted_message_pool_release_failure));
    }

#if TYPHOON_USING_CPP11
    //*****************************************************
    template <typename TMessage1, typename... TMessages>
    struct pool_message_parameters
    {
    private:

      // Size of the first pool message type.
      static constexpr size_t size1 = sizeof(tphn::reference_counted_message<TMessage1, TCounter>);

      // Maximum size of the the rest of the pool message types.
      static constexpr size_t size2 = pool_message_parameters<TMessages...>::max_size;

      // Size of the first pool message type.
      static constexpr size_t alignment1 = tphn::alignment_of<tphn::reference_counted_message<TMessage1, TCounter>>::value;

      // Maximum size of the the rest of the pool message types.
      static constexpr size_t alignment2 = pool_message_parameters<TMessages...>::max_alignment;

    public:

      // The maximum size.
      static constexpr size_t max_size = (size1 < size2) ? size2 : size1;

      // The maximum alignment.
      static constexpr size_t max_alignment = (alignment1 < alignment2) ? alignment2 : alignment1;
    };

    //*****************************************************
    template <typename TMessage1>
    struct pool_message_parameters<TMessage1>
    {
    public:

      TYPHOON_STATIC_ASSERT((tphn::is_base_of<tphn::imessage, TMessage1>::value), "TMessage not derived from tphn::imessage");

      // The size of this pool message type.
      static constexpr size_t max_size = sizeof(tphn::reference_counted_message<TMessage1, TCounter>);

      // The maximum alignment.
      static constexpr size_t max_alignment = tphn::alignment_of<tphn::reference_counted_message<TMessage1, TCounter>>::value;
    };
#else
    template <typename TMessage1,              typename TMessage2  = TMessage1, typename TMessage3  = TMessage1, typename TMessage4  = TMessage1,
              typename TMessage5  = TMessage1, typename TMessage6  = TMessage1, typename TMessage7  = TMessage1, typename TMessage8  = TMessage1>
    struct pool_message_parameters
    {
      TYPHOON_STATIC_ASSERT((tphn::is_base_of<tphn::imessage, TMessage1>::value), "TMessage1 not derived from tphn::imessage");
      TYPHOON_STATIC_ASSERT((tphn::is_base_of<tphn::imessage, TMessage1>::value), "TMessage2 not derived from tphn::imessage");
      TYPHOON_STATIC_ASSERT((tphn::is_base_of<tphn::imessage, TMessage1>::value), "TMessage3 not derived from tphn::imessage");
      TYPHOON_STATIC_ASSERT((tphn::is_base_of<tphn::imessage, TMessage1>::value), "TMessage4 not derived from tphn::imessage");
      TYPHOON_STATIC_ASSERT((tphn::is_base_of<tphn::imessage, TMessage1>::value), "TMessage5 not derived from tphn::imessage");
      TYPHOON_STATIC_ASSERT((tphn::is_base_of<tphn::imessage, TMessage1>::value), "TMessage6 not derived from tphn::imessage");
      TYPHOON_STATIC_ASSERT((tphn::is_base_of<tphn::imessage, TMessage1>::value), "TMessage7 not derived from tphn::imessage");
      TYPHOON_STATIC_ASSERT((tphn::is_base_of<tphn::imessage, TMessage1>::value), "TMessage8 not derived from tphn::imessage");

      static TYPHOON_CONSTANT size_t max_size = tphn::largest<tphn::reference_counted_message<TMessage1, TCounter>,
                                                  tphn::reference_counted_message<TMessage2, TCounter>,
                                                  tphn::reference_counted_message<TMessage3, TCounter>,
                                                  tphn::reference_counted_message<TMessage4, TCounter>,
                                                  tphn::reference_counted_message<TMessage5, TCounter>,
                                                  tphn::reference_counted_message<TMessage6, TCounter>,
                                                  tphn::reference_counted_message<TMessage7, TCounter>,
                                                  tphn::reference_counted_message<TMessage8, TCounter> >::size;


      static TYPHOON_CONSTANT size_t max_alignment = tphn::largest<tphn::reference_counted_message<TMessage1, TCounter>,
                                                       tphn::reference_counted_message<TMessage2, TCounter>,
                                                       tphn::reference_counted_message<TMessage3, TCounter>,
                                                       tphn::reference_counted_message<TMessage4, TCounter>,
                                                       tphn::reference_counted_message<TMessage5, TCounter>,
                                                       tphn::reference_counted_message<TMessage6, TCounter>,
                                                       tphn::reference_counted_message<TMessage7, TCounter>,
                                                       tphn::reference_counted_message<TMessage8, TCounter> >::alignment;
    };
#endif

  private:

    /// The raw memory block pool.
    imemory_block_allocator& memory_block_allocator;

    // Should not be copied.
    reference_counted_message_pool(const reference_counted_message_pool&) TYPHOON_DELETE;
    reference_counted_message_pool& operator =(const reference_counted_message_pool&) TYPHOON_DELETE;
  };

#if TYPHOON_USING_CPP11 && TYPHOON_HAS_ATOMIC
  using  atomic_counted_message_pool = reference_counted_message_pool<tphn::atomic_int>;
#endif
}

#endif
