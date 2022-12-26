/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tphn
https://www.tphncpp.com

Copyright(c) 2017 John Wellbelove

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

#ifndef TYPHOON_MESSAGE_TIMER_INCLUDED
#define TYPHOON_MESSAGE_TIMER_INCLUDED

#include "platform.h"
#include "nullptr.h"
#include "message_types.h"
#include "message.h"
#include "message_router.h"
#include "message_bus.h"
#include "static_assert.h"
#include "timer.h"
#include "atomic.h"
#include "algorithm.h"

#include <stdint.h>

#if defined(TYPHOON_IN_UNIT_TEST) && TYPHOON_NOT_USING_STL
  #define TYPHOON_DISABLE_TIMER_UPDATES
  #define TYPHOON_ENABLE_TIMER_UPDATES
  #define TYPHOON_TIMER_UPDATES_ENABLED true

  #undef TYPHOON_MESSAGE_TIMER_USE_ATOMIC_LOCK
  #undef TYPHOON_MESSAGE_TIMER_USE_INTERRUPT_LOCK
#else
  #if !defined(TYPHOON_MESSAGE_TIMER_USE_ATOMIC_LOCK) && !defined(TYPHOON_MESSAGE_TIMER_USE_INTERRUPT_LOCK)
    #error TYPHOON_MESSAGE_TIMER_USE_ATOMIC_LOCK or TYPHOON_MESSAGE_TIMER_USE_INTERRUPT_LOCK not defined
  #endif

  #if defined(TYPHOON_MESSAGE_TIMER_USE_ATOMIC_LOCK) && defined(TYPHOON_MESSAGE_TIMER_USE_INTERRUPT_LOCK)
    #error Only define one of TYPHOON_MESSAGE_TIMER_USE_ATOMIC_LOCK or TYPHOON_MESSAGE_TIMER_USE_INTERRUPT_LOCK
  #endif

  #if defined(TYPHOON_MESSAGE_TIMER_USE_ATOMIC_LOCK)
    #define TYPHOON_DISABLE_TIMER_UPDATES (++process_semaphore)
    #define TYPHOON_ENABLE_TIMER_UPDATES  (--process_semaphore)
    #define TYPHOON_TIMER_UPDATES_ENABLED (process_semaphore.load() == 0)
  #endif

  #if defined(TYPHOON_MESSAGE_TIMER_USE_INTERRUPT_LOCK)
    #if !defined(TYPHOON_MESSAGE_TIMER_DISABLE_INTERRUPTS) || !defined(TYPHOON_MESSAGE_TIMER_ENABLE_INTERRUPTS)
      #error TYPHOON_MESSAGE_TIMER_DISABLE_INTERRUPTS and/or TYPHOON_MESSAGE_TIMER_ENABLE_INTERRUPTS not defined
    #endif

    #define TYPHOON_DISABLE_TIMER_UPDATES TYPHOON_MESSAGE_TIMER_DISABLE_INTERRUPTS
    #define TYPHOON_ENABLE_TIMER_UPDATES  TYPHOON_MESSAGE_TIMER_ENABLE_INTERRUPTS
    #define TYPHOON_TIMER_UPDATES_ENABLED true
  #endif
#endif

namespace tphn
{
  //*************************************************************************
  /// The configuration of a timer.
  struct message_timer_data
  {
    //*******************************************
    message_timer_data()
      : p_message(TYPHOON_NULLPTR),
        p_router(TYPHOON_NULLPTR),
        period(0),
        delta(tphn::timer::state::INACTIVE),
        destination_router_id(tphn::imessage_bus::ALL_MESSAGE_ROUTERS),
        id(tphn::timer::id::NO_TIMER),
        previous(tphn::timer::id::NO_TIMER),
        next(tphn::timer::id::NO_TIMER),
        repeating(true)
    {
    }

    //*******************************************
    message_timer_data(tphn::timer::id::type     id_,
                       const tphn::imessage&     message_,
                       tphn::imessage_router&    irouter_,
                       uint32_t                 period_,
                       bool                     repeating_,
                       tphn::message_router_id_t destination_router_id_ = tphn::imessage_bus::ALL_MESSAGE_ROUTERS)
      : p_message(&message_),
        p_router(&irouter_),
        period(period_),
        delta(tphn::timer::state::INACTIVE),
        destination_router_id(destination_router_id_),
        id(id_),
        previous(tphn::timer::id::NO_TIMER),
        next(tphn::timer::id::NO_TIMER),
        repeating(repeating_)
    {
    }

    //*******************************************
    /// Returns true if the timer is active.
    //*******************************************
    bool is_active() const
    {
      return delta != tphn::timer::state::INACTIVE;
    }

    //*******************************************
    /// Sets the timer to the inactive state.
    //*******************************************
    void set_inactive()
    {
      delta = tphn::timer::state::INACTIVE;
    }

    const tphn::imessage*     p_message;
    tphn::imessage_router*    p_router;
    uint32_t                 period;
    uint32_t                 delta;
    tphn::message_router_id_t destination_router_id;
    tphn::timer::id::type     id;
    uint_least8_t            previous;
    uint_least8_t            next;
    bool                     repeating;

  private:

    // Disabled.
    message_timer_data(const message_timer_data& other);
    message_timer_data& operator =(const message_timer_data& other);
  };

  namespace private_message_timer
  {
    //*************************************************************************
    /// A specialised intrusive linked list for timer data.
    //*************************************************************************
    class list
    {
    public:

      //*******************************
      list(tphn::message_timer_data* ptimers_)
        : head(tphn::timer::id::NO_TIMER),
          tail(tphn::timer::id::NO_TIMER),
          current(tphn::timer::id::NO_TIMER),
          ptimers(ptimers_)
      {
      }

      //*******************************
      bool empty() const
      {
        return head == tphn::timer::id::NO_TIMER;
      }

      //*******************************
      // Inserts the timer at the correct delta position
      //*******************************
      void insert(tphn::timer::id::type id_)
      {
        tphn::message_timer_data& timer = ptimers[id_];

        if (head == tphn::timer::id::NO_TIMER)
        {
          // No entries yet.
          head = id_;
          tail = id_;
          timer.previous = tphn::timer::id::NO_TIMER;
          timer.next     = tphn::timer::id::NO_TIMER;
        }
        else
        {
          // We already have entries.
          tphn::timer::id::type test_id = begin();

          while (test_id != tphn::timer::id::NO_TIMER)
          {
            tphn::message_timer_data& test = ptimers[test_id];

            // Find the correct place to insert.
            if (timer.delta <= test.delta)
            {
              if (test.id == head)
              {
                head = timer.id;
              }

              // Insert before test.
              timer.previous = test.previous;
              test.previous  = timer.id;
              timer.next     = test.id;

              // Adjust the next delta to compensate.
              test.delta -= timer.delta;

              if (timer.previous != tphn::timer::id::NO_TIMER)
              {
                ptimers[timer.previous].next = timer.id;
              }
              break;
            }
            else
            {
              timer.delta -= test.delta;
            }

            test_id = next(test_id);
          }

          // Reached the end?
          if (test_id == tphn::timer::id::NO_TIMER)
          {
            // Tag on to the tail.
            ptimers[tail].next = timer.id;
            timer.previous     = tail;
            timer.next         = tphn::timer::id::NO_TIMER;
            tail               = timer.id;
          }
        }
      }

      //*******************************
      void remove(tphn::timer::id::type id_, bool has_expired)
      {
        tphn::message_timer_data& timer = ptimers[id_];

        if (head == id_)
        {
          head = timer.next;
        }
        else
        {
          ptimers[timer.previous].next = timer.next;
        }

        if (tail == id_)
        {
          tail = timer.previous;
        }
        else
        {
          ptimers[timer.next].previous = timer.previous;
        }

        if (!has_expired)
        {
          // Adjust the next delta.
          if (timer.next != tphn::timer::id::NO_TIMER)
          {
            ptimers[timer.next].delta += timer.delta;
          }
        }

        timer.previous = tphn::timer::id::NO_TIMER;
        timer.next     = tphn::timer::id::NO_TIMER;
        timer.delta    = tphn::timer::state::INACTIVE;
      }

      //*******************************
      tphn::message_timer_data& front()
      {
        return ptimers[head];
      }

      //*******************************
      tphn::timer::id::type begin()
      {
        current = head;
        return current;
      }

      //*******************************
      tphn::timer::id::type previous(tphn::timer::id::type last)
      {
        current = ptimers[last].previous;
        return current;
      }

      //*******************************
      tphn::timer::id::type next(tphn::timer::id::type last)
      {
        current = ptimers[last].next;
        return current;
      }

      //*******************************
      void clear()
      {
        tphn::timer::id::type id = begin();

        while (id != tphn::timer::id::NO_TIMER)
        {
          tphn::message_timer_data& timer = ptimers[id];
          id = next(id);
          timer.next = tphn::timer::id::NO_TIMER;
        }

        head    = tphn::timer::id::NO_TIMER;
        tail    = tphn::timer::id::NO_TIMER;
        current = tphn::timer::id::NO_TIMER;
      }

    private:

      tphn::timer::id::type head;
      tphn::timer::id::type tail;
      tphn::timer::id::type current;

      tphn::message_timer_data* const ptimers;
    };
  }

  //***************************************************************************
  /// Interface for message timer
  //***************************************************************************
  class imessage_timer
  {
  public:

    //*******************************************
    /// Register a timer.
    //*******************************************
    tphn::timer::id::type register_timer(const tphn::imessage&     message_,
                                        tphn::imessage_router&    router_,
                                        uint32_t                 period_,
                                        bool                     repeating_,
                                        tphn::message_router_id_t destination_router_id_ = tphn::imessage_router::ALL_MESSAGE_ROUTERS)
    {
      tphn::timer::id::type id = tphn::timer::id::NO_TIMER;

      bool is_space = (registered_timers < MAX_TIMERS);

      if (is_space)
      {
        // There's no point adding null message routers.
        if (!router_.is_null_router())
        {
          // Search for the free space.
          for (uint_least8_t i = 0U; i < MAX_TIMERS; ++i)
          {
            tphn::message_timer_data& timer = timer_array[i];

            if (timer.id == tphn::timer::id::NO_TIMER)
            {
              // Create in-place.
              new (&timer) message_timer_data(i, message_, router_, period_, repeating_, destination_router_id_);
              ++registered_timers;
              id = i;
              break;
            }
          }
        }
      }

      return id;
    }

    //*******************************************
    /// Unregister a timer.
    //*******************************************
    bool unregister_timer(tphn::timer::id::type id_)
    {
      bool result = false;

      if (id_ != tphn::timer::id::NO_TIMER)
      {
        tphn::message_timer_data& timer = timer_array[id_];

        if (timer.id != tphn::timer::id::NO_TIMER)
        {
          if (timer.is_active())
          {
            TYPHOON_DISABLE_TIMER_UPDATES;
            active_list.remove(timer.id, true);
            TYPHOON_ENABLE_TIMER_UPDATES;
          }

          // Reset in-place.
          new (&timer) message_timer_data();
          --registered_timers;

          result = true;
        }
      }

      return result;
    }

    //*******************************************
    /// Enable/disable the timer.
    //*******************************************
    void enable(bool state_)
    {
      enabled = state_;
    }

    //*******************************************
    /// Get the enable/disable state.
    //*******************************************
    bool is_running() const
    {
      return enabled;
    }

    //*******************************************
    /// Clears the timer of data.
    //*******************************************
    void clear()
    {
      TYPHOON_DISABLE_TIMER_UPDATES;
      active_list.clear();
      TYPHOON_ENABLE_TIMER_UPDATES;

      for (int i = 0; i < MAX_TIMERS; ++i)
      {
        new (&timer_array[i]) message_timer_data();
      }

      registered_timers = 0;
    }

    //*******************************************
    // Called by the timer service to indicate the
    // amount of time that has elapsed since the last successful call to 'tick'.
    // Returns true if the tick was processed,
    // false if not.
    //*******************************************
    bool tick(uint32_t count)
    {
      if (enabled)
      {
        if (TYPHOON_TIMER_UPDATES_ENABLED)
        {
          // We have something to do?
          bool has_active = !active_list.empty();

          if (has_active)
          {
            while (has_active && (count >= active_list.front().delta))
            {
              tphn::message_timer_data& timer = active_list.front();

              count -= timer.delta;

              active_list.remove(timer.id, true);

              if (timer.repeating)
              {
                timer.delta = timer.period;
                active_list.insert(timer.id);
              }

              if (timer.p_router != TYPHOON_NULLPTR)
              {
                timer.p_router->receive(timer.destination_router_id, *(timer.p_message));
              }

              has_active = !active_list.empty();
            }

            if (has_active)
            {
              // Subtract any remainder from the next due timeout.
              active_list.front().delta -= count;
            }
          }

          return true;
        }
      }

      return false;
    }

    //*******************************************
    /// Starts a timer.
    //*******************************************
    bool start(tphn::timer::id::type id_, bool immediate_ = false)
    {
      bool result = false;

      // Valid timer id?
      if (id_ != tphn::timer::id::NO_TIMER)
      {
        tphn::message_timer_data& timer = timer_array[id_];

        // Registered timer?
        if (timer.id != tphn::timer::id::NO_TIMER)
        {
          // Has a valid period.
          if (timer.period != tphn::timer::state::INACTIVE)
          {
            TYPHOON_DISABLE_TIMER_UPDATES;
            if (timer.is_active())
            {
              active_list.remove(timer.id, false);
            }

            timer.delta = immediate_ ? 0 : timer.period;
            active_list.insert(timer.id);
            TYPHOON_ENABLE_TIMER_UPDATES;

            result = true;
          }
        }
      }

      return result;
    }

    //*******************************************
    /// Stops a timer.
    //*******************************************
    bool stop(tphn::timer::id::type id_)
    {
      bool result = false;

      // Valid timer id?
      if (id_ != tphn::timer::id::NO_TIMER)
      {
        tphn::message_timer_data& timer = timer_array[id_];

        // Registered timer?
        if (timer.id != tphn::timer::id::NO_TIMER)
        {
          if (timer.is_active())
          {
            TYPHOON_DISABLE_TIMER_UPDATES;
            active_list.remove(timer.id, false);
            TYPHOON_ENABLE_TIMER_UPDATES;
          }

          result = true;
        }
      }

      return result;
    }

    //*******************************************
    /// Sets a timer's period.
    //*******************************************
    bool set_period(tphn::timer::id::type id_, uint32_t period_)
    {
      if (stop(id_))
      {
        timer_array[id_].period = period_;
        return true;
      }

      return false;
    }

    //*******************************************
    /// Sets a timer's mode.
    //*******************************************
    bool set_mode(tphn::timer::id::type id_, bool repeating_)
    {
      if (stop(id_))
      {
        timer_array[id_].repeating = repeating_;
        return true;
      }

      return false;
    }

  protected:

    //*******************************************
    /// Constructor.
    //*******************************************
    imessage_timer(message_timer_data* const timer_array_, const uint_least8_t  MAX_TIMERS_)
      : timer_array(timer_array_),
        active_list(timer_array_),
        enabled(false),
#if defined(TYPHOON_MESSAGE_TIMER_USE_ATOMIC_LOCK)
        process_semaphore(0),
#endif
        registered_timers(0),
        MAX_TIMERS(MAX_TIMERS_)
    {
    }

    //*******************************************
    /// Destructor.
    //*******************************************
    ~imessage_timer()
    {
    }

  private:

    // The array of timer data structures.
    message_timer_data* const timer_array;

    // The list of active timers.
    private_message_timer::list active_list;

    volatile bool enabled;

#if defined(TYPHOON_MESSAGE_TIMER_USE_ATOMIC_LOCK)
  
#if defined(TYPHOON_TIMER_SEMAPHORE_TYPE)
  typedef TYPHOON_TIMER_SEMAPHORE_TYPE timer_semaphore_t;
#else
  #if TYPHOON_HAS_ATOMIC
    typedef tphn::atomic_uint16_t timer_semaphore_t;
  #else
    #error No atomic type available
  #endif
#endif

    volatile tphn::timer_semaphore_t process_semaphore;
#endif
    volatile uint_least8_t registered_timers;

  public:

    const uint_least8_t MAX_TIMERS;
  };

  //***************************************************************************
  /// The message timer
  //***************************************************************************
  template <uint_least8_t MAX_TIMERS_>
  class message_timer : public tphn::imessage_timer
  {
  public:

    TYPHOON_STATIC_ASSERT(MAX_TIMERS_ <= 254, "No more than 254 timers are allowed");

    //*******************************************
    /// Constructor.
    //*******************************************
    message_timer()
      : imessage_timer(timer_array, MAX_TIMERS_)
    {
    }

  private:

    message_timer_data timer_array[MAX_TIMERS_];
  };
}

#undef TYPHOON_DISABLE_TIMER_UPDATES
#undef TYPHOON_ENABLE_TIMER_UPDATES
#undef TYPHOON_TIMER_UPDATES_ENABLED

#endif