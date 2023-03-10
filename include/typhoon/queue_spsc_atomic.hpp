///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tpn
https://www.tpncpp.com

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

#ifndef TYPHOON_SPSC_QUEUE_ATOMIC_HPP
#define TYPHOON_SPSC_QUEUE_ATOMIC_HPP

#include "platform.hpp"
#include "alignment.hpp"
#include "parameter_type.hpp"
#include "atomic.hpp"
#include "memory_model.hpp"
#include "integral_limits.hpp"
#include "utility.hpp"
#include "placement_new.hpp"

#include <stddef.h>
#include <stdint.h>

#if TYPHOON_HAS_ATOMIC

namespace tpn
{
  template <const size_t MEMORY_MODEL = tpn::memory_model::MEMORY_MODEL_LARGE>
  class queue_spsc_atomic_base
  {
  public:

    /// The type used for determining the size of queue.
    typedef typename tpn::size_type_lookup<MEMORY_MODEL>::type size_type;

    //*************************************************************************
    /// Is the queue empty?
    /// Accurate from the 'pop' thread.
    /// 'Not empty' is a guess from the 'push' thread.
    //*************************************************************************
    bool empty() const
    {
      return read.load(tpn::memory_order_acquire) == write.load(tpn::memory_order_acquire);
    }

    //*************************************************************************
    /// Is the queue full?
    /// Accurate from the 'push' thread.
    /// 'Not full' is a guess from the 'pop' thread.
    //*************************************************************************
    bool full() const
    {
      size_type next_index = get_next_index(write.load(tpn::memory_order_acquire), RESERVED);

      return (next_index == read.load(tpn::memory_order_acquire));
    }

    //*************************************************************************
    /// How many items in the queue?
    /// Due to concurrency, this is a guess.
    //*************************************************************************
    size_type size() const
    {
      size_type write_index = write.load(tpn::memory_order_acquire);
      size_type read_index = read.load(tpn::memory_order_acquire);

      size_type n;

      if (write_index >= read_index)
      {
        n = write_index - read_index;
      }
      else
      {
        n = RESERVED - read_index + write_index;
      }

      return n;
    }

    //*************************************************************************
    /// How much free space available in the queue.
    /// Due to concurrency, this is a guess.
    //*************************************************************************
    size_type available() const
    {
      return RESERVED - size() - 1;
    }

    //*************************************************************************
    /// How many items can the queue hold.
    //*************************************************************************
    size_type capacity() const
    {
      return RESERVED - 1;
    }

    //*************************************************************************
    /// How many items can the queue hold.
    //*************************************************************************
    size_type max_size() const
    {
      return RESERVED - 1;
    }

  protected:

    queue_spsc_atomic_base(size_type reserved_)
      : write(0),
        read(0),
        RESERVED(reserved_)
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

    tpn::atomic<size_type> write; ///< Where to input new data.
    tpn::atomic<size_type> read;  ///< Where to get the oldest data.
    const size_type RESERVED;     ///< The maximum number of items in the queue.

  private:

    //*************************************************************************
    /// Destructor.
    //*************************************************************************
#if defined(TYPHOON_POLYMORPHIC_SPSC_QUEUE_ATOMIC) || defined(TYPHOON_POLYMORPHIC_CONTAINERS)
  public:
    virtual ~queue_spsc_atomic_base()
    {
    }
#else
  protected:
    ~queue_spsc_atomic_base()
    {
    }
#endif
  };

  //***************************************************************************
  ///\ingroup queue_spsc_atomic
  ///\brief This is the base for all queue_spscs that contain a particular type.
  ///\details Normally a reference to this type will be taken from a derived queue_spsc.
  ///\code
  /// tpn::queue_spsc_atomic<int, 10> myQueue;
  /// tpn::iqueue_spsc_atomic<int>& iQueue = myQueue;
  ///\endcode
  /// This queue supports concurrent access by one producer and one consumer.
  /// \tparam T The type of value that the queue_spsc_atomic holds.
  //***************************************************************************
  template <typename T, const size_t MEMORY_MODEL = tpn::memory_model::MEMORY_MODEL_LARGE>
  class iqueue_spsc_atomic : public queue_spsc_atomic_base<MEMORY_MODEL>
  {
  private:

    typedef typename tpn::queue_spsc_atomic_base<MEMORY_MODEL> base_t;

  public:

    typedef T                          value_type;      ///< The type stored in the queue.
    typedef T&                         reference;       ///< A reference to the type used in the queue.
    typedef const T&                   const_reference; ///< A const reference to the type used in the queue.
#if TYPHOON_USING_CPP11
    typedef T&&                        rvalue_reference;///< An rvalue_reference to the type used in the queue.
#endif
    typedef typename base_t::size_type size_type;       ///< The type used for determining the size of the queue.

    using base_t::write;
    using base_t::read;
    using base_t::RESERVED;
    using base_t::get_next_index;

    //*************************************************************************
    /// Push a value to the queue.
    //*************************************************************************
    bool push(const_reference value)
    {
      size_type write_index = write.load(tpn::memory_order_relaxed);
      size_type next_index  = get_next_index(write_index, RESERVED);

      if (next_index != read.load(tpn::memory_order_acquire))
      {
        ::new (&p_buffer[write_index]) T(value);

        write.store(next_index, tpn::memory_order_release);

        return true;
      }

      // Queue is full.
      return false;
    }

#if TYPHOON_USING_CPP11 && TYPHOON_NOT_USING_STLPORT && !defined(TYPHOON_QUEUE_ATOMIC_FORCE_CPP03_IMPLEMENTATION)
    //*************************************************************************
    /// Push a value to the queue.
    //*************************************************************************
    bool push(rvalue_reference value)
    {
      size_type write_index = write.load(tpn::memory_order_relaxed);
      size_type next_index = get_next_index(write_index, RESERVED);

      if (next_index != read.load(tpn::memory_order_acquire))
      {
        ::new (&p_buffer[write_index]) T(tpn::move(value));

        write.store(next_index, tpn::memory_order_release);

        return true;
      }

      // Queue is full.
      return false;
    }
#endif

#if TYPHOON_USING_CPP11 && TYPHOON_NOT_USING_STLPORT && !defined(TYPHOON_QUEUE_ATOMIC_FORCE_CPP03_IMPLEMENTATION)
    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
    //*************************************************************************
    template <typename ... Args>
    bool emplace(Args&&... args)
    {
      size_type write_index = write.load(tpn::memory_order_relaxed);
      size_type next_index  = get_next_index(write_index, RESERVED);

      if (next_index != read.load(tpn::memory_order_acquire))
      {
        ::new (&p_buffer[write_index]) T(tpn::forward<Args>(args)...);

        write.store(next_index, tpn::memory_order_release);

        return true;
      }

      // Queue is full.
      return false;
    }
#else
    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
    //*************************************************************************
    template <typename T1>
    bool emplace(const T1& value1)
    {
      size_type write_index = write.load(tpn::memory_order_relaxed);
      size_type next_index  = get_next_index(write_index, RESERVED);

      if (next_index != read.load(tpn::memory_order_acquire))
      {
        ::new (&p_buffer[write_index]) T(value1);

        write.store(next_index, tpn::memory_order_release);

        return true;
      }

      // Queue is full.
      return false;
    }

    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
    //*************************************************************************
    template <typename T1, typename T2>
    bool emplace(const T1& value1, const T2& value2)
    {
      size_type write_index = write.load(tpn::memory_order_relaxed);
      size_type next_index  = get_next_index(write_index, RESERVED);

      if (next_index != read.load(tpn::memory_order_acquire))
      {
        ::new (&p_buffer[write_index]) T(value1, value2);

        write.store(next_index, tpn::memory_order_release);

        return true;
      }

      // Queue is full.
      return false;
    }

    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
    //*************************************************************************
    template <typename T1, typename T2, typename T3>
    bool emplace(const T1& value1, const T2& value2, const T3& value3)
    {
      size_type write_index = write.load(tpn::memory_order_relaxed);
      size_type next_index  = get_next_index(write_index, RESERVED);

      if (next_index != read.load(tpn::memory_order_acquire))
      {
        ::new (&p_buffer[write_index]) T(value1, value2, value3);

        write.store(next_index, tpn::memory_order_release);

        return true;
      }

      // Queue is full.
      return false;
    }

    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
    //*************************************************************************
    template <typename T1, typename T2, typename T3, typename T4>
    bool emplace(const T1& value1, const T2& value2, const T3& value3, const T4& value4)
    {
      size_type write_index = write.load(tpn::memory_order_relaxed);
      size_type next_index  = get_next_index(write_index, RESERVED);

      if (next_index != read.load(tpn::memory_order_acquire))
      {
        ::new (&p_buffer[write_index]) T(value1, value2, value3, value4);

        write.store(next_index, tpn::memory_order_release);

        return true;
      }

      // Queue is full.
      return false;
    }
#endif

    //*************************************************************************
    /// Peek the next value in the queue without removing it.
    //*************************************************************************
    bool front(reference value)
    {
      size_type read_index = read.load(tpn::memory_order_relaxed);

      if (read_index == write.load(tpn::memory_order_acquire))
      {
        // Queue is empty
        return false;
      }

      value = p_buffer[read_index];

      return true;
    }

    //*************************************************************************
    /// Pop a value from the queue.
    //*************************************************************************
    bool pop(reference value)
    {
      size_type read_index = read.load(tpn::memory_order_relaxed);

      if (read_index == write.load(tpn::memory_order_acquire))
      {
        // Queue is empty
        return false;
      }

      size_type next_index = get_next_index(read_index, RESERVED);

#if TYPHOON_USING_CPP11 && TYPHOON_NOT_USING_STLPORT && !defined(TYPHOON_QUEUE_LOCKABLE_FORCE_CPP03_IMPLEMENTATION)
      value = tpn::move(p_buffer[read_index]);
#else
      value = p_buffer[read_index];
#endif

      p_buffer[read_index].~T();

      read.store(next_index, tpn::memory_order_release);

      return true;
    }

    //*************************************************************************
    /// Pop a value from the queue and discard.
    //*************************************************************************
    bool pop()
    {
      size_type read_index = read.load(tpn::memory_order_relaxed);

      if (read_index == write.load(tpn::memory_order_acquire))
      {
        // Queue is empty
        return false;
      }

      size_type next_index = get_next_index(read_index, RESERVED);

      p_buffer[read_index].~T();

      read.store(next_index, tpn::memory_order_release);

      return true;
    }

    //*************************************************************************
    /// Peek a value from the front of the queue.
    //*************************************************************************
    reference front()
    {
      size_type read_index = read.load(tpn::memory_order_relaxed);

      return p_buffer[read_index];
    }

    //*************************************************************************
    /// Peek a value from the front of the queue.
    //*************************************************************************
    const_reference front() const
    {
      size_type read_index = read.load(tpn::memory_order_relaxed);

      return p_buffer[read_index];
    }

    //*************************************************************************
    /// Clear the queue.
    /// Must be called from thread that pops the queue or when there is no
    /// possibility of concurrent access.
    //*************************************************************************
    void clear()
    {
      while (pop())
      {
        // Do nothing.
      }
    }

  protected:

    //*************************************************************************
    /// The constructor that is called from derived classes.
    //*************************************************************************
    iqueue_spsc_atomic(T* p_buffer_, size_type reserved_)
      : base_t(reserved_),
        p_buffer(p_buffer_)
    {
    }

  private:

    // Disable copy construction and assignment.
    iqueue_spsc_atomic(const iqueue_spsc_atomic&) TYPHOON_DELETE;
    iqueue_spsc_atomic& operator =(const iqueue_spsc_atomic&) TYPHOON_DELETE;

#if TYPHOON_USING_CPP11
    iqueue_spsc_atomic(iqueue_spsc_atomic&&) = delete;
    iqueue_spsc_atomic& operator =(iqueue_spsc_atomic&&) = delete;
#endif

    T* p_buffer; ///< The internal buffer.
  };

  //***************************************************************************
  ///\ingroup queue_spsc
  /// A fixed capacity spsc queue.
  /// This queue supports concurrent access by one producer and one consumer.
  /// \tparam T            The type this queue should support.
  /// \tparam SIZE         The maximum capacity of the queue.
  /// \tparam MEMORY_MODEL The memory model for the queue. Determines the type of the internal counter variables.
  //***************************************************************************
  template <typename T, size_t SIZE, const size_t MEMORY_MODEL = tpn::memory_model::MEMORY_MODEL_LARGE>
  class queue_spsc_atomic : public iqueue_spsc_atomic<T, MEMORY_MODEL>
  {
  private:

    typedef typename tpn::iqueue_spsc_atomic<T, MEMORY_MODEL> base_t;

  public:

    typedef typename base_t::size_type size_type;

  private:

    static TYPHOON_CONSTANT size_type RESERVED_SIZE = size_type(SIZE + 1);

  public:

    TYPHOON_STATIC_ASSERT((SIZE <= (tpn::integral_limits<size_type>::max - 1)), "Size too large for memory model");

    static TYPHOON_CONSTANT size_type MAX_SIZE = size_type(SIZE);

    //*************************************************************************
    /// Default constructor.
    //*************************************************************************
    queue_spsc_atomic()
      : base_t(reinterpret_cast<T*>(&buffer[0]), RESERVED_SIZE)
    {
    }

    //*************************************************************************
    /// Destructor.
    //*************************************************************************
    ~queue_spsc_atomic()
    {
      base_t::clear();
    }

  private:

    /// The uninitialised buffer of T used in the queue_spsc.
    typename tpn::aligned_storage<sizeof(T), tpn::alignment_of<T>::value>::type buffer[RESERVED_SIZE];
  };
}

#endif

#endif
