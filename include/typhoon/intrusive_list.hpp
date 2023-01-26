///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tpn
https://www.tpncpp.com

Copyright(c) 2016 John Wellbelove

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

#ifndef TYPHOON_INTRUSIVE_LIST_HPP
#define TYPHOON_INTRUSIVE_LIST_HPP

#include "platform.hpp"
#include "nullptr.hpp"
#include "type_traits.hpp"
#include "exception.hpp"
#include "error_handler.hpp"
#include "intrusive_links.hpp"
#include "static_assert.hpp"
#include "algorithm.hpp"
#include "iterator.hpp"
#include "functional.hpp"

#include <stddef.h>

#include "private/minmax_push.hpp"

namespace tpn
{
  //***************************************************************************
  /// Exception for the intrusive_list.
  ///\ingroup intrusive_list
  //***************************************************************************
  class intrusive_list_exception : public exception
  {
  public:

    intrusive_list_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Empty exception for the intrusive_list.
  ///\ingroup intrusive_list
  //***************************************************************************
  class intrusive_list_empty : public intrusive_list_exception
  {
  public:

    intrusive_list_empty(string_type file_name_, numeric_type line_number_)
      : intrusive_list_exception(TYPHOON_ERROR_TEXT("intrusive_list:empty", TYPHOON_INTRUSIVE_LIST_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Iterator exception for the intrusive_list.
  ///\ingroup intrusive_list
  //***************************************************************************
  class intrusive_list_iterator_exception : public intrusive_list_exception
  {
  public:

    intrusive_list_iterator_exception(string_type file_name_, numeric_type line_number_)
      : intrusive_list_exception(TYPHOON_ERROR_TEXT("intrusive_list:iterator", TYPHOON_INTRUSIVE_LIST_FILE_ID"B"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Unsorted exception for the intrusive_list.
  ///\ingroup intrusive_list
  //***************************************************************************
  class intrusive_list_unsorted : public intrusive_list_exception
  {
  public:

    intrusive_list_unsorted(string_type file_name_, numeric_type line_number_)
      : intrusive_list_exception(TYPHOON_ERROR_TEXT("intrusive_list:unsorted", TYPHOON_INTRUSIVE_LIST_FILE_ID"C"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Base for intrusive list.
  ///\ingroup intrusive_list
  //***************************************************************************
  template <typename TLink>
  class intrusive_list_base
  {
  public:

    // Node typedef.
    typedef TLink link_type;

    //*************************************************************************
    /// Assigns a range of values to the intrusive_list.
    /// If TYPHOON_THROW_EXCEPTIONS & TYPHOON_DEBUG are defined emits a
    /// intrusive_list_iterator_exception if the iterators are reversed.
    //*************************************************************************
    template <typename TIterator>
    void assign(TIterator first, TIterator last)
    {
#if TYPHOON_IS_DEBUG_BUILD
      intmax_t d = tpn::distance(first, last);
      TYPHOON_ASSERT(d >= 0, TYPHOON_ERROR(intrusive_list_iterator_exception));
#endif

      initialise();

      link_type* p_last_link = &terminal_link;

      // Add all of the elements.
      while (first != last)
      {
        link_type& link = *first++;
        tpn::link_splice<link_type>(p_last_link, link);
        p_last_link = &link;
        ++current_size;
      }
    }

    //*************************************************************************
    /// Pushes a value to the front of the intrusive_list.
    //*************************************************************************
    void push_front(link_type& value)
    {
      insert_link(terminal_link, value);
    }

    //*************************************************************************
    /// Removes a value from the front of the intrusive_list.
    //*************************************************************************
    void pop_front()
    {
#if defined(TYPHOON_CHECK_PUSH_POP)
      TYPHOON_ASSERT(!empty(), TYPHOON_ERROR(intrusive_list_empty));
#endif
      remove_link(get_head());
    }

    //*************************************************************************
    /// Pushes a value to the back of the intrusive_list.
    //*************************************************************************
    void push_back(link_type& value)
    {
      insert_link(terminal_link.link_type::tpn_previous, value);
    }

    //*************************************************************************
    /// Removes a value from the back of the intrusive_list.
    //*************************************************************************
    void pop_back()
    {
#if defined(TYPHOON_CHECK_PUSH_POP)
      TYPHOON_ASSERT(!empty(), TYPHOON_ERROR(intrusive_list_empty));
#endif
      remove_link(get_tail());
    }

    //*************************************************************************
    /// Clears the intrusive_list.
    //*************************************************************************
    void clear()
    {
      initialise();
    }

    //*************************************************************************
    /// Reverses the list.
    //*************************************************************************
    void reverse()
    {
      if (is_trivial_list())
      {
        return;
      }

      link_type* pnode = terminal_link.tpn_next;

      while (pnode != &terminal_link)
      {
        pnode->reverse();
        pnode = pnode->tpn_previous; // Now we've reversed it, we must go to the previous node.
      }

      // Terminal node.
      pnode->reverse();
    }

    //*************************************************************************
    /// Returns true if the list has no elements.
    //*************************************************************************
    bool empty() const
    {
      return (terminal_link.link_type::tpn_next == &terminal_link);
    }

    //*************************************************************************
    /// Returns the number of elements.
    //*************************************************************************
    size_t size() const
    {
      return current_size;
    }

  protected:

    /// The link that acts as the intrusive_list start & end.
    link_type terminal_link;

    size_t current_size; ///< Counts the number of elements in the list.

    //*************************************************************************
    /// Destructor
    //*************************************************************************
    ~intrusive_list_base()
    {
    }

    //*************************************************************************
    /// Is the intrusive_list a trivial length?
    //*************************************************************************
    bool is_trivial_list() const
    {
      return (terminal_link.link_type::tpn_next == &terminal_link) || (terminal_link.link_type::tpn_next->tpn_next == &terminal_link);
    }

    //*************************************************************************
    /// Insert a link.
    //*************************************************************************
    void insert_link(link_type& previous, link_type& new_link)
    {
      // Connect to the intrusive_list.
      tpn::link_splice<link_type>(previous, new_link);
      ++current_size;
    }

    //*************************************************************************
    /// Insert a link.
    //*************************************************************************
    void insert_link(link_type* previous, link_type& new_link)
    {
      // Connect to the intrusive_list.
      tpn::link_splice<link_type>(previous, new_link);
      ++current_size;
    }

    //*************************************************************************
    /// Insert a link.
    //*************************************************************************
    void insert_link(link_type& previous, link_type* new_link)
    {
      // Connect to the intrusive_list.
      tpn::link_splice<link_type>(previous, new_link);
      ++current_size;
    }

    //*************************************************************************
    /// Insert a link.
    //*************************************************************************
    void insert_link(link_type* previous, link_type* new_link)
    {
      // Connect to the intrusive_list.
      tpn::link_splice<link_type>(previous, new_link);
      ++current_size;
    }

    //*************************************************************************
    /// Remove a link.
    //*************************************************************************
    void remove_link(link_type& link)
    {
      tpn::unlink<link_type>(link);
      --current_size;
    }

    //*************************************************************************
    /// Remove a link.
    //*************************************************************************
    void remove_link(link_type* link)
    {
      tpn::unlink<link_type>(*link);
      --current_size;
    }

    //*************************************************************************
    /// Get the head link.
    //*************************************************************************
    link_type* get_head()
    {
      return terminal_link.tpn_next;
    }

    //*************************************************************************
    /// Get the head link.
    //*************************************************************************
    const link_type* get_head() const
    {
      return terminal_link.tpn_next;
    }

    //*************************************************************************
    /// Get the tail link.
    //*************************************************************************
    link_type* get_tail()
    {
      return terminal_link.tpn_previous;
    }

    //*************************************************************************
    /// Get the tail link.
    //*************************************************************************
    const link_type* get_tail() const
    {
      return terminal_link.tpn_previous;
    }

    //*************************************************************************
    /// Initialise the intrusive_list.
    //*************************************************************************
    void initialise()
    {
      tpn::link(terminal_link, terminal_link);
      current_size = 0;
    }
  };

  //***************************************************************************
  /// An intrusive list.
  ///\ingroup intrusive_list
  ///\note TLink must be a base of TValue.
  //***************************************************************************
  template <typename TValue, typename TLink = tpn::bidirectional_link<0> >
  class intrusive_list : public tpn::intrusive_list_base<TLink>
  {
  public:

    // Node typedef.
    typedef typename tpn::intrusive_list_base<TLink>::link_type link_type;

    typedef intrusive_list<TValue, TLink> list_type;

    // STL style typedefs.
    typedef TValue            value_type;
    typedef value_type*       pointer;
    typedef const value_type* const_pointer;
    typedef value_type&       reference;
    typedef const value_type& const_reference;
    typedef size_t            size_type;

    //*************************************************************************
    /// iterator.
    //*************************************************************************
    class iterator : public tpn::iterator<TYPHOON_OR_STD::bidirectional_iterator_tag, value_type>
    {
    public:

      friend class intrusive_list;
      friend class const_iterator;

      iterator()
        : p_value(TYPHOON_NULLPTR)
      {
      }

      iterator(value_type& value)
        : p_value(&value)
      {
      }

      iterator(const iterator& other)
        : p_value(other.p_value)
      {
      }

      iterator& operator ++()
      {
        // Read the appropriate 'tpn_next'.
        p_value = static_cast<value_type*>(p_value->link_type::tpn_next);
        return *this;
      }

      iterator operator ++(int)
      {
        iterator temp(*this);
        // Read the appropriate 'tpn_next'.
        p_value = static_cast<value_type*>(p_value->link_type::tpn_next);
        return temp;
      }

      iterator& operator --()
      {
        // Read the appropriate 'tpn_previous'.
        p_value = static_cast<value_type*>(p_value->link_type::tpn_previous);
        return *this;
      }

      iterator operator --(int)
      {
        iterator temp(*this);
        // Read the appropriate 'tpn_previous'.
        p_value = static_cast<value_type*>(p_value->link_type::tpn_previous);
        return temp;
      }

      iterator& operator =(const iterator& other)
      {
        p_value = other.p_value;
        return *this;
      }

      reference operator *() const
      {
        return *p_value;
      }

      pointer operator &() const
      {
        return p_value;
      }

      pointer operator ->() const
      {
        return p_value;
      }

      friend bool operator == (const iterator& lhs, const iterator& rhs)
      {
        return lhs.p_value == rhs.p_value;
      }

      friend bool operator != (const iterator& lhs, const iterator& rhs)
      {
        return !(lhs == rhs);
      }

    private:

      value_type* p_value;
    };

    //*************************************************************************
    /// const_iterator
    //*************************************************************************
    class const_iterator : public tpn::iterator<TYPHOON_OR_STD::bidirectional_iterator_tag, const value_type>
    {
    public:

      friend class intrusive_list;

      const_iterator()
        : p_value(TYPHOON_NULLPTR)
      {
      }

      const_iterator(const value_type& value)
        : p_value(&value)
      {
      }

      const_iterator(const typename intrusive_list::iterator& other)
        : p_value(other.p_value)
      {
      }

      const_iterator(const const_iterator& other)
        : p_value(other.p_value)
      {
      }

      const_iterator& operator ++()
      {
        // Read the appropriate 'tpn_next'.
        p_value = static_cast<value_type*>(p_value->link_type::tpn_next);
        return *this;
      }

      const_iterator operator ++(int)
      {
        const_iterator temp(*this);
        // Read the appropriate 'tpn_next'.
        p_value = static_cast<value_type*>(p_value->link_type::tpn_next);
        return temp;
      }

      const_iterator& operator --()
      {
        // Read the appropriate 'tpn_previous'.
        p_value = static_cast<value_type*>(p_value->link_type::tpn_previous);
        return *this;
      }

      const_iterator operator --(int)
      {
        const_iterator temp(*this);
        // Read the appropriate 'tpn_previous'.
        p_value = static_cast<value_type*>(p_value->link_type::tpn_previous);
        return temp;
      }

      const_iterator& operator =(const const_iterator& other)
      {
        p_value = other.p_value;
        return *this;
      }

      const_reference operator *() const
      {
        return *p_value;
      }

      const_pointer operator &() const
      {
        return p_value;
      }

      const_pointer operator ->() const
      {
        return p_value;
      }

      friend bool operator == (const const_iterator& lhs, const const_iterator& rhs)
      {
        return lhs.p_value == rhs.p_value;
      }

      friend bool operator != (const const_iterator& lhs, const const_iterator& rhs)
      {
        return !(lhs == rhs);
      }

    private:

      const value_type* p_value;
    };

    typedef typename tpn::iterator_traits<iterator>::difference_type difference_type;

    //*************************************************************************
    /// Constructor.
    //*************************************************************************
    intrusive_list()
    {
      this->initialise();
    }

    //*************************************************************************
    /// Destructor.
    //*************************************************************************
    ~intrusive_list()
    {
      this->clear();
    }

    //*************************************************************************
    /// Constructor from range
    //*************************************************************************
    template <typename TIterator>
    intrusive_list(TIterator first, TIterator last, typename tpn::enable_if<!tpn::is_integral<TIterator>::value, int>::type = 0)
    {
      this->assign(first, last);
    }

    //*************************************************************************
    /// Gets the beginning of the intrusive_list.
    //*************************************************************************
    iterator begin()
    {
      return iterator(static_cast<value_type&>(*this->get_head()));
    }

    //*************************************************************************
    /// Gets the beginning of the intrusive_list.
    //*************************************************************************
    const_iterator begin() const
    {
      return const_iterator(static_cast<const value_type&>(*this->get_head()));
    }

    //*************************************************************************
    /// Gets the beginning of the intrusive_list.
    //*************************************************************************
    const_iterator cbegin() const
    {
      return const_iterator(static_cast<const value_type&>(*this->get_head()));
    }

    //*************************************************************************
    /// Gets the end of the intrusive_list.
    //*************************************************************************
    iterator end()
    {
      return iterator(static_cast<value_type&>(this->terminal_link));
    }

    //*************************************************************************
    /// Gets the end of the intrusive_list.
    //*************************************************************************
    const_iterator end() const
    {
      return const_iterator(static_cast<const value_type&>(this->terminal_link));
    }

    //*************************************************************************
    /// Gets the end of the intrusive_list.
    //*************************************************************************
    const_iterator cend() const
    {
      return const_iterator(static_cast<const value_type&>(this->terminal_link));
    }

    //*************************************************************************
    /// Gets a reference to the first element.
    //*************************************************************************
    reference front()
    {
      return *static_cast<value_type*>(this->get_head());
    }

    //*************************************************************************
    /// Gets a const reference to the first element.
    //*************************************************************************
    const_reference front() const
    {
      return *static_cast<const value_type*>(this->get_head());
    }

    //*************************************************************************
    /// Gets a reference to the last element.
    //*************************************************************************
    reference back()
    {
      return *static_cast<value_type*>(this->get_tail());
    }

    //*************************************************************************
    /// Gets a const reference to the last element.
    //*************************************************************************
    const_reference back() const
    {
      return *static_cast<const value_type*>(this->get_tail());
    }

    //*************************************************************************
    /// Inserts a value to the intrusive_list before the specified position.
    //*************************************************************************
    iterator insert(const_iterator position, value_type& value)
    {
      this->insert_link(position.p_value->link_type::tpn_previous, value);
      return iterator(value);
    }

    //*************************************************************************
    /// Inserts a range of values to the intrusive_list after the specified position.
    //*************************************************************************
    template <typename TIterator>
    void insert(const_iterator position, TIterator first, TIterator last)
    {
      while (first != last)
      {
        // Set up the next free link.
        this->insert_link(*position.p_value->link_type::tpn_previous, *first);
        ++first;
      }
    }

    //*************************************************************************
    /// Erases the value at the specified position.
    //*************************************************************************
    iterator erase(iterator position)
    {
      iterator next(position);
      ++next;

      this->remove_link(*position.p_value);

      return next;
    }

    //*************************************************************************
    /// Erases the value at the specified position.
    //*************************************************************************
    iterator erase(const_iterator position)
    {
      iterator next(position);
      ++next;

      this->remove_link(*position.p_value);

      return next;
    }

    //*************************************************************************
    /// Erases a range of elements.
    /// Clears the links after erasing if AUTO or CHECKED.
    //*************************************************************************
    iterator erase(const_iterator first, const_iterator last)
    {
      const link_type* cp_first = first.p_value;
      const link_type* cp_last  = last.p_value;

      link_type* p_first = const_cast<link_type*>(cp_first);
      link_type* p_last  = const_cast<link_type*>(cp_last);

      // Join the ends.
      tpn::link<link_type>(p_first->tpn_previous, p_last);

      this->current_size -= tpn::distance(first, last);

      if (p_last == &this->terminal_link)
      {
        return end();
      }
      else
      {
        return iterator(*static_cast<value_type*>(p_last));
      }
    }

    //*************************************************************************
    /// Removes all but the one element from every consecutive group of equal
    /// elements in the container.
    //*************************************************************************
    template <typename TIsEqual>
    void unique(TIsEqual isEqual)
    {
      if (this->empty())
      {
        return;
      }

      iterator i_item = begin();
      ++i_item;
      iterator i_previous = begin();

      while (i_item != end())
      {
        if (isEqual(*i_previous, *i_item))
        {
          i_item = erase(i_item);
        }
        else
        {
          i_previous = i_item;
          ++i_item;
        }
      }
    }

    //*************************************************************************
    /// Sort using in-place merge sort algorithm.
    //*************************************************************************
    void sort()
    {
      sort(tpn::less<value_type>());
    }

    //*************************************************************************
    /// Stable sort using in-place merge sort algorithm.
    /// Copyright 2001 Simon Tatham.
    ///
    /// Permission is hereby granted, free of charge, to any person
    /// obtaining a copy of this software and associated documentation
    /// files (the "Software"), to deal in the Software without
    /// restriction, including without limitation the rights to use,
    /// copy, modify, merge, publish, distribute, sublicense, and/or
    /// sell copies of the Software, and to permit persons to whom the
    /// Software is furnished to do so, subject to the following
    /// conditions:
    ///
    /// The above copyright notice and this permission notice shall be
    /// included in all copies or substantial portions of the Software.
    ///
    /// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    /// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    /// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    /// NONINFRINGEMENT.  IN NO EVENT SHALL SIMON TATHAM BE LIABLE FOR
    /// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    /// CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
    /// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    /// SOFTWARE.
    //*************************************************************************
    template <typename TCompare>
    void sort(TCompare compare)
    {
      iterator i_left;
      iterator i_right;
      iterator i_node;
      iterator i_head;
      iterator i_tail;
      int      list_size = 1;
      int      number_of_merges;
      int      left_size;
      int      right_size;

      if (this->is_trivial_list())
      {
        return;
      }

      while (true)
      {
        i_left = begin();
        i_head = end();
        i_tail = end();

        number_of_merges = 0;  // Count the number of merges we do in this pass.

        while (i_left != end())
        {
          ++number_of_merges;  // There exists a merge to be done.
          i_right = i_left;
          left_size = 0;

          // Step 'list_size' places along from left
          for (int i = 0; i < list_size; ++i)
          {
            ++left_size;
            ++i_right;

            if (i_right == end())
            {
              break;
            }
          }

          // If right hasn't fallen off end, we have two lists to merge.
          right_size = list_size;

          // Now we have two lists. Merge them.
          while (left_size > 0 || (right_size > 0 && i_right != end()))
          {
            // Decide whether the next node of merge comes from left or right.
            if (left_size == 0)
            {
              // Left is empty. The node must come from right.
              i_node = i_right++;
              --right_size;
            }
            else if (right_size == 0 || i_right == end())
            {
              // Right is empty. The node must come from left.
              i_node = i_left++;
              --left_size;
            }
            else if (!compare(*i_right, *i_left))
            {
              // First node of left is lower or same. The node must come from left.
              i_node = i_left++;
              --left_size;
            }
            else
            {
              // First node of right is lower. The node must come from right.
              i_node = i_right;
              ++i_right;
              --right_size;
            }

            // Add the next node to the merged head.
            if (i_head == end())
            {
              tpn::link<link_type>(i_head.p_value, i_node.p_value);
              i_head = i_node;
              i_tail = i_node;
            }
            else
            {
              tpn::link<link_type>(i_tail.p_value, i_node.p_value);
              i_tail = i_node;
            }

            tpn::link<link_type>(i_tail.p_value, this->terminal_link);
          }

          // Now left has stepped `list_size' places along, and right has too.
          i_left = i_right;
        }

        // If we have done only one merge, we're finished.
        if (number_of_merges <= 1)   // Allow for number_of_merges == 0, the empty head case
        {
          return;
        }

        // Otherwise repeat, merging lists twice the size
        list_size *= 2;
      }
    }

    //*************************************************************************
    // Removes the values specified.
    //*************************************************************************
    void remove(const_reference value)
    {
      iterator i_item = begin();

      while (i_item != end())
      {
        if (*i_item == value)
        {
          i_item = erase(i_item);
        }
        else
        {
          ++i_item;
        }
      }
    }

    //*************************************************************************
    /// Removes according to a predicate.
    //*************************************************************************
    template <typename TPredicate>
    void remove_if(TPredicate predicate)
    {
      iterator i_item = begin();

      while (i_item != end())
      {
        if (predicate(*i_item))
        {
          i_item = erase(i_item);
        }
        else
        {
          ++i_item;
        }
      }
    }

    //*************************************************************************
    /// Splice another list into this one.
    //*************************************************************************
    void splice(iterator position, list_type& other)
    {
      // No point splicing to ourself!
      if (&other != this)
      {
        if (!other.empty())
        {
          link_type& first = *other.get_head();
          link_type& last = *other.get_tail();

          if (&other != this)
          {
            this->current_size += other.size();
          }

          link_type& after = *position.p_value;
          link_type& before = *after.tpn_previous;

          tpn::link<link_type>(before, first);
          tpn::link<link_type>(last, after);

          other.initialise();
        }
      }
    }

    //*************************************************************************
    /// Splice an element from another list into this one.
    //*************************************************************************
    void splice(iterator position, list_type& other, iterator isource)
    {
      link_type& before = *position.p_value->link_type::tpn_previous;

      tpn::unlink<link_type>(*isource.p_value);
      tpn::link_splice<link_type>(before, *isource.p_value);

      if (&other != this)
      {
        ++this->current_size;
        --other.current_size;
      }
    }

    //*************************************************************************
    /// Splice a range of elements from another list into this one.
    //*************************************************************************
    void splice(iterator position, list_type& other, iterator begin_, iterator end_)
    {
      if (!other.empty())
      {
        if (&other != this)
        {
          size_t n = tpn::distance(begin_, end_);
          this->current_size += n;
          other.current_size -= n;
        }

        link_type& first = *begin_.p_value;
        link_type& last  = *end_.p_value->link_type::tpn_previous;

        // Unlink from the source list.
        tpn::unlink(first, last);

        // Fix our links.
        link_type& before = *position.p_value->link_type::tpn_previous;

        tpn::link_splice<link_type>(before, first, last);
      }
    }

    //*************************************************************************
    /// Merge another list into this one. Both lists should be sorted.
    //*************************************************************************
    void merge(list_type& other)
    {
      merge(other, tpn::less<value_type>());
    }

    //*************************************************************************
    /// Merge another list into this one. Both lists should be sorted.
    //*************************************************************************
    template <typename TCompare>
    void merge(list_type& other, TCompare compare)
    {
      if ((this != &other) && !other.empty())
      {
#if TYPHOON_IS_DEBUG_BUILD
        TYPHOON_ASSERT(tpn::is_sorted(other.begin(), other.end(), compare), TYPHOON_ERROR(intrusive_list_unsorted));
        TYPHOON_ASSERT(tpn::is_sorted(begin(), end(), compare), TYPHOON_ERROR(intrusive_list_unsorted));
#endif

        value_type* other_begin = static_cast<value_type*>(other.get_head());
        value_type* other_end   = static_cast<value_type*>(&other.terminal_link);

        value_type* this_begin = static_cast<value_type*>(this->get_head());
        value_type* this_end   = static_cast<value_type*>(&this->terminal_link);

        while ((this_begin != this_end) && (other_begin != other_end))
        {
          // Find the place to insert.
          while ((this_begin != this_end) && !(compare(*other_begin, *this_begin)))
          {
            this_begin = static_cast<value_type*>(this_begin->link_type::tpn_next);
          }

          // Insert.
          if (this_begin != this_end)
          {
            while ((other_begin != other_end) && (compare(*other_begin, *this_begin)))
            {
              value_type* value = other_begin;
              other_begin = static_cast<value_type*>(other_begin->link_type::tpn_next);
              tpn::link_splice<link_type>(*this_begin->link_type::tpn_previous, *value);
            }
          }
        }

        // Any left over?
        if ((this_begin == this_end) && (other_begin != other_end))
        {
          tpn::link_splice<link_type>(*this->get_tail(), *other_begin, *other_end->link_type::tpn_previous);
        }

        this->current_size += other.size();

        other.initialise();
      }
    }

  private:

    // Disabled.
    intrusive_list(const intrusive_list& other);
    intrusive_list& operator = (const intrusive_list& rhs);
  };
}

#include "private/minmax_pop.hpp"

#endif
