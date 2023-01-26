/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tpn
https://www.tpncpp.com

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

#ifndef TYPHOON_CALLBACK_TIMER_HPP
#define TYPHOON_CALLBACK_TIMER_HPP

#include "platform.hpp"
#include "algorithm.hpp"
#include "nullptr.hpp"
#include "function.hpp"
#include "static_assert.hpp"
#include "timer.hpp"
#include "atomic.hpp"
#include "error_handler.hpp"
#include "placement_new.hpp"
#include "delegate.hpp"

#include <stdint.h>

#if defined(TYPHOON_IN_UNIT_TEST) && TYPHOON_NOT_USING_STL
  #define TYPHOON_DISABLE_TIMER_UPDATES
  #define TYPHOON_ENABLE_TIMER_UPDATES
  #define TYPHOON_TIMER_UPDATES_ENABLED true

  #undef TYPHOON_CALLBACK_TIMER_USE_ATOMIC_LOCK
  #undef TYPHOON_CALLBACK_TIMER_USE_INTERRUPT_LOCK
#else
  #if !defined(TYPHOON_CALLBACK_TIMER_USE_ATOMIC_LOCK) && !defined(TYPHOON_CALLBACK_TIMER_USE_INTERRUPT_LOCK)
    #error TYPHOON_CALLBACK_TIMER_USE_ATOMIC_LOCK or TYPHOON_CALLBACK_TIMER_USE_INTERRUPT_LOCK not defined
  #endif

  #if defined(TYPHOON_CALLBACK_TIMER_USE_ATOMIC_LOCK) && defined(TYPHOON_CALLBACK_TIMER_USE_INTERRUPT_LOCK)
    #error Only define one of TYPHOON_CALLBACK_TIMER_USE_ATOMIC_LOCK or TYPHOON_CALLBACK_TIMER_USE_INTERRUPT_LOCK
  #endif

  #if defined(TYPHOON_CALLBACK_TIMER_USE_ATOMIC_LOCK)
    #define TYPHOON_DISABLE_TIMER_UPDATES (++process_semaphore)
    #define TYPHOON_ENABLE_TIMER_UPDATES  (--process_semaphore)
    #define TYPHOON_TIMER_UPDATES_ENABLED (process_semaphore.load() == 0)
  #endif
#endif

#if defined(TYPHOON_CALLBACK_TIMER_USE_INTERRUPT_LOCK)
  #if !defined(TYPHOON_CALLBACK_TIMER_DISABLE_INTERRUPTS) || !defined(TYPHOON_CALLBACK_TIMER_ENABLE_INTERRUPTS)
    #error TYPHOON_CALLBACK_TIMER_DISABLE_INTERRUPTS and/or TYPHOON_CALLBACK_TIMER_ENABLE_INTERRUPTS not defined
  #endif

  #define TYPHOON_DISABLE_TIMER_UPDATES TYPHOON_CALLBACK_TIMER_DISABLE_INTERRUPTS
  #define TYPHOON_ENABLE_TIMER_UPDATES  TYPHOON_CALLBACK_TIMER_ENABLE_INTERRUPTS
  #define TYPHOON_TIMER_UPDATES_ENABLED true
#endif

namespace tpn
{
  //*************************************************************************
  /// The configuration of a timer.
  struct callback_timer_data
  {
    typedef tpn::delegate<void(void)> callback_type;

    enum callback_type_id
    {
      C_CALLBACK,
      IFUNCTION,
      DELEGATE
    };

    //*******************************************
    callback_timer_data()
      : p_callback(TYPHOON_NULLPTR),
        period(0),
        delta(tpn::timer::state::INACTIVE),
        id(tpn::timer::id::NO_TIMER),
        previous(tpn::timer::id::NO_TIMER),
        next(tpn::timer::id::NO_TIMER),
        repeating(true),
        cbk_type(IFUNCTION)
    {
    }

    //*******************************************
    /// C function callback
    //*******************************************
    callback_timer_data(tpn::timer::id::type id_,
                        void                 (*p_callback_)(),
                        uint32_t             period_,
                        bool                 repeating_)
      : p_callback(reinterpret_cast<void*>(p_callback_)),
        period(period_),
        delta(tpn::timer::state::INACTIVE),
        id(id_),
        previous(tpn::timer::id::NO_TIMER),
        next(tpn::timer::id::NO_TIMER),
        repeating(repeating_),
        cbk_type(C_CALLBACK)
    {
    }

    //*******************************************
    /// TYPHOON function callback
    //*******************************************
    callback_timer_data(tpn::timer::id::type  id_,
                        tpn::ifunction<void>& callback_,
                        uint32_t              period_,
                        bool                  repeating_)
      : p_callback(reinterpret_cast<void*>(&callback_)),
        period(period_),
        delta(tpn::timer::state::INACTIVE),
        id(id_),
        previous(tpn::timer::id::NO_TIMER),
        next(tpn::timer::id::NO_TIMER),
        repeating(repeating_),
        cbk_type(IFUNCTION)
    {
    }

#if TYPHOON_USING_CPP11
    //*******************************************
    /// TYPHOON delegate callback
    //*******************************************
    callback_timer_data(tpn::timer::id::type id_,
                        callback_type&       callback_,
                        uint32_t             period_,
                        bool                 repeating_)
            : p_callback(reinterpret_cast<void*>(&callback_)),
              period(period_),
              delta(tpn::timer::state::INACTIVE),
              id(id_),
              previous(tpn::timer::id::NO_TIMER),
              next(tpn::timer::id::NO_TIMER),
              repeating(repeating_),
              cbk_type(DELEGATE)
    {
    }
#endif

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

    void*                 p_callback;
    uint32_t              period;
    uint32_t              delta;
    tpn::timer::id::type  id;
    uint_least8_t         previous;
    uint_least8_t         next;
    bool                  repeating;
    callback_type_id      cbk_type;

  private:

    // Disabled.
    callback_timer_data(const callback_timer_data& other);
    callback_timer_data& operator =(const callback_timer_data& other);
  };

  namespace private_callback_timer
  {
    //*************************************************************************
    /// A specialised intrusive linked list for timer data.
    //*************************************************************************
    class list
    {
    public:

      //*******************************
      list(tpn::callback_timer_data* ptimers_)
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
        tpn::callback_timer_data& timer = ptimers[id_];

        if (head == tpn::timer::id::NO_TIMER)
        {
          // No entries yet.
          head = id_;
          tail = id_;
          timer.previous = tpn::timer::id::NO_TIMER;
          timer.next     = tpn::timer::id::NO_TIMER;
        }
        else
        {
          // We already have entries.
          tpn::timer::id::type test_id = begin();

          while (test_id != tpn::timer::id::NO_TIMER)
          {
            tpn::callback_timer_data& test = ptimers[test_id];

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
            timer.previous     = tail;
            timer.next         = tpn::timer::id::NO_TIMER;
            tail               = timer.id;
          }
        }
      }

      //*******************************
      void remove(tpn::timer::id::type id_, bool has_expired)
      {
        tpn::callback_timer_data& timer = ptimers[id_];

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
        timer.next     = tpn::timer::id::NO_TIMER;
        timer.delta    = tpn::timer::state::INACTIVE;
      }

      //*******************************
      tpn::callback_timer_data& front()
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
          tpn::callback_timer_data& timer = ptimers[id];
          id = next(id);
          timer.next = tpn::timer::id::NO_TIMER;
        }

        head    = tpn::timer::id::NO_TIMER;
        tail    = tpn::timer::id::NO_TIMER;
        current = tpn::timer::id::NO_TIMER;
      }

    private:

      tpn::timer::id::type head;
      tpn::timer::id::type tail;
      tpn::timer::id::type current;

      tpn::callback_timer_data* const ptimers;
    };
  }

  //***************************************************************************
  /// Interface for callback timer
  //***************************************************************************
  class icallback_timer
  {
  public:

    typedef tpn::delegate<void(void)> callback_type;

    //*******************************************
    /// Register a timer.
    //*******************************************
    tpn::timer::id::type register_timer(void     (*p_callback_)(),
                                        uint32_t period_,
                                        bool     repeating_)
    {
      tpn::timer::id::type id = tpn::timer::id::NO_TIMER;

      bool is_space = (registered_timers < MAX_TIMERS);

      if (is_space)
      {
        // Search for the free space.
        for (uint_least8_t i = 0U; i < MAX_TIMERS; ++i)
        {
          tpn::callback_timer_data& timer = timer_array[i];

          if (timer.id == tpn::timer::id::NO_TIMER)
          {
            // Create in-place.
            new (&timer) callback_timer_data(i, p_callback_, period_, repeating_);
            ++registered_timers;
            id = i;
            break;
          }
        }
      }

      return id;
    }

    //*******************************************
    /// Register a timer.
    //*******************************************
    tpn::timer::id::type register_timer(tpn::ifunction<void>& callback_,
                                        uint32_t              period_,
                                        bool                  repeating_)
    {
      tpn::timer::id::type id = tpn::timer::id::NO_TIMER;

      bool is_space = (registered_timers < MAX_TIMERS);

      if (is_space)
      {
        // Search for the free space.
        for (uint_least8_t i = 0U; i < MAX_TIMERS; ++i)
        {
          tpn::callback_timer_data& timer = timer_array[i];

          if (timer.id == tpn::timer::id::NO_TIMER)
          {
            // Create in-place.
            new (&timer) callback_timer_data(i, callback_, period_, repeating_);
            ++registered_timers;
            id = i;
            break;
          }
        }
      }

      return id;
    }

      //*******************************************
      /// Register a timer.
      //*******************************************
#if TYPHOON_USING_CPP11
      tpn::timer::id::type register_timer(callback_type& callback_,
                                          uint32_t       period_,
                                          bool           repeating_)
      {
          tpn::timer::id::type id = tpn::timer::id::NO_TIMER;

          bool is_space = (registered_timers < MAX_TIMERS);

          if (is_space)
          {
              // Search for the free space.
              for (uint_least8_t i = 0U; i < MAX_TIMERS; ++i)
              {
                  tpn::callback_timer_data& timer = timer_array[i];

                  if (timer.id == tpn::timer::id::NO_TIMER)
                  {
                      // Create in-place.
                      new (&timer) callback_timer_data(i, callback_, period_, repeating_);
                      ++registered_timers;
                      id = i;
                      break;
                  }
              }
          }

          return id;
      }
#endif

    //*******************************************
    /// Unregister a timer.
    //*******************************************
    bool unregister_timer(tpn::timer::id::type id_)
    {
      bool result = false;

      if (id_ != tpn::timer::id::NO_TIMER)
      {
        tpn::callback_timer_data& timer = timer_array[id_];

        if (timer.id != tpn::timer::id::NO_TIMER)
        {
          if (timer.is_active())
          {
            TYPHOON_DISABLE_TIMER_UPDATES;
            active_list.remove(timer.id, false);
            TYPHOON_ENABLE_TIMER_UPDATES;
          }

          // Reset in-place.
          new (&timer) callback_timer_data();
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
        ::new (&timer_array[i]) callback_timer_data();
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
              tpn::callback_timer_data& timer = active_list.front();

              count -= timer.delta;

              active_list.remove(timer.id, true);

              if (timer.repeating)
              {
                // Reinsert the timer.
                timer.delta = timer.period;
                active_list.insert(timer.id);
              }

              if (timer.p_callback != TYPHOON_NULLPTR)
              {
                if (timer.cbk_type == callback_timer_data::C_CALLBACK)
                {
                  // Call the C callback.
                  reinterpret_cast<void(*)()>(timer.p_callback)();
                }
                else if(timer.cbk_type == callback_timer_data::IFUNCTION)
                {
                  // Call the function wrapper callback.
                  (*reinterpret_cast<tpn::ifunction<void>*>(timer.p_callback))();
                }
#if TYPHOON_USING_CPP11
                else if(timer.cbk_type == callback_timer_data::DELEGATE)
                {
                    // Call the delegate callback.
                    (*reinterpret_cast<callback_type*>(timer.p_callback))();
                }
#endif
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
        tpn::callback_timer_data& timer = timer_array[id_];

        // Registered timer?
        if (timer.id != tpn::timer::id::NO_TIMER)
        {
          // Has a valid period.
          if (timer.period != tpn::timer::state::INACTIVE)
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
    bool stop(tpn::timer::id::type id_)
    {
      bool result = false;

      // Valid timer id?
      if (id_ != tpn::timer::id::NO_TIMER)
      {
        tpn::callback_timer_data& timer = timer_array[id_];

        // Registered timer?
        if (timer.id != tpn::timer::id::NO_TIMER)
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

    //*******************************************
    /// Constructor.
    //*******************************************
    icallback_timer(callback_timer_data* const timer_array_, const uint_least8_t  MAX_TIMERS_)
      : timer_array(timer_array_),
        active_list(timer_array_),
        enabled(false),
#if defined(TYPHOON_CALLBACK_TIMER_USE_ATOMIC_LOCK)
        process_semaphore(0),
#endif
        registered_timers(0),
        MAX_TIMERS(MAX_TIMERS_)
    {
    }

  private:

    // The array of timer data structures.
    callback_timer_data* const timer_array;

    // The list of active timers.
    private_callback_timer::list active_list;

    volatile bool enabled;
#if defined(TYPHOON_CALLBACK_TIMER_USE_ATOMIC_LOCK)

#if defined(TYPHOON_TIMER_SEMAPHORE_TYPE)
    typedef TYPHOON_TIMER_SEMAPHORE_TYPE timer_semaphore_t;
#else
  #if TYPHOON_HAS_ATOMIC
    typedef tpn::atomic_uint16_t timer_semaphore_t;
  #else
    #error No atomic type available
  #endif
#endif

    volatile tpn::timer_semaphore_t process_semaphore;
#endif
    volatile uint_least8_t registered_timers;

  public:

    const uint_least8_t MAX_TIMERS;
  };

  //***************************************************************************
  /// The callback timer
  //***************************************************************************
  template <const uint_least8_t MAX_TIMERS_>
  class callback_timer : public tpn::icallback_timer
  {
  public:

    TYPHOON_STATIC_ASSERT(MAX_TIMERS_ <= 254, "No more than 254 timers are allowed");

    //*******************************************
    /// Constructor.
    //*******************************************
    callback_timer()
      : icallback_timer(timer_array, MAX_TIMERS_)
    {
    }

  private:

    callback_timer_data timer_array[MAX_TIMERS_];
  };
}

#undef TYPHOON_DISABLE_TIMER_UPDATES
#undef TYPHOON_ENABLE_TIMER_UPDATES
#undef TYPHOON_TIMER_UPDATES_ENABLED

#endif
