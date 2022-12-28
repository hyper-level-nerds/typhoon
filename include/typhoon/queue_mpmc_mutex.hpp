///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tphn
https://www.tphncpp.com

Copyright(c) 2018 John Wellbelove

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

#ifndef TYPHOON_MPMC_QUEUE_MUTEX_HPP
#define TYPHOON_MPMC_QUEUE_MUTEX_HPP

#include "platform.hpp"
#include "mutex.hpp"

#if TYPHOON_HAS_MUTEX

#include "alignment.hpp"
#include "parameter_type.hpp"
#include "memory_model.hpp"
#include "integral_limits.hpp"
#include "utility.hpp"
#include "placement_new.hpp"

#include <stddef.h>
#include <stdint.h>

namespace tphn
{
  template <const size_t MEMORY_MODEL = tphn::memory_model::MEMORY_MODEL_LARGE>
  class queue_mpmc_mutex_base
  {
  public:

    /// The type used for determining the size of queue.
    typedef typename tphn::size_type_lookup<MEMORY_MODEL>::type size_type;

    //*************************************************************************
    /// How many items can the queue hold.
    //*************************************************************************
    size_type capacity() const
    {
      return MAX_SIZE;
    }

    //*************************************************************************
    /// How many items can the queue hold.
    //*************************************************************************
    size_type max_size() const
    {
      return MAX_SIZE;
    }

  protected:

    queue_mpmc_mutex_base(size_type max_size_)
      : write_index(0),
        read_index(0),
        current_size(0),
        MAX_SIZE(max_size_)
    {
    }

    //*************************************************************************
    /// Calculate the next index.
    //*************************************************************************
    static size_type get_next_index(size_type index, size_type maximum)
    {
      ++index;

      if (index == maximum) TYPHOON_UNLIKELY
      {
        index = 0;
      }

      return index;
    }

    size_type write_index;    ///< Where to input new data.
    size_type read_index;     ///< Where to get the oldest data.
    size_type current_size;   ///< The current size of the queue.
    const size_type MAX_SIZE; ///< The maximum number of items in the queue.

    //*************************************************************************
    /// Destructor.
    //*************************************************************************
#if defined(TYPHOON_POLYMORPHIC_MPMC_QUEUE_MUTEX) || defined(TYPHOON_POLYMORPHIC_CONTAINERS)
  public:
    virtual ~queue_mpmc_mutex_base()
    {
    }
#else
  protected:
    ~queue_mpmc_mutex_base()
    {
    }
#endif
  };

  //***************************************************************************
  ///\ingroup queue_mpmc
  ///\brief This is the base for all queue_mpmc_mutex's that contain a particular type.
  ///\details Normally a reference to this type will be taken from a derived queue_mpmc_mutex.
  ///\code
  /// tphn::queue_mpmc_mutex<int, 10> myQueue;
  /// tphn::iqueue_mpmc_mutex<int>& iQueue = myQueue;
  ///\endcode
  /// This queue supports concurrent access by one producer and one consumer.
  /// \tparam T The type of value that the queue_mpmc_mutex holds.
  //***************************************************************************
  template <typename T, const size_t MEMORY_MODEL = tphn::memory_model::MEMORY_MODEL_LARGE>
  class iqueue_mpmc_mutex : public queue_mpmc_mutex_base<MEMORY_MODEL>
  {
  private:

    typedef tphn::queue_mpmc_mutex_base<MEMORY_MODEL> base_t;

  public:

    typedef T                          value_type;      ///< The type stored in the queue.
    typedef T&                         reference;       ///< A reference to the type used in the queue.
    typedef const T&                   const_reference; ///< A const reference to the type used in the queue.
#if TYPHOON_USING_CPP11
    typedef T&&                        rvalue_reference;///< An rvalue reference to the type used in the queue.
#endif
    typedef typename base_t::size_type size_type;       ///< The type used for determining the size of the queue.

    using base_t::write_index;
    using base_t::read_index;
    using base_t::current_size;
    using base_t::MAX_SIZE;
    using base_t::get_next_index;

    //*************************************************************************
    /// Push a value to the queue.
    //*************************************************************************
    bool push(const_reference value)
    {
      access.lock();

      bool result = push_implementation(value);

      access.unlock();

      return result;
    }

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Push a value to the queue.
    //*************************************************************************
    bool push(rvalue_reference value)
    {
      access.lock();

      bool result = push_implementation(tphn::move(value));

      access.unlock();

      return result;
    }
#endif

#if TYPHOON_USING_CPP11 && TYPHOON_NOT_USING_STLPORT && !defined(TYPHOON_QUEUE_MPMC_MUTEX_FORCE_CPP03_IMPLEMENTATION)
    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tphn::queue_full if the queue if already full.
    //*************************************************************************
    template <typename ... Args>
    bool emplace(Args&&... args)
    {
      access.lock();

      bool result = emplace_implementation(tphn::forward<Args>(args)...);

      access.unlock();

      return result;
    }
#else
    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tphn::queue_full if the queue if already full.
    //*************************************************************************
    template <typename T1>
    bool emplace(const T1& value1)
    {
      access.lock();

      bool result = emplace_implementation(value1);

      access.unlock();

      return result;
    }

    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tphn::queue_full if the queue if already full.
    //*************************************************************************
    template <typename T1, typename T2>
    bool emplace(const T1& value1, const T2& value2)
    {
      access.lock();

      bool result = emplace_implementation(value1, value2);

      access.unlock();

      return result;
    }

    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tphn::queue_full if the queue if already full.
    //*************************************************************************
    template <typename T1, typename T2, typename T3>
    bool emplace(const T1& value1, const T2& value2, const T3& value3)
    {
      access.lock();

      bool result = emplace_implementation(value1, value2, value3);

      access.unlock();

      return result;
    }

    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tphn::queue_full if the queue if already full.
    //*************************************************************************
    template <typename T1, typename T2, typename T3, typename T4>
    bool emplace(const T1& value1, const T2& value2, const T3& value3, const T4& value4)
    {
      access.lock();

      bool result = emplace_implementation(value1, value2, value3, value4);

      access.unlock();

      return result;
    }
#endif

    //*************************************************************************
    /// Pop a value from the queue.
    //*************************************************************************
    bool pop(reference value)
    {
      access.lock();

      bool result = pop_implementation(value);

      access.unlock();

      return result;
    }

    //*************************************************************************
    /// Pop a value from the queue and discard.
    //*************************************************************************
    bool pop()
    {
      access.lock();

      bool result = pop_implementation();

      access.unlock();

      return result;
    }

    //*************************************************************************
    /// Peek a value at the front of the queue.
    //*************************************************************************
    reference front()
    {
      access.lock();

      reference result = front_implementation();

      access.unlock();

      return result;
    }

    //*************************************************************************
    /// Peek a value at the front of the queue.
    //*************************************************************************
    const_reference front() const
    {
      access.lock();

      const_reference result = front_implementation();

      access.unlock();

      return result;
    }

    //*************************************************************************
    /// Clear the queue.
    //*************************************************************************
    void clear()
    {
      access.lock();

      while (pop_implementation())
      {
        // Do nothing.
      }

      access.unlock();
    }

    //*************************************************************************
    /// Is the queue empty?
    //*************************************************************************
    bool empty() const
    {
      access.lock();

      size_type result = (current_size == 0);

      access.unlock();

      return result;
    }

    //*************************************************************************
    /// Is the queue full?
    //*************************************************************************
    bool full() const
    {
      access.lock();

      size_type result = (current_size == MAX_SIZE);

      access.unlock();

      return result;
    }

    //*************************************************************************
    /// How many items in the queue?
    //*************************************************************************
    size_type size() const
    {
      access.lock();

      size_type result = current_size;

      access.unlock();

      return result;
    }

    //*************************************************************************
    /// How much free space available in the queue.
    //*************************************************************************
    size_type available() const
    {
      access.lock();

      size_type result = MAX_SIZE - current_size;

      access.unlock();

      return result;
    }

  protected:

    //*************************************************************************
    /// The constructor that is called from derived classes.
    //*************************************************************************
    iqueue_mpmc_mutex(T* p_buffer_, size_type max_size_)
      : base_t(max_size_),
        p_buffer(p_buffer_)
    {
    }

  private:

    //*************************************************************************
    /// Push a value to the queue.
    //*************************************************************************
    bool push_implementation(const_reference value)
    {
      if (current_size != MAX_SIZE)
      {
        ::new (&p_buffer[write_index]) T(value);

        write_index = get_next_index(write_index, MAX_SIZE);

        ++current_size;

        return true;
      }

      // Queue is full.
      return false;
    }

#if TYPHOON_USING_CPP11 && TYPHOON_NOT_USING_STLPORT && !defined(TYPHOON_QUEUE_MPMC_MUTEX_FORCE_CPP03_IMPLEMENTATION)
    //*************************************************************************
    /// Push a value to the queue.
    //*************************************************************************
    bool push_implementation(rvalue_reference value)
    {
      if (current_size != MAX_SIZE)
      {
        ::new (&p_buffer[write_index]) T(tphn::move(value));

        write_index = get_next_index(write_index, MAX_SIZE);

        ++current_size;

        return true;
      }

      // Queue is full.
      return false;
    }
#endif

#if TYPHOON_USING_CPP11 && TYPHOON_NOT_USING_STLPORT && !defined(TYPHOON_QUEUE_MPMC_MUTEX_FORCE_CPP03_IMPLEMENTATION)
    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    //*************************************************************************
    template <typename ... Args>
    bool emplace_implementation(Args&&... args)
    {
      if (current_size != MAX_SIZE)
      {
        ::new (&p_buffer[write_index]) T(tphn::forward<Args>(args)...);

        write_index = get_next_index(write_index, MAX_SIZE);

        ++current_size;

        return true;
      }

      // Queue is full.
      return false;
    }
#else
    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    //*************************************************************************
    template <typename T1>
    bool emplace_implementation(const T1& value1)
    {
      if (current_size != MAX_SIZE)
      {
        ::new (&p_buffer[write_index]) T(value1);

        write_index = get_next_index(write_index, MAX_SIZE);

        ++current_size;

        return true;
      }

      // Queue is full.
      return false;
    }

    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    //*************************************************************************
    template <typename T1, typename T2>
    bool emplace_implementation(const T1& value1, const T2& value2)
    {
      if (current_size != MAX_SIZE)
      {
        ::new (&p_buffer[write_index]) T(value1, value2);

        write_index = get_next_index(write_index, MAX_SIZE);

        ++current_size;

        return true;
      }

      // Queue is full.
      return false;
    }

    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    //*************************************************************************
    template <typename T1, typename T2, typename T3>
    bool emplace_implementation(const T1& value1, const T2& value2, const T3& value3)
    {
      if (current_size != MAX_SIZE)
      {
        ::new (&p_buffer[write_index]) T(value1, value2, value3);

        write_index = get_next_index(write_index, MAX_SIZE);

        ++current_size;

        return true;
      }

      // Queue is full.
      return false;
    }

    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    //*************************************************************************
    template <typename T1, typename T2, typename T3, typename T4>
    bool emplace_implementation(const T1& value1, const T2& value2, const T3& value3, const T4& value4)
    {
      if (current_size != MAX_SIZE)
      {
        ::new (&p_buffer[write_index]) T(value1, value2, value3, value4);

        write_index = get_next_index(write_index, MAX_SIZE);

        ++current_size;

        return true;
      }

      // Queue is full.
      return false;
    }
#endif

    //*************************************************************************
    /// Pop a value from the queue.
    //*************************************************************************
    bool pop_implementation(reference value)
    {
      if (current_size == 0)
      {
        // Queue is empty
        return false;
      }

#if TYPHOON_USING_CPP11 && TYPHOON_NOT_USING_STLPORT && !defined(TYPHOON_QUEUE_LOCKABLE_FORCE_CPP03_IMPLEMENTATION)
      value = tphn::move(p_buffer[read_index]);
#else
      value = p_buffer[read_index];
#endif

      p_buffer[read_index].~T();

      read_index = get_next_index(read_index, MAX_SIZE);

      --current_size;

      return true;
    }

    //*************************************************************************
    /// Pop a value from the queue and discard.
    //*************************************************************************
    bool pop_implementation()
    {
      if (current_size == 0)
      {
        // Queue is empty
        return false;
      }

      p_buffer[read_index].~T();

      read_index = get_next_index(read_index, MAX_SIZE);

      --current_size;

      return true;
    }

    //*************************************************************************
    /// Peek a value at the front of the queue.
    //*************************************************************************
    reference front_implementation()
    {
      return p_buffer[read_index];
    }

    //*************************************************************************
    /// Peek a value at the front of the queue.
    //*************************************************************************
    const_reference front_implementation() const
    {
      return p_buffer[read_index];
    }

    // Disable copy construction and assignment.
    iqueue_mpmc_mutex(const iqueue_mpmc_mutex&);
    iqueue_mpmc_mutex& operator =(const iqueue_mpmc_mutex&);

    T* p_buffer; ///< The internal buffer.

    mutable tphn::mutex access; ///< The object that locks/unlocks access.
  };

  //***************************************************************************
  ///\ingroup queue_mpmc
  /// A fixed capacity mpmc queue.
  /// This queue supports concurrent access by one producer and one consumer.
  /// \tparam T            The type this queue should support.
  /// \tparam SIZE         The maximum capacity of the queue.
  /// \tparam MEMORY_MODEL The memory model for the queue. Determines the type of the internal counter variables.
  //***************************************************************************
  template <typename T, size_t SIZE, const size_t MEMORY_MODEL = tphn::memory_model::MEMORY_MODEL_LARGE>
  class queue_mpmc_mutex : public tphn::iqueue_mpmc_mutex<T, MEMORY_MODEL>
  {
  private:

    typedef tphn::iqueue_mpmc_mutex<T, MEMORY_MODEL> base_t;

  public:

    typedef typename base_t::size_type size_type;

    TYPHOON_STATIC_ASSERT((SIZE <= tphn::integral_limits<size_type>::max), "Size too large for memory model");

    static TYPHOON_CONSTANT size_type MAX_SIZE = size_type(SIZE);

    //*************************************************************************
    /// Default constructor.
    //*************************************************************************
    queue_mpmc_mutex()
      : base_t(reinterpret_cast<T*>(&buffer[0]), MAX_SIZE)
    {
    }

    //*************************************************************************
    /// Destructor.
    //*************************************************************************
    ~queue_mpmc_mutex()
    {
      base_t::clear();
    }

  private:

    queue_mpmc_mutex(const queue_mpmc_mutex&) TYPHOON_DELETE;
    queue_mpmc_mutex& operator = (const queue_mpmc_mutex&) TYPHOON_DELETE;

#if TYPHOON_USING_CPP11
    queue_mpmc_mutex(queue_mpmc_mutex&&) = delete;
    queue_mpmc_mutex& operator = (queue_mpmc_mutex&&) = delete;
#endif

    /// The uninitialised buffer of T used in the queue_mpmc_mutex.
    typename tphn::aligned_storage<sizeof(T), tphn::alignment_of<T>::value>::type buffer[MAX_SIZE];
  };
}

#endif
#endif
