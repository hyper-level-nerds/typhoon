///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tpn
https://www.tpncpp.com

Copyright(c) 2014 John Wellbelove, Mark Kitson

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

#ifndef TYPHOON_QUEUE_HPP
#define TYPHOON_QUEUE_HPP

#include "platform.hpp"
#include "iterator.hpp"
#include "alignment.hpp"
#include "array.hpp"
#include "exception.hpp"
#include "error_handler.hpp"
#include "debug_count.hpp"
#include "type_traits.hpp"
#include "parameter_type.hpp"
#include "memory_model.hpp"
#include "integral_limits.hpp"
#include "utility.hpp"
#include "placement_new.hpp"

#include <stddef.h>
#include <stdint.h>

//*****************************************************************************
///\defgroup queue queue
/// A First-in / first-out queue with the capacity defined at compile time,
/// written in the STL style.
///\ingroup containers
//*****************************************************************************

namespace tpn
{
  //***************************************************************************
  /// The base class for queue exceptions.
  ///\ingroup queue
  //***************************************************************************
  class queue_exception : public exception
  {
  public:

    queue_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// The exception thrown when the queue is full.
  ///\ingroup queue
  //***************************************************************************
  class queue_full : public queue_exception
  {
  public:

    queue_full(string_type file_name_, numeric_type line_number_)
      : queue_exception(TYPHOON_ERROR_TEXT("queue:full", TYPHOON_QUEUE_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// The exception thrown when the queue is empty.
  ///\ingroup queue
  //***************************************************************************
  class queue_empty : public queue_exception
  {
  public:

    queue_empty(string_type file_name_, numeric_type line_number_)
      : queue_exception(TYPHOON_ERROR_TEXT("queue:empty", TYPHOON_QUEUE_FILE_ID"B"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// The base class for all queues.
  ///\ingroup queue
  //***************************************************************************
  template <const size_t MEMORY_MODEL = tpn::memory_model::MEMORY_MODEL_LARGE>
  class queue_base
  {
  public:

    /// The type used for determining the size of queue.
    typedef typename tpn::size_type_lookup<MEMORY_MODEL>::type size_type;

    //*************************************************************************
    /// Returns the current number of items in the queue.
    //*************************************************************************
    size_type size() const
    {
      return current_size;
    }

    //*************************************************************************
    /// Returns the maximum number of items that can be queued.
    //*************************************************************************
    size_type max_size() const
    {
      return CAPACITY;
    }

    //*************************************************************************
    /// Returns the maximum number of items that can be queued.
    //*************************************************************************
    size_type capacity() const
    {
      return CAPACITY;
    }

    //*************************************************************************
    /// Checks to see if the queue is empty.
    /// \return <b>true</b> if the queue is empty, otherwise <b>false</b>
    //*************************************************************************
    bool empty() const
    {
      return current_size == 0;
    }

    //*************************************************************************
    /// Checks to see if the queue is full.
    /// \return <b>true</b> if the queue is full, otherwise <b>false</b>
    //*************************************************************************
    bool full() const
    {
      return current_size == CAPACITY;
    }

    //*************************************************************************
    /// Returns the remaining capacity.
    ///\return The remaining capacity.
    //*************************************************************************
    size_type available() const
    {
      return max_size() - size();
    }

  protected:

    //*************************************************************************
    /// The constructor that is called from derived classes.
    //*************************************************************************
    queue_base(size_type max_size_)
      : in(0),
        out(0),
        current_size(0),
        CAPACITY(max_size_)
    {
    }

    //*************************************************************************
    /// Destructor.
    //*************************************************************************
    ~queue_base()
    {
    }

    //*************************************************************************
    /// Increments (and wraps) the 'in' index value to record a queue addition.
    //*************************************************************************
    void add_in()
    {
      if (++in == CAPACITY) TYPHOON_UNLIKELY
      {
        in = 0;
      }

      ++current_size;
      TYPHOON_INCREMENT_DEBUG_COUNT
    }

    //*************************************************************************
    /// Increments (and wraps) the 'out' index value to record a queue deletion.
    //*************************************************************************
    void del_out()
    {
      if (++out == CAPACITY) TYPHOON_UNLIKELY
      {
        out = 0;
      }
      --current_size;
      TYPHOON_DECREMENT_DEBUG_COUNT
    }

    //*************************************************************************
    /// Clears the indexes.
    //*************************************************************************
    void index_clear()
    {
      in = 0;
      out = 0;
      current_size = 0;
      TYPHOON_RESET_DEBUG_COUNT
    }

    size_type in;            ///< Where to input new data.
    size_type out;           ///< Where to get the oldest data.
    size_type current_size;   ///< The number of items in the queue.
    const size_type CAPACITY; ///< The maximum number of items in the queue.
    TYPHOON_DECLARE_DEBUG_COUNT  ///< For internal debugging purposes.

  };

  //***************************************************************************
  ///\ingroup queue
  ///\brief This is the base for all queues that contain a particular type.
  ///\details Normally a reference to this type will be taken from a derived queue.
  ///\code
  /// tpn::queue<int, 10> myQueue;
  /// tpn::iqueue<int>& iQueue = myQueue;
  ///\endcode
  /// \warning This queue cannot be used for concurrent access from multiple threads.
  /// \tparam T The type of value that the queue holds.
  //***************************************************************************
  template <typename T, const size_t MEMORY_MODEL = tpn::memory_model::MEMORY_MODEL_LARGE>
  class iqueue : public tpn::queue_base<MEMORY_MODEL>
  {
  private:

    typedef typename tpn::queue_base<MEMORY_MODEL> base_t;

  public:

    typedef T                          value_type;      ///< The type stored in the queue.
    typedef T&                         reference;       ///< A reference to the type used in the queue.
    typedef const T&                   const_reference; ///< A const reference to the type used in the queue.
#if TYPHOON_USING_CPP11
    typedef T&&                        rvalue_reference;///< An rvalue reference to the type used in the queue.
#endif
    typedef T*                         pointer;         ///< A pointer to the type used in the queue.
    typedef const T*                   const_pointer;   ///< A const pointer to the type used in the queue.
    typedef typename base_t::size_type size_type;       ///< The type used for determining the size of the queue.

    using base_t::in;
    using base_t::out;
    using base_t::CAPACITY;
    using base_t::current_size;
    using base_t::full;
    using base_t::empty;
    using base_t::add_in;
    using base_t::del_out;

    //*************************************************************************
    /// Gets a reference to the value at the front of the queue.<br>
    /// \return A reference to the value at the front of the queue.
    //*************************************************************************
    reference front()
    {
      return p_buffer[out];
    }

    //*************************************************************************
    /// Gets a const reference to the value at the front of the queue.<br>
    /// \return A const reference to the value at the front of the queue.
    //*************************************************************************
    const_reference front() const
    {
      return p_buffer[out];
    }

    //*************************************************************************
    /// Gets a reference to the value at the back of the queue.<br>
    /// \return A reference to the value at the back of the queue.
    //*************************************************************************
    reference back()
    {
      return p_buffer[in == 0 ? CAPACITY - 1 : in - 1];
    }

    //*************************************************************************
    /// Gets a const reference to the value at the back of the queue.<br>
    /// \return A const reference to the value at the back of the queue.
    //*************************************************************************
    const_reference back() const
    {
      return p_buffer[in == 0 ? CAPACITY - 1 : in - 1];
    }

    //*************************************************************************
    /// Adds a value to the queue.
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
    ///\param value The value to push to the queue.
    //*************************************************************************
    void push(const_reference value)
    {
#if defined(TYPHOON_CHECK_PUSH_POP)
      TYPHOON_ASSERT(!full(), TYPHOON_ERROR(queue_full));
#endif
      ::new (&p_buffer[in]) T(value);
      add_in();
    }

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Adds a value to the queue.
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
    ///\param value The value to push to the queue.
    //*************************************************************************
    void push(rvalue_reference value)
    {
#if defined(TYPHOON_CHECK_PUSH_POP)
      TYPHOON_ASSERT(!full(), TYPHOON_ERROR(queue_full));
#endif
      ::new (&p_buffer[in]) T(tpn::move(value));
      add_in();
    }
#endif

#if TYPHOON_USING_CPP11 && TYPHOON_NOT_USING_STLPORT && !defined(TYPHOON_QUEUE_FORCE_CPP03_IMPLEMENTATION)
    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
    ///\param value The value to use to construct the item to push to the queue.
    //*************************************************************************
    template <typename ... Args>
    void emplace(Args && ... args)
    {
#if defined(TYPHOON_CHECK_PUSH_POP)
      TYPHOON_ASSERT(!full(), TYPHOON_ERROR(queue_full));
#endif
      ::new (&p_buffer[in]) T(tpn::forward<Args>(args)...);
      add_in();
    }
#else
    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
    ///\param value The value to use to construct the item to push to the queue.
    //*************************************************************************
    template <typename T1>
    void emplace(const T1& value1)
    {
#if defined(TYPHOON_CHECK_PUSH_POP)
      TYPHOON_ASSERT(!full(), TYPHOON_ERROR(queue_full));
#endif
      ::new (&p_buffer[in]) T(value1);
      add_in();
    }

    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
    ///\param value The value to use to construct the item to push to the queue.
    //*************************************************************************
    template <typename T1, typename T2>
    void emplace(const T1& value1, const T2& value2)
    {
#if defined(TYPHOON_CHECK_PUSH_POP)
      TYPHOON_ASSERT(!full(), TYPHOON_ERROR(queue_full));
#endif
      ::new (&p_buffer[in]) T(value1, value2);
      add_in();
    }

    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
    ///\param value The value to use to construct the item to push to the queue.
    //*************************************************************************
    template <typename T1, typename T2, typename T3>
    void emplace(const T1& value1, const T2& value2, const T3& value3)
    {
#if defined(TYPHOON_CHECK_PUSH_POP)
      TYPHOON_ASSERT(!full(), TYPHOON_ERROR(queue_full));
#endif
      ::new (&p_buffer[in]) T(value1, value2, value3);
      add_in();
    }

    //*************************************************************************
    /// Constructs a value in the queue 'in place'.
    /// If asserts or exceptions are enabled, throws an tpn::queue_full if the queue if already full.
    ///\param value The value to use to construct the item to push to the queue.
    //*************************************************************************
    template <typename T1, typename T2, typename T3, typename T4>
    void emplace(const T1& value1, const T2& value2, const T3& value3, const T4& value4)
    {
#if defined(TYPHOON_CHECK_PUSH_POP)
      TYPHOON_ASSERT(!full(), TYPHOON_ERROR(queue_full));
#endif
      ::new (&p_buffer[in]) T(value1, value2, value3, value4);
      add_in();
    }
#endif

    //*************************************************************************
    /// Clears the queue to the empty state.
    //*************************************************************************
    void clear()
    {
      if TYPHOON_IF_CONSTEXPR(tpn::is_trivially_destructible<T>::value)
      {
        base_t::index_clear();
      }
      else
      {
        while (current_size > 0)
        {
          p_buffer[out].~T();
          del_out();
        }

        in = 0;
        out = 0;
      }
    }

    //*************************************************************************
    /// Removes the oldest value from the back of the queue.
    /// Does nothing if the queue is already empty.
    /// If asserts or exceptions are enabled, throws an tpn::queue_empty if the queue is empty.
    //*************************************************************************
    void pop()
    {
#if defined(TYPHOON_CHECK_PUSH_POP)
      TYPHOON_ASSERT(!empty(), TYPHOON_ERROR(queue_empty));
#endif
      p_buffer[out].~T();
      del_out();
    }

    //*************************************************************************
    /// Gets the oldest value and removes it from the front of the queue.
    /// If asserts or exceptions are enabled, throws an tpn::queue_empty if the queue is empty.
    //*************************************************************************
    void pop_into(reference destination)
    {
      destination = TYPHOON_MOVE(front());
      pop();
    }

    //*************************************************************************
    /// Gets the oldest value and removes it from the front of the queue and
    /// pushes it to the destination container.
    /// If asserts or exceptions are enabled, throws an tpn::queue_empty if the queue is empty.
    /// NOTE: The destination must support a push(T) member function.
    //*************************************************************************
    template <typename TContainer>
    void pop_into(TContainer& destination)
    {
      destination.push(TYPHOON_MOVE(front()));
      pop();
    }

    //*************************************************************************
    /// Assignment operator.
    //*************************************************************************
    iqueue& operator = (const iqueue& rhs)
    {
      if (&rhs != this)
      {
        clear();
        clone(rhs);
      }

      return *this;
    }

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Assignment operator.
    //*************************************************************************
    iqueue& operator = (iqueue&& rhs)
    {
      if (&rhs != this)
      {
        clear();
        move_clone(rhs);
      }

      return *this;
    }
#endif

  protected:

    //*************************************************************************
    /// Make this a clone of the supplied queue
    //*************************************************************************
    void clone(const iqueue& other)
    {
      clear();

      size_type index = other.out;

      for (size_type i = 0; i < other.size(); ++i)
      {
        push(other.p_buffer[index]);
        index = (index == (CAPACITY - 1)) ? 0 : index + 1;
      }
    }

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Make this a moved clone of the supplied queue
    //*************************************************************************
    void move_clone(iqueue&& other)
    {
      clear();

      size_type index = other.out;

      for (size_type i = 0; i < other.size(); ++i)
      {
        push(tpn::move(other.p_buffer[index]));
        index = (index == (CAPACITY - 1)) ? 0 : index + 1;
      }
    }
#endif

    //*************************************************************************
    /// The constructor that is called from derived classes.
    //*************************************************************************
    iqueue(T* p_buffer_, size_type max_size_)
      : base_t(max_size_),
        p_buffer(p_buffer_)
    {
    }

  private:

    // Disable copy construction.
    iqueue(const iqueue&);

    T* p_buffer; ///< The internal buffer.

    //*************************************************************************
    /// Destructor.
    //*************************************************************************
#if defined(TYPHOON_POLYMORPHIC_QUEUE) || defined(TYPHOON_POLYMORPHIC_CONTAINERS)
  public:
    virtual ~iqueue()
    {
    }
#else
  protected:
    ~iqueue()
    {
    }
#endif
  };

  //***************************************************************************
  ///\ingroup queue
  /// A fixed capacity queue.
  /// This queue does not support concurrent access by different threads.
  /// \tparam T            The type this queue should support.
  /// \tparam SIZE         The maximum capacity of the queue.
  /// \tparam MEMORY_MODEL The memory model for the queue. Determines the type of the internal counter variables.
  //***************************************************************************
  template <typename T, const size_t SIZE, const size_t MEMORY_MODEL = tpn::memory_model::MEMORY_MODEL_LARGE>
  class queue : public tpn::iqueue<T, MEMORY_MODEL>
  {
  private:

    typedef tpn::iqueue<T, MEMORY_MODEL> base_t;

  public:

    typedef typename base_t::size_type                                                  size_type;
    typedef typename tpn::aligned_storage<sizeof(T), tpn::alignment_of<T>::value>::type container_type;

    TYPHOON_STATIC_ASSERT((SIZE <= tpn::integral_limits<size_type>::max), "Size too large for memory model");

    static TYPHOON_CONSTANT size_type MAX_SIZE = size_type(SIZE);

    //*************************************************************************
    /// Default constructor.
    //*************************************************************************
    queue()
      : base_t(reinterpret_cast<T*>(&buffer[0]), SIZE)
    {
    }

    //*************************************************************************
    /// Copy constructor
    //*************************************************************************
    queue(const queue& rhs)
      : base_t(reinterpret_cast<T*>(&buffer[0]), SIZE)
    {
      base_t::clone(rhs);
    }

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Copy constructor
    //*************************************************************************
    queue(queue&& rhs)
      : base_t(reinterpret_cast<T*>(&buffer[0]), SIZE)
    {
      base_t::move_clone(tpn::move(rhs));
    }
#endif

    //*************************************************************************
    /// Destructor.
    //*************************************************************************
    ~queue()
    {
      base_t::clear();
    }

    //*************************************************************************
    /// Assignment operator.
    //*************************************************************************
    queue& operator = (const queue& rhs)
    {
      if (&rhs != this)
      {
        base_t::clone(rhs);
      }

      return *this;
    }

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Move assignment operator.
    //*************************************************************************
    queue& operator = (queue&& rhs)
    {
      if (&rhs != this)
      {
        base_t::move_clone(rhs);
      }

      return *this;
    }
#endif

  private:

    /// The uninitialised buffer of T used in the queue.
    container_type buffer[SIZE];
  };
}

#endif
