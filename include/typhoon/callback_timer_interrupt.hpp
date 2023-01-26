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

#ifndef TYPHOON_CALLBACK_TIMER_INTERRUPT_HPP
#define TYPHOON_CALLBACK_TIMER_INTERRUPT_HPP

#include "platform.hpp"
#include "algorithm.hpp"
#include "nullptr.hpp"
#include "delegate.hpp"
#include "static_assert.hpp"
#include "timer.hpp"
#include "error_handler.hpp"
#include "placement_new.hpp"

#include <stdint.h>

namespace tpn
{
  //***************************************************************************
  /// Interface for callback timer
  //***************************************************************************
  template <typename TInterruptGuard>
  class icallback_timer_interrupt
  {
  public:

    typedef tpn::delegate<void(void)> callback_type;

    //*******************************************
    /// Register a timer.
    //*******************************************
    tpn::timer::id::type register_timer(const callback_type& callback_,
                                        uint32_t             period_,
                                        bool                 repeating_)
    {
      tpn::timer::id::type id = tpn::timer::id::NO_TIMER;

      bool is_space = (number_of_registered_timers < MAX_TIMERS);

      if (is_space)
      {
        // Search for the free space.
        for (uint_least8_t i = 0U; i < MAX_TIMERS; ++i)
        {
          timer_data& timer = timer_array[i];

          if (timer.id == tpn::timer::id::NO_TIMER)
          {
            TInterruptGuard guard;
            (void)guard; // Silence 'unused variable warnings.

            // Create in-place.
            new (&timer) timer_data(i, callback_, period_, repeating_);
            ++number_of_registered_timers;
            id = i;
            break;
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
            TInterruptGuard guard;
            (void)guard; // Silence 'unused variable warnings.

            active_list.remove(timer.id, false);
          }

          // Reset in-place.
          new (&timer) timer_data();
          --number_of_registered_timers;

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
      {
        TInterruptGuard guard;
        (void)guard; // Silence 'unused variable warnings.

        active_list.clear();
        number_of_registered_timers = 0;
      }

      for (uint8_t i = 0U; i < MAX_TIMERS; ++i)
      {
        ::new (&timer_array[i]) timer_data();
      }
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
        // We have something to do?
        bool has_active = !active_list.empty();       

        if (has_active)
        {
          while (has_active && (count >= active_list.front().delta))
          {
            timer_data& timer = active_list.front();

            count -= timer.delta;

            active_list.remove(timer.id, true);

            if (timer.callback.is_valid())
            {
              timer.callback();
            }

            if (timer.repeating)
            {
              // Reinsert the timer.
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
            TInterruptGuard guard;
            (void)guard; // Silence 'unused variable warnings.

            if (timer.is_active())
            {
              active_list.remove(timer.id, false);
            }

            timer.delta = immediate_ ? 0U : timer.period;
            active_list.insert(timer.id);

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
            TInterruptGuard guard;
            (void)guard; // Silence 'unused variable warnings.

            active_list.remove(timer.id, false);
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
        : callback()
        , period(0U)
        , delta(tpn::timer::state::INACTIVE)
        , id(tpn::timer::id::NO_TIMER)
        , previous(tpn::timer::id::NO_TIMER)
        , next(tpn::timer::id::NO_TIMER)
        , repeating(true)
      {
      }

      //*******************************************
      /// TYPHOON delegate callback
      //*******************************************
      timer_data(tpn::timer::id::type id_,
                 callback_type        callback_,
                 uint32_t             period_,
                 bool                 repeating_)
        : callback(callback_)
        , period(period_)
        , delta(tpn::timer::state::INACTIVE)
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

      callback_type        callback;
      uint32_t             period;
      uint32_t             delta;
      tpn::timer::id::type id;
      uint_least8_t        previous;
      uint_least8_t        next;
      bool                 repeating;

    private:

      // Disabled.
      timer_data(const timer_data& other) TYPHOON_DELETE;
      timer_data& operator =(const timer_data& other) TYPHOON_DELETE;
    };

    //*******************************************
    /// Constructor.
    //*******************************************
    icallback_timer_interrupt(timer_data* const timer_array_, const uint_least8_t  MAX_TIMERS_)
      : timer_array(timer_array_),
        active_list(timer_array_),
        enabled(false),
        number_of_registered_timers(0U),
        MAX_TIMERS(MAX_TIMERS_)
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
        : head(tpn::timer::id::NO_TIMER)
        , tail(tpn::timer::id::NO_TIMER)
        , current(tpn::timer::id::NO_TIMER)
        , ptimers(ptimers_)
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
    volatile uint_least8_t number_of_registered_timers;

  public:

    const uint_least8_t MAX_TIMERS;
  };

  //***************************************************************************
  /// The callback timer
  //***************************************************************************
  template <uint_least8_t MAX_TIMERS_, typename TInterruptGuard>
  class callback_timer_interrupt : public tpn::icallback_timer_interrupt<TInterruptGuard>
  {
  public:

    TYPHOON_STATIC_ASSERT(MAX_TIMERS_ <= 254U, "No more than 254 timers are allowed");

    typedef typename icallback_timer_interrupt<TInterruptGuard>::callback_type callback_type;

    //*******************************************
    /// Constructor.
    //*******************************************
    callback_timer_interrupt()
      : icallback_timer_interrupt<TInterruptGuard>(timer_array, MAX_TIMERS_)
    {
    }

  private:

    typename icallback_timer_interrupt<TInterruptGuard>::timer_data timer_array[MAX_TIMERS_];
  };
}

#endif
