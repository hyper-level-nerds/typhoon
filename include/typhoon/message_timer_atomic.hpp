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

#ifndef TYPHOON_MESSAGE_TIMER_ATOMIC_HPP
#define TYPHOON_MESSAGE_TIMER_ATOMIC_HPP

#include "platform.hpp"
#include "nullptr.hpp"
#include "message_types.hpp"
#include "message.hpp"
#include "message_router.hpp"
#include "message_bus.hpp"
#include "static_assert.hpp"
#include "timer.hpp"
#include "atomic.hpp"
#include "algorithm.hpp"

#include <stdint.h>

#if TYPHOON_HAS_ATOMIC

namespace tpn
{
  //***************************************************************************
  /// Interface for message timer
  //***************************************************************************
  template <typename TSemaphore>
  class imessage_timer_atomic
  {
  public:

    //*******************************************
    /// Register a timer.
    //*******************************************
    tpn::timer::id::type register_timer(const tpn::imessage&     message_,
                                        tpn::imessage_router&    router_,
                                        uint32_t                 period_,
                                        bool                     repeating_,
                                        tpn::message_router_id_t destination_router_id_ = tpn::imessage_router::ALL_MESSAGE_ROUTERS)
    {
      tpn::timer::id::type id = tpn::timer::id::NO_TIMER;

      bool is_space = (registered_timers < MAX_TIMERS);

      if (is_space)
      {
        // There's no point adding null message routers.
        if (!router_.is_null_router())
        {
          // Search for the free space.
          for (uint_least8_t i = 0U; i < MAX_TIMERS; ++i)
          {
            timer_data& timer = timer_array[i];

            if (timer.id == tpn::timer::id::NO_TIMER)
            {
              // Create in-place.
              new (&timer) timer_data(i, message_, router_, period_, repeating_, destination_router_id_);
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
    bool unregister_timer(tpn::timer::id::type id_)
    {
      bool result = false;

      if (id_ != tpn::timer::id::NO_TIMER)
      {
        timer_data& timer = timer_array[id_];

        if (timer.id != tpn::timer::id::NO_TIMER)
        {
          if (timer.is_active())
          {
            ++process_semaphore;
            active_list.remove(timer.id, true);
            --process_semaphore;
          }

          // Reset in-place.
          new (&timer) timer_data();
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
      ++process_semaphore;
      active_list.clear();
      --process_semaphore;

      for (int i = 0; i < MAX_TIMERS; ++i)
      {
        new (&timer_array[i]) timer_data();
      }

      registered_timers = 0U;
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
        if (process_semaphore == 0U)
        {
          // We have something to do?
          bool has_active = !active_list.empty();

          if (has_active)
          {
            while (has_active && (count >= active_list.front().delta))
            {
              timer_data& timer = active_list.front();

              count -= timer.delta;

              active_list.remove(timer.id, true);

              if (timer.p_router != TYPHOON_NULLPTR)
              {
                timer.p_router->receive(timer.destination_router_id, *(timer.p_message));
              }

              if (timer.repeating)
              {
                timer.delta = timer.period;
                active_list.insert(timer.id);
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
    bool start(tpn::timer::id::type id_, bool immediate_ = false)
    {
      bool result = false;

      // Valid timer id?
      if (id_ != tpn::timer::id::NO_TIMER)
      {
        timer_data& timer = timer_array[id_];

        // Registered timer?
        if (timer.id != tpn::timer::id::NO_TIMER)
        {
          // Has a valid period.
          if (timer.period != tpn::timer::state::INACTIVE)
          {
            ++process_semaphore;
            if (timer.is_active())
            {
              active_list.remove(timer.id, false);
            }

            timer.delta = immediate_ ? 0U : timer.period;
            active_list.insert(timer.id);
            --process_semaphore;

            result = true;
          }
        }
      }

      return result;
    }

    //*******************************************
    /// Stops a timer.
    //*******************************************
    bool stop(tpn::timer::id::type id_)
    {
      bool result = false;

      // Valid timer id?
      if (id_ != tpn::timer::id::NO_TIMER)
      {
        timer_data& timer = timer_array[id_];

        // Registered timer?
        if (timer.id != tpn::timer::id::NO_TIMER)
        {
          if (timer.is_active())
          {
            ++process_semaphore;
            active_list.remove(timer.id, false);
            --process_semaphore;
          }

          result = true;
        }
      }

      return result;
    }

    //*******************************************
    /// Sets a timer's period.
    //*******************************************
    bool set_period(tpn::timer::id::type id_, uint32_t period_)
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
    bool set_mode(tpn::timer::id::type id_, bool repeating_)
    {
      if (stop(id_))
      {
        timer_array[id_].repeating = repeating_;
        return true;
      }

      return false;
    }

  protected:

    //*************************************************************************
    /// The configuration of a timer.
    struct timer_data
    {
      //*******************************************
      timer_data()
        : p_message(TYPHOON_NULLPTR)
        , p_router(TYPHOON_NULLPTR)
        , period(0U)
        , delta(tpn::timer::state::INACTIVE)
        , destination_router_id(tpn::imessage_bus::ALL_MESSAGE_ROUTERS)
        , id(tpn::timer::id::NO_TIMER)
        , previous(tpn::timer::id::NO_TIMER)
        , next(tpn::timer::id::NO_TIMER)
        , repeating(true)
      {
      }

      //*******************************************
      timer_data(tpn::timer::id::type     id_,
        const tpn::imessage& message_,
        tpn::imessage_router& irouter_,
        uint32_t                 period_,
        bool                     repeating_,
        tpn::message_router_id_t destination_router_id_ = tpn::imessage_bus::ALL_MESSAGE_ROUTERS)
        : p_message(&message_)
        , p_router(&irouter_)
        , period(period_)
        , delta(tpn::timer::state::INACTIVE)
        , destination_router_id(destination_router_id_)
        , id(id_)
        , previous(tpn::timer::id::NO_TIMER)
        , next(tpn::timer::id::NO_TIMER)
        , repeating(repeating_)
      {
      }

      //*******************************************
      /// Returns true if the timer is active.
      //*******************************************
      bool is_active() const
      {
        return delta != tpn::timer::state::INACTIVE;
      }

      //*******************************************
      /// Sets the timer to the inactive state.
      //*******************************************
      void set_inactive()
      {
        delta = tpn::timer::state::INACTIVE;
      }

      const tpn::imessage*     p_message;
      tpn::imessage_router*    p_router;
      uint32_t                 period;
      uint32_t                 delta;
      tpn::message_router_id_t destination_router_id;
      tpn::timer::id::type     id;
      uint_least8_t            previous;
      uint_least8_t            next;
      bool                     repeating;

    private:

      // Disabled.
      timer_data(const timer_data& other);
      timer_data& operator =(const timer_data& other);
    };

    //*******************************************
    /// Constructor.
    //*******************************************
    imessage_timer_atomic(timer_data* const timer_array_, const uint_least8_t  MAX_TIMERS_)
      : timer_array(timer_array_)
      , active_list(timer_array_)
      , enabled(false)
      , process_semaphore(0U)
      , registered_timers(0U)
      , MAX_TIMERS(MAX_TIMERS_)
    {
    }

    //*******************************************
    /// Destructor.
    //*******************************************
    ~imessage_timer_atomic()
    {
    }

  private:

    //*************************************************************************
    /// A specialised intrusive linked list for timer data.
    //*************************************************************************
    class timer_list
    {
    public:

      //*******************************
      timer_list(timer_data* ptimers_)
        : head(tpn::timer::id::NO_TIMER),
        tail(tpn::timer::id::NO_TIMER),
        current(tpn::timer::id::NO_TIMER),
        ptimers(ptimers_)
      {
      }

      //*******************************
      bool empty() const
      {
        return head == tpn::timer::id::NO_TIMER;
      }

      //*******************************
      // Inserts the timer at the correct delta position
      //*******************************
      void insert(tpn::timer::id::type id_)
      {
        timer_data& timer = ptimers[id_];

        if (head == tpn::timer::id::NO_TIMER)
        {
          // No entries yet.
          head = id_;
          tail = id_;
          timer.previous = tpn::timer::id::NO_TIMER;
          timer.next = tpn::timer::id::NO_TIMER;
        }
        else
        {
          // We already have entries.
          tpn::timer::id::type test_id = begin();

          while (test_id != tpn::timer::id::NO_TIMER)
          {
            timer_data& test = ptimers[test_id];

            // Find the correct place to insert.
            if (timer.delta <= test.delta)
            {
              if (test.id == head)
              {
                head = timer.id;
              }

              // Insert before test.
              timer.previous = test.previous;
              test.previous = timer.id;
              timer.next = test.id;

              // Adjust the next delta to compensate.
              test.delta -= timer.delta;

              if (timer.previous != tpn::timer::id::NO_TIMER)
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
          if (test_id == tpn::timer::id::NO_TIMER)
          {
            // Tag on to the tail.
            ptimers[tail].next = timer.id;
            timer.previous = tail;
            timer.next = tpn::timer::id::NO_TIMER;
            tail = timer.id;
          }
        }
      }

      //*******************************
      void remove(tpn::timer::id::type id_, bool has_expired)
      {
        timer_data& timer = ptimers[id_];

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
          if (timer.next != tpn::timer::id::NO_TIMER)
          {
            ptimers[timer.next].delta += timer.delta;
          }
        }

        timer.previous = tpn::timer::id::NO_TIMER;
        timer.next = tpn::timer::id::NO_TIMER;
        timer.delta = tpn::timer::state::INACTIVE;
      }

      //*******************************
      timer_data& front()
      {
        return ptimers[head];
      }

      //*******************************
      tpn::timer::id::type begin()
      {
        current = head;
        return current;
      }

      //*******************************
      tpn::timer::id::type previous(tpn::timer::id::type last)
      {
        current = ptimers[last].previous;
        return current;
      }

      //*******************************
      tpn::timer::id::type next(tpn::timer::id::type last)
      {
        current = ptimers[last].next;
        return current;
      }

      //*******************************
      void clear()
      {
        tpn::timer::id::type id = begin();

        while (id != tpn::timer::id::NO_TIMER)
        {
          timer_data& timer = ptimers[id];
          id = next(id);
          timer.next = tpn::timer::id::NO_TIMER;
        }

        head = tpn::timer::id::NO_TIMER;
        tail = tpn::timer::id::NO_TIMER;
        current = tpn::timer::id::NO_TIMER;
      }

    private:

      tpn::timer::id::type head;
      tpn::timer::id::type tail;
      tpn::timer::id::type current;

      timer_data* const ptimers;
    };

    // The array of timer data structures.
    timer_data* const timer_array;

    // The list of active timers.
    timer_list active_list;

    volatile bool enabled;
    volatile TSemaphore process_semaphore;
    volatile uint_least8_t registered_timers;

  public:

    const uint_least8_t MAX_TIMERS;
  };

  //***************************************************************************
  /// The message timer
  //***************************************************************************
  template <uint_least8_t MAX_TIMERS_, typename TSemaphore>
  class message_timer_atomic : public tpn::imessage_timer_atomic<TSemaphore>
  {
  public:

    TYPHOON_STATIC_ASSERT(MAX_TIMERS_ <= 254, "No more than 254 timers are allowed");

    //*******************************************
    /// Constructor.
    //*******************************************
    message_timer_atomic()
      : imessage_timer_atomic<TSemaphore>(timer_array, MAX_TIMERS_)
    {
    }

  private:

    typename tpn::imessage_timer_atomic<TSemaphore>::timer_data timer_array[MAX_TIMERS_];
  };
}

#endif
#endif
