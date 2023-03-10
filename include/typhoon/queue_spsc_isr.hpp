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

#ifndef TYPHOON_SPSC_QUEUE_ISR_HPP
#define TYPHOON_SPSC_QUEUE_ISR_HPP

#include "platform.hpp"
#include "alignment.hpp"
#include "parameter_type.hpp"
#include "memory_model.hpp"
#include "integral_limits.hpp"
#include "utility.hpp"
#include "placement_new.hpp"

#include <stddef.h>
#include <stdint.h>

namespace tpn
{
  template <typename T, const size_t MEMORY_MODEL = tpn::memory_model::MEMORY_MODEL_LARGE>
  class queue_spsc_isr_base
  {
  public:

    /// The type used for determining the size of queue.
    typedef typename tpn::size_type_lookup<MEMORY_MODEL>::type size_type;

    typedef T        value_type;      ///< The type stored in the queue.
    typedef T&       reference;       ///< A reference to the type used in the queue.
    typedef const T& const_reference; ///< A const reference to the type used in the queue.
#if TYPHOON_USING_CPP11
    typedef T&&      rvalue_reference;///< An rvalue reference to the type used in the queue.
#endif

    //*************************************************************************
    /// Push a value to the queue from an ISR.
    //*************************************************************************
    bool push_from_isr(const_reference value)
    {
      return push_implementation(value);
    }

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Push a value to the queue from an ISR.
    //*************************************************************************
    bool push_from_isr(rvalue_reference value)
    {
      return push_implementation(tpn::move(value));
    }
#endif

    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
    ///\param value The value to use to construct the item to push to the queue.
    //*************************************************************************
#if TYPHOON_USING_CPP11 && TYPHOON_NOT_USING_STLPORT && !defined(TYPHOON_QUEUE_ISR_FORCE_CPP03_IMPLEMENTATION)
    template <typename ... Args>
    bool emplace_from_isr(Args&&... args)
    {
      return emplace_implementation(tpn::forward<Args>(args)...);
    }
#endif

    //*************************************************************************
    /// Pop a value from the queue from an ISR
    //*************************************************************************
    bool pop_from_isr(reference value)
    {
      return pop_implementation(value);
    }

    //*************************************************************************
    /// Pop a value from the queue from an ISR, and discard.
    //*************************************************************************
    bool pop_from_isr()
    {
      return pop_implementation();
    }

    //*************************************************************************
    /// Peek a value at the front of the queue from an ISR
    //*************************************************************************
    reference front_from_isr()
    {
      return front_implementation();
    }

    //*************************************************************************
    /// Peek a value at the front of the queue from an ISR
    //*************************************************************************
    const_reference front_from_isr() const
    {
      return front_implementation();
    }

    //*************************************************************************
    /// How much free space available in the queue.
    /// Called from ISR.
    //*************************************************************************
    size_type available_from_isr() const
    {
      return MAX_SIZE - current_size;
    }

    //*************************************************************************
    /// Clear the queue from the ISR.
    //*************************************************************************
    void clear_from_isr()
    {
      while (pop_implementation())
      {
        // Do nothing.
      }
    }

    //*************************************************************************
    /// Is the queue empty?
    /// Called from ISR.
    //*************************************************************************
    bool empty_from_isr() const
    {
      return (current_size == 0);
    }

    //*************************************************************************
    /// Is the queue full?
    /// Called from ISR.
    //*************************************************************************
    bool full_from_isr() const
    {
      return (current_size == MAX_SIZE);
    }

    //*************************************************************************
    /// How many items in the queue?
    /// Called from ISR.
    //*************************************************************************
    size_type size_from_isr() const
    {
      return current_size;
    }

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

    queue_spsc_isr_base(T* p_buffer_, size_type max_size_)
      : p_buffer(p_buffer_),
        write_index(0),
        read_index(0),
        current_size(0),
        MAX_SIZE(max_size_)
    {
    }

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

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Push a value to the queue.
    //*************************************************************************
    bool push_implementation(rvalue_reference value)
    {
      if (current_size != MAX_SIZE)
      {
        ::new (&p_buffer[write_index]) T(tpn::move(value));

        write_index = get_next_index(write_index, MAX_SIZE);

        ++current_size;

        return true;
      }

      // Queue is full.
      return false;
    }
#endif

#if TYPHOON_USING_CPP11 && TYPHOON_NOT_USING_STLPORT && !defined(TYPHOON_QUEUE_ISR_FORCE_CPP03_IMPLEMENTATION)
    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
    ///\param value The value to use to construct the item to push to the queue.
    //*************************************************************************
    template <typename ... Args>
    bool emplace_implementation(Args&&... args)
    {
      if (current_size != MAX_SIZE)
      {
        ::new (&p_buffer[write_index]) T(tpn::forward<Args>(args)...);

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
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
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
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
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
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
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
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
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
      value = tpn::move(p_buffer[read_index]);
#else
      value = p_buffer[read_index];
#endif

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

    T* p_buffer;              ///< The internal buffer.
    size_type write_index;    ///< Where to input new data.
    size_type read_index;     ///< Where to get the oldest data.
    size_type current_size;   ///< The current size of the queue.
    const size_type MAX_SIZE; ///< The maximum number of items in the queue.

  private:

    //*************************************************************************
    /// Destructor.
    //*************************************************************************
#if defined(TYPHOON_POLYMORPHIC_SPSC_QUEUE_ISR) || defined(TYPHOON_POLYMORPHIC_CONTAINERS)
  public:
    virtual ~queue_spsc_isr_base()
    {
    }
#else
  protected:
    ~queue_spsc_isr_base()
    {
    }
#endif
  };

  //***************************************************************************
  ///\ingroup queue_spsc
  ///\brief This is the base for all queue_spsc_isrs that contain a particular type.
  ///\details Normally a reference to this type will be taken from a derived queue_spsc_isr.
  ///\code
  /// tpn::queue_spsc_isr_isr<int, 10> myQueue;
  /// tpn::iqueue_isr<int>& iQueue = myQueue;
  ///\endcode
  /// This queue supports concurrent access by one producer and one consumer.
  /// \tparam T The type of value that the queue_spsc_isr holds.
  //***************************************************************************
  template <typename T, typename TAccess, const size_t MEMORY_MODEL = tpn::memory_model::MEMORY_MODEL_LARGE>
  class iqueue_spsc_isr : public queue_spsc_isr_base<T, MEMORY_MODEL>
  {
  private:

    typedef queue_spsc_isr_base<T, MEMORY_MODEL> base_t;

  public:

    typedef typename base_t::value_type       value_type;      ///< The type stored in the queue.
    typedef typename base_t::reference        reference;       ///< A reference to the type used in the queue.
    typedef typename base_t::const_reference  const_reference; ///< A const reference to the type used in the queue.
#if TYPHOON_USING_CPP11
    typedef typename base_t::rvalue_reference rvalue_reference;///< A const reference to the type used in the queue.
#endif
    typedef typename base_t::size_type        size_type;       ///< The type used for determining the size of the queue.

    //*************************************************************************
    /// Push a value to the queue.
    //*************************************************************************
    bool push(const_reference value)
    {
      TAccess::lock();

      bool result = this->push_implementation(value);

      TAccess::unlock();

      return result;
    }

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Push a value to the queue.
    //*************************************************************************
    bool push(rvalue_reference value)
    {
      TAccess::lock();

      bool result = this->push_implementation(tpn::move(value));

      TAccess::unlock();

      return result;
    }
#endif

    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
    //*************************************************************************
#if TYPHOON_USING_CPP11 && TYPHOON_NOT_USING_STLPORT && !defined(TYPHOON_QUEUE_ISR_FORCE_CPP03_IMPLEMENTATION)
    template <typename ... Args>
    bool emplace(Args&&... args)
    {
      TAccess::lock();

      bool result = this->emplace_implementation(tpn::forward<Args>(args)...);

      TAccess::unlock();

      return result;
    }
#else
    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
    //*************************************************************************
    template <typename T1>
    bool emplace(const T1& value1)
    {
      TAccess::lock();

      bool result = this->emplace_implementation(value1);

      TAccess::unlock();

      return result;
    }

    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
    //*************************************************************************
    template <typename T1, typename T2>
    bool emplace(const T1& value1, const T2& value2)
    {
      TAccess::lock();

      bool result = this->emplace_implementation(value1, value2);

      TAccess::unlock();

      return result;
    }

    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
    //*************************************************************************
    template <typename T1, typename T2, typename T3>
    bool emplace(const T1& value1, const T2& value2, const T3& value3)
    {
      TAccess::lock();

      bool result = this->emplace_implementation(value1, value2, value3);

      TAccess::unlock();

      return result;
    }

    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
    //*************************************************************************
    template <typename T1, typename T2, typename T3, typename T4>
    bool emplace(const T1& value1, const T2& value2, const T3& value3, const T4& value4)
    {
      TAccess::lock();

      bool result = this->emplace_implementation(value1, value2, value3, value4);

      TAccess::unlock();

      return result;
    }
#endif

    //*************************************************************************
    /// Pop a value from the queue.
    //*************************************************************************
    bool pop(reference value)
    {
      TAccess::lock();

      bool result = this->pop_implementation(value);

      TAccess::unlock();

      return result;
    }

    //*************************************************************************
    /// Pop a value from the queue and discard.
    //*************************************************************************
    bool pop()
    {
      TAccess::lock();

      bool result = this->pop_implementation();

      TAccess::unlock();

      return result;
    }

    //*************************************************************************
    /// Peek a value at the front of the queue.
    //*************************************************************************
    reference front()
    {
      TAccess::lock();

      reference result = this->front_implementation();

      TAccess::unlock();

      return result;
    }

    //*************************************************************************
    /// Peek a value at the front of the queue.
    //*************************************************************************
    const_reference front() const
    {
      TAccess::lock();

      const_reference result = this->front_implementation();

      TAccess::unlock();

      return result;
    }

    //*************************************************************************
    /// Clear the queue.
    //*************************************************************************
    void clear()
    {
      TAccess::lock();

      while (this->pop_implementation())
      {
        // Do nothing.
      }

      TAccess::unlock();
    }

    //*************************************************************************
    /// Is the queue empty?
    //*************************************************************************
    bool empty() const
    {
      TAccess::lock();

      size_type result = (this->current_size == 0);

      TAccess::unlock();

      return result;
    }

    //*************************************************************************
    /// Is the queue full?
    //*************************************************************************
    bool full() const
    {
      TAccess::lock();

      size_type result = (this->current_size == this->MAX_SIZE);

      TAccess::unlock();

      return result;
    }

    //*************************************************************************
    /// How many items in the queue?
    //*************************************************************************
    size_type size() const
    {
      TAccess::lock();

      size_type result = this->current_size;

      TAccess::unlock();

      return result;
    }

    //*************************************************************************
    /// How much free space available in the queue.
    //*************************************************************************
    size_type available() const
    {
      TAccess::lock();

      size_type result = this->MAX_SIZE - this->current_size;

      TAccess::unlock();

      return result;
    }

  protected:

    //*************************************************************************
    /// The constructor that is called from derived classes.
    //*************************************************************************
    iqueue_spsc_isr(T* p_buffer_, size_type max_size_)
      : base_t(p_buffer_, max_size_)
    {
    }

  private:

    // Disable copy construction and assignment.
    iqueue_spsc_isr(const iqueue_spsc_isr&) TYPHOON_DELETE;
    iqueue_spsc_isr& operator =(const iqueue_spsc_isr&) TYPHOON_DELETE;

#if TYPHOON_USING_CPP11
    iqueue_spsc_isr(iqueue_spsc_isr&&) = delete;
    iqueue_spsc_isr& operator =(iqueue_spsc_isr&&) = delete;
#endif

    TAccess access; ///< The object that locks/unlocks interrupts.
  };

  //***************************************************************************
  ///\ingroup queue_spsc
  /// A fixed capacity spsc queue.
  /// This queue supports concurrent access by one producer and one consumer.
  /// \tparam T            The type this queue should support.
  /// \tparam SIZE         The maximum capacity of the queue.
  /// \tparam TAccess      The type that will lock and unlock interrupts.
  /// \tparam MEMORY_MODEL The memory model for the queue. Determines the type of the internal counter variables.
  //***************************************************************************
  template <typename T, size_t SIZE, typename TAccess, const size_t MEMORY_MODEL = tpn::memory_model::MEMORY_MODEL_LARGE>
  class queue_spsc_isr : public tpn::iqueue_spsc_isr<T, TAccess, MEMORY_MODEL>
  {
  private:

    typedef tpn::iqueue_spsc_isr<T, TAccess, MEMORY_MODEL> base_t;

  public:

    typedef typename base_t::size_type size_type;

    TYPHOON_STATIC_ASSERT((SIZE <= tpn::integral_limits<size_type>::max), "Size too large for memory model");

    static TYPHOON_CONSTANT size_type MAX_SIZE = size_type(SIZE);

    //*************************************************************************
    /// Default constructor.
    //*************************************************************************
    queue_spsc_isr()
      : base_t(reinterpret_cast<T*>(&buffer[0]), MAX_SIZE)
    {
    }

    //*************************************************************************
    /// Destructor.
    //*************************************************************************
    ~queue_spsc_isr()
    {
      base_t::clear();
    }

  private:

    queue_spsc_isr(const queue_spsc_isr&);
    queue_spsc_isr& operator = (const queue_spsc_isr&);

#if TYPHOON_USING_CPP11
    queue_spsc_isr(queue_spsc_isr&&) = delete;
    queue_spsc_isr& operator =(queue_spsc_isr&&) = delete;
#endif

    /// The uninitialised buffer of T used in the queue_spsc_isr.
    typename tpn::aligned_storage<sizeof(T), tpn::alignment_of<T>::value>::type buffer[MAX_SIZE];
  };
}

#endif
