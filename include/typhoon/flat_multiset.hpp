///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tpn
https://www.tpncpp.com

Copyright(c) 2015 John Wellbelove

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

#ifndef TYPHOON_FLAT_MULTISET_HPP
#define TYPHOON_FLAT_MULTISET_HPP

#include "platform.hpp"
#include "reference_flat_multiset.hpp"
#include "pool.hpp"
#include "placement_new.hpp"
#include "nth_type.hpp"
#include "type_traits.hpp"
#include "initializer_list.hpp"

#include "private/comparator_is_transparent.hpp"

//*****************************************************************************
///\defgroup flat_multiset flat_multiset
/// A flat_multiset with the capacity defined at compile time.
/// Has insertion of O(N) and find of O(logN)
/// Duplicate entries and not allowed.
///\ingroup containers
//*****************************************************************************

namespace tpn
{
  //***************************************************************************
  /// The base class for specifically sized flat_multisets.
  /// Can be used as a reference type for all flat_multisets containing a specific type.
  ///\ingroup flat_multiset
  //***************************************************************************
  template <typename T, typename TKeyCompare = tpn::less<T> >
  class iflat_multiset : private tpn::ireference_flat_multiset<T, TKeyCompare>
  {
  private:

    typedef tpn::ireference_flat_multiset<T, TKeyCompare> refset_t;
    typedef typename refset_t::lookup_t lookup_t;
    typedef tpn::ipool storage_t;

    typedef const T& key_parameter_t;

  public:

    typedef T                 key_type;
    typedef T                 value_type;
    typedef TKeyCompare       key_compare;
    typedef value_type& reference;
    typedef const value_type& const_reference;
#if TYPHOON_USING_CPP11
    typedef value_type&& rvalue_reference;
#endif
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef size_t            size_type;

    typedef typename refset_t::iterator       iterator;
    typedef typename refset_t::const_iterator const_iterator;

    typedef TYPHOON_OR_STD::reverse_iterator<iterator>       reverse_iterator;
    typedef TYPHOON_OR_STD::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef typename tpn::iterator_traits<iterator>::difference_type difference_type;

  public:

    //*********************************************************************
    /// Returns an iterator to the beginning of the flat_multiset.
    ///\return An iterator to the beginning of the flat_multiset.
    //*********************************************************************
    iterator begin()
    {
      return refset_t::begin();
    }

    //*********************************************************************
    /// Returns a const_iterator to the beginning of the flat_multiset.
    ///\return A const iterator to the beginning of the flat_multiset.
    //*********************************************************************
    const_iterator begin() const
    {
      return refset_t::begin();
    }

    //*********************************************************************
    /// Returns an iterator to the end of the flat_multiset.
    ///\return An iterator to the end of the flat_multiset.
    //*********************************************************************
    iterator end()
    {
      return refset_t::end();
    }

    //*********************************************************************
    /// Returns a const_iterator to the end of the flat_multiset.
    ///\return A const iterator to the end of the flat_multiset.
    //*********************************************************************
    const_iterator end() const
    {
      return refset_t::end();
    }

    //*********************************************************************
    /// Returns a const_iterator to the beginning of the flat_multiset.
    ///\return A const iterator to the beginning of the flat_multiset.
    //*********************************************************************
    const_iterator cbegin() const
    {
      return refset_t::cbegin();
    }

    //*********************************************************************
    /// Returns a const_iterator to the end of the flat_multiset.
    ///\return A const iterator to the end of the flat_multiset.
    //*********************************************************************
    const_iterator cend() const
    {
      return refset_t::cend();
    }

    //*********************************************************************
    /// Returns an reverse iterator to the reverse beginning of the flat_multiset.
    ///\return Iterator to the reverse beginning of the flat_multiset.
    //*********************************************************************
    reverse_iterator rbegin()
    {
      return refset_t::rbegin();
    }

    //*********************************************************************
    /// Returns a const reverse iterator to the reverse beginning of the flat_multiset.
    ///\return Const iterator to the reverse beginning of the flat_multiset.
    //*********************************************************************
    const_reverse_iterator rbegin() const
    {
      return refset_t::rbegin();
    }

    //*********************************************************************
    /// Returns a reverse iterator to the end + 1 of the flat_multiset.
    ///\return Reverse iterator to the end + 1 of the flat_multiset.
    //*********************************************************************
    reverse_iterator rend()
    {
      return refset_t::rend();
    }

    //*********************************************************************
    /// Returns a const reverse iterator to the end + 1 of the flat_multiset.
    ///\return Const reverse iterator to the end + 1 of the flat_multiset.
    //*********************************************************************
    const_reverse_iterator rend() const
    {
      return refset_t::rend();
    }

    //*********************************************************************
    /// Returns a const reverse iterator to the reverse beginning of the flat_multiset.
    ///\return Const reverse iterator to the reverse beginning of the flat_multiset.
    //*********************************************************************
    const_reverse_iterator crbegin() const
    {
      return refset_t::crbegin();
    }

    //*********************************************************************
    /// Returns a const reverse iterator to the end + 1 of the flat_multiset.
    ///\return Const reverse iterator to the end + 1 of the flat_multiset.
    //*********************************************************************
    const_reverse_iterator crend() const
    {
      return refset_t::crend();
    }

    //*********************************************************************
    /// Assigns values to the flat_multiset.
    /// If asserts or exceptions are enabled, emits flat_multiset_full if the flat_multiset does not have enough free space.
    /// If asserts or exceptions are enabled, emits flat_multiset_iterator if the iterators are reversed.
    ///\param first The iterator to the first element.
    ///\param last  The iterator to the last element + 1.
    //*********************************************************************
    template <typename TIterator>
    void assign(TIterator first, TIterator last)
    {
#if TYPHOON_IS_DEBUG_BUILD
      difference_type d = tpn::distance(first, last);
      TYPHOON_ASSERT(d <= difference_type(capacity()), TYPHOON_ERROR(flat_multiset_full));
#endif

      clear();

      while (first != last)
      {
        insert(*first);
        ++first;
      }
    }

    //*********************************************************************
    /// Inserts a value to the flat_multiset.
    /// If asserts or exceptions are enabled, emits flat_multiset_full if the flat_multiset is already full.
    ///\param value    The value to insert.
    //*********************************************************************
    TYPHOON_OR_STD::pair<iterator, bool> insert(const_reference value)
    {
      TYPHOON_OR_STD::pair<iterator, bool> result(end(), false);

      TYPHOON_ASSERT(!full(), TYPHOON_ERROR(flat_multiset_full));

      iterator i_element = tpn::upper_bound(begin(), end(), value, compare);

      value_type* pvalue = storage.allocate<value_type>();
      ::new (pvalue) value_type(value);
      TYPHOON_INCREMENT_DEBUG_COUNT
        result = refset_t::insert_at(i_element, *pvalue);

      return result;
    }

#if TYPHOON_USING_CPP11
    //*********************************************************************
    /// Inserts a value to the flat_multiset.
    /// If asserts or exceptions are enabled, emits flat_multiset_full if the flat_multiset is already full.
    ///\param value    The value to insert.
    //*********************************************************************
    TYPHOON_OR_STD::pair<iterator, bool> insert(rvalue_reference value)
    {
      TYPHOON_OR_STD::pair<iterator, bool> result(end(), false);

      TYPHOON_ASSERT(!full(), TYPHOON_ERROR(flat_multiset_full));

      iterator i_element = tpn::upper_bound(begin(), end(), value, compare);

      value_type* pvalue = storage.allocate<value_type>();
      ::new (pvalue) value_type(tpn::move(value));
      TYPHOON_INCREMENT_DEBUG_COUNT
        result = refset_t::insert_at(i_element, *pvalue);

      return result;
    }
#endif

    //*********************************************************************
    /// Inserts a value to the flat_multiset.
    /// If asserts or exceptions are enabled, emits flat_multiset_full if the flat_multiset is already full.
    ///\param position The position to insert at.
    ///\param value    The value to insert.
    //*********************************************************************
    iterator insert(const_iterator /*position*/, const_reference value)
    {
      return insert(value).first;
    }

#if TYPHOON_USING_CPP11
    //*********************************************************************
    /// Inserts a value to the flat_multiset.
    /// If asserts or exceptions are enabled, emits flat_multiset_full if the flat_multiset is already full.
    ///\param position The position to insert at.
    ///\param value    The value to insert.
    //*********************************************************************
    iterator insert(const_iterator /*position*/, rvalue_reference value)
    {
      return insert(tpn::move(value)).first;
    }
#endif

    //*********************************************************************
    /// Inserts a range of values to the flat_multiset.
    /// If asserts or exceptions are enabled, emits flat_multiset_full if the flat_multiset does not have enough free space.
    ///\param position The position to insert at.
    ///\param first    The first element to add.
    ///\param last     The last + 1 element to add.
    //*********************************************************************
    template <class TIterator>
    void insert(TIterator first, TIterator last)
    {
      while (first != last)
      {
        insert(*first);
        ++first;
      }
    }

    //*************************************************************************
    /// Emplaces a value to the set.
    //*************************************************************************
    template <typename T1>
    TYPHOON_OR_STD::pair<iterator, bool> emplace(const_reference value)
    {
      return insert(value);
    }

    //*************************************************************************
    /// Emplaces a value to the set.
    //*************************************************************************
#if TYPHOON_USING_CPP11 && TYPHOON_NOT_USING_STLPORT
    template <typename ... Args>
    TYPHOON_OR_STD::pair<iterator, bool> emplace(Args && ... args)
    {
      TYPHOON_ASSERT(!full(), TYPHOON_ERROR(flat_multiset_full));

      // Create it.
      value_type* pvalue = storage.allocate<value_type>();
      ::new (pvalue) value_type(tpn::forward<Args>(args)...);

      iterator i_element = upper_bound(*pvalue);

      TYPHOON_INCREMENT_DEBUG_COUNT
        return TYPHOON_OR_STD::pair<iterator, bool>(refset_t::insert_at(i_element, *pvalue));
    }
#else
    //*************************************************************************
    /// Emplaces a value to the set.
    //*************************************************************************
    template <typename T1>
    TYPHOON_OR_STD::pair<iterator, bool> emplace(const T1& value1)
    {
      TYPHOON_ASSERT(!full(), TYPHOON_ERROR(flat_multiset_full));

      // Create it.
      value_type* pvalue = storage.allocate<value_type>();
      ::new (pvalue) value_type(value1);

      iterator i_element = upper_bound(*pvalue);

      TYPHOON_INCREMENT_DEBUG_COUNT
        return TYPHOON_OR_STD::pair<iterator, bool>(refset_t::insert_at(i_element, *pvalue));
    }

    //*************************************************************************
    /// Emplaces a value to the set.
    //*************************************************************************
    template <typename T1, typename T2>
    TYPHOON_OR_STD::pair<iterator, bool> emplace(const T1& value1, const T2& value2)
    {
      TYPHOON_ASSERT(!full(), TYPHOON_ERROR(flat_multiset_full));

      // Create it.
      value_type* pvalue = storage.allocate<value_type>();
      ::new (pvalue) value_type(value1, value2);

      iterator i_element = upper_bound(*pvalue);

      TYPHOON_INCREMENT_DEBUG_COUNT
        return TYPHOON_OR_STD::pair<iterator, bool>(refset_t::insert_at(i_element, *pvalue));
    }

    //*************************************************************************
    /// Emplaces a value to the set.
    //*************************************************************************
    template <typename T1, typename T2, typename T3>
    TYPHOON_OR_STD::pair<iterator, bool> emplace(const T1& value1, const T2& value2, const T3& value3)
    {
      TYPHOON_ASSERT(!full(), TYPHOON_ERROR(flat_multiset_full));

      // Create it.
      value_type* pvalue = storage.allocate<value_type>();
      ::new (pvalue) value_type(value1, value2, value3);

      iterator i_element = upper_bound(*pvalue);

      TYPHOON_INCREMENT_DEBUG_COUNT
        return TYPHOON_OR_STD::pair<iterator, bool>(refset_t::insert_at(i_element, *pvalue));
    }

    //*************************************************************************
    /// Emplaces a value to the set.
    //*************************************************************************
    template <typename T1, typename T2, typename T3, typename T4>
    TYPHOON_OR_STD::pair<iterator, bool> emplace(const T1& value1, const T2& value2, const T3& value3, const T4& value4)
    {
      TYPHOON_ASSERT(!full(), TYPHOON_ERROR(flat_multiset_full));

      // Create it.
      value_type* pvalue = storage.allocate<value_type>();
      ::new (pvalue) value_type(value1, value2, value3, value4);

      iterator i_element = upper_bound(*pvalue);

      TYPHOON_INCREMENT_DEBUG_COUNT
        return TYPHOON_OR_STD::pair<iterator, bool>(refset_t::insert_at(i_element, *pvalue));
    }
#endif

    //*********************************************************************
    /// Erases an element.
    ///\param key The key to erase.
    ///\return The number of elements erased. 0 or 1.
    //*********************************************************************
    size_t erase(key_parameter_t key)
    {
      TYPHOON_OR_STD::pair<iterator, iterator> range = equal_range(key);

      if (range.first == end())
      {
        return 0;
      }
      else
      {
        size_t d = tpn::distance(range.first, range.second);
        erase(range.first, range.second);
        return d;
      }
    }

#if TYPHOON_USING_CPP11
    //*********************************************************************
    template <typename K, typename KC = TKeyCompare, tpn::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    size_t erase(K&& key)
    {
      TYPHOON_OR_STD::pair<iterator, iterator> range = equal_range(tpn::forward<K>(key));

      if (range.first == end())
      {
        return 0;
      }
      else
      {
        size_t d = tpn::distance(range.first, range.second);
        erase(range.first, range.second);
        return d;
      }
    }
#endif

    //*********************************************************************
    /// Erases an element.
    ///\param i_element Iterator to the element.
    //*********************************************************************
    iterator erase(iterator i_element)
    {
      tpn::destroy_at(tpn::addressof(*i_element));
      storage.release(tpn::addressof(*i_element));
      TYPHOON_DECREMENT_DEBUG_COUNT
      return refset_t::erase(i_element);
    }

    //*********************************************************************
    /// Erases an element.
    ///\param i_element Iterator to the element.
    //*********************************************************************
    iterator erase(const_iterator i_element)
    {
      tpn::destroy_at(tpn::addressof(*i_element));
      storage.release(tpn::addressof(*i_element));
      TYPHOON_DECREMENT_DEBUG_COUNT
      return refset_t::erase(i_element);
    }

    //*********************************************************************
    /// Erases a range of elements.
    /// The range includes all the elements between first and last, including the
    /// element pointed by first, but not the one pointed by last.
    ///\param first Iterator to the first element.
    ///\param last  Iterator to the last element.
    //*********************************************************************
    iterator erase(const_iterator first, const_iterator last)
    {
      const_iterator itr = first;

      while (itr != last)
      {
        tpn::destroy_at(tpn::addressof(*itr));
        storage.release(tpn::addressof(*itr));
        ++itr;
        TYPHOON_DECREMENT_DEBUG_COUNT
      }

      return refset_t::erase(first, last);
    }

    //*************************************************************************
    /// Clears the flat_multiset.
    //*************************************************************************
    void clear()
    {
      if TYPHOON_IF_CONSTEXPR(tpn::is_trivially_destructible<value_type>::value)
      {
        storage.release_all();
      }
      else
      {
        iterator itr = begin();

        while (itr != end())
        {
          tpn::destroy_at(tpn::addressof(*itr));
          storage.release(tpn::addressof(*itr));
          ++itr;
        }
      }

      TYPHOON_RESET_DEBUG_COUNT
        refset_t::clear();
    }

    //*********************************************************************
    /// Finds an element.
    ///\param key The key to search for.
    ///\return An iterator pointing to the element or end() if not found.
    //*********************************************************************
    iterator find(key_parameter_t key)
    {
      return refset_t::find(key);
    }

#if TYPHOON_USING_CPP11
    //*********************************************************************
    template <typename K, typename KC = TKeyCompare, tpn::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    iterator find(const K& key)
    {
      return refset_t::find(key);
    }
#endif

    //*********************************************************************
    /// Finds an element.
    ///\param key The key to search for.
    ///\return An iterator pointing to the element or end() if not found.
    //*********************************************************************
    const_iterator find(key_parameter_t key) const
    {
      return refset_t::find(key);
    }

#if TYPHOON_USING_CPP11
    //*********************************************************************
    template <typename K, typename KC = TKeyCompare, tpn::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    const_iterator find(const K& key) const
    {
      return refset_t::find(key);
    }
#endif

    //*********************************************************************
    /// Counts an element.
    ///\param key The key to search for.
    ///\return 1 if the key exists, otherwise 0.
    //*********************************************************************
    size_t count(key_parameter_t key) const
    {
      return refset_t::count(key);
    }

#if TYPHOON_USING_CPP11
    //*********************************************************************
    template <typename K, typename KC = TKeyCompare, tpn::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    size_t count(const K& key) const
    {
      return refset_t::count(key);
    }
#endif

    //*********************************************************************
    /// Finds the lower bound of a key
    ///\param key The key to search for.
    ///\return An iterator.
    //*********************************************************************
    iterator lower_bound(key_parameter_t key)
    {
      return refset_t::lower_bound(key);
    }

#if TYPHOON_USING_CPP11
    //*********************************************************************
    template <typename K, typename KC = TKeyCompare, tpn::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    iterator lower_bound(const K& key)
    {
      return refset_t::lower_bound(key);
    }
#endif

    //*********************************************************************
    /// Finds the lower bound of a key
    ///\param key The key to search for.
    ///\return An iterator.
    //*********************************************************************
    const_iterator lower_bound(key_parameter_t key) const
    {
      return refset_t::lower_bound(key);
    }

#if TYPHOON_USING_CPP11
    //*********************************************************************
    template <typename K, typename KC = TKeyCompare, tpn::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    const_iterator lower_bound(const K& key) const
    {
      return refset_t::lower_bound(key);
    }
#endif

    //*********************************************************************
    /// Finds the upper bound of a key
    ///\param key The key to search for.
    ///\return An iterator.
    //*********************************************************************
    iterator upper_bound(key_parameter_t key)
    {
      return refset_t::upper_bound(key);
    }

#if TYPHOON_USING_CPP11
    //*********************************************************************
    template <typename K, typename KC = TKeyCompare, tpn::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    iterator upper_bound(const K& key)
    {
      return refset_t::upper_bound(key);
    }
#endif

    //*********************************************************************
    /// Finds the upper bound of a key
    ///\param key The key to search for.
    ///\return An iterator.
    //*********************************************************************
    const_iterator upper_bound(key_parameter_t key) const
    {
      return refset_t::upper_bound(key);
    }

#if TYPHOON_USING_CPP11
    //*********************************************************************
    template <typename K, typename KC = TKeyCompare, tpn::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    const_iterator upper_bound(const K& key) const
    {
      return refset_t::upper_bound(key);
    }
#endif

    //*********************************************************************
    /// Finds the range of equal elements of a key
    ///\param key The key to search for.
    ///\return An iterator pair.
    //*********************************************************************
    TYPHOON_OR_STD::pair<iterator, iterator> equal_range(key_parameter_t key)
    {
      return refset_t::equal_range(key);
    }

#if TYPHOON_USING_CPP11
    //*********************************************************************
    template <typename K, typename KC = TKeyCompare, tpn::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    TYPHOON_OR_STD::pair<iterator, iterator> equal_range(const K& key)
    {
      return refset_t::equal_range(key);
    }
#endif

    //*********************************************************************
    /// Finds the range of equal elements of a key
    ///\param key The key to search for.
    ///\return An iterator pair.
    //*********************************************************************
    TYPHOON_OR_STD::pair<const_iterator, const_iterator> equal_range(key_parameter_t key) const
    {
      return refset_t::equal_range(key);
    }

#if TYPHOON_USING_CPP11
    //*********************************************************************
    template <typename K, typename KC = TKeyCompare, tpn::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    TYPHOON_OR_STD::pair<const_iterator, const_iterator> equal_range(const K& key) const
    {
      return refset_t::equal_range(key);
    }
#endif

    //*************************************************************************
    /// Check if the map contains the key.
    //*************************************************************************
    bool contains(key_parameter_t key) const
    {
      return find(key) != end();
    }

#if TYPHOON_USING_CPP11
    //*************************************************************************
    template <typename K, typename KC = TKeyCompare, tpn::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    bool contains(const K& k) const
    {
      return find(k) != end();
    }
#endif

    //*************************************************************************
    /// Assignment operator.
    //*************************************************************************
    iflat_multiset& operator = (const iflat_multiset& rhs)
    {
      if (&rhs != this)
      {
        assign(rhs.cbegin(), rhs.cend());
      }

      return *this;
    }

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Move assignment operator.
    //*************************************************************************
    iflat_multiset& operator = (iflat_multiset&& rhs)
    {
      move_container(tpn::move(rhs));

      return *this;
    }
#endif

    //*************************************************************************
    /// Gets the current size of the flat_multiset.
    ///\return The current size of the flat_multiset.
    //*************************************************************************
    size_type size() const
    {
      return refset_t::size();
    }

    //*************************************************************************
    /// Checks the 'empty' state of the flat_multiset.
    ///\return <b>true</b> if empty.
    //*************************************************************************
    bool empty() const
    {
      return refset_t::empty();
    }

    //*************************************************************************
    /// Checks the 'full' state of the flat_multiset.
    ///\return <b>true</b> if full.
    //*************************************************************************
    bool full() const
    {
      return refset_t::full();
    }

    //*************************************************************************
    /// Returns the capacity of the flat_multiset.
    ///\return The capacity of the flat_multiset.
    //*************************************************************************
    size_type capacity() const
    {
      return refset_t::capacity();
    }

    //*************************************************************************
    /// Returns the maximum possible size of the flat_multiset.
    ///\return The maximum size of the flat_multiset.
    //*************************************************************************
    size_type max_size() const
    {
      return refset_t::max_size();
    }

    //*************************************************************************
    /// Returns the remaining capacity.
    ///\return The remaining capacity.
    //*************************************************************************
    size_t available() const
    {
      return refset_t::available();
    }

  protected:

    //*********************************************************************
    /// Constructor.
    //*********************************************************************
    iflat_multiset(lookup_t& lookup_, storage_t& storage_)
      : refset_t(lookup_),
      storage(storage_)
    {
    }

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Move a flat_multimap.
    /// Assumes the flat_multimap is initialised and empty.
    //*************************************************************************
    void move_container(iflat_multiset&& rhs)
    {
      if (&rhs != this)
      {
        this->clear();

        tpn::iflat_multiset<T, TKeyCompare>::iterator first = rhs.begin();
        tpn::iflat_multiset<T, TKeyCompare>::iterator last = rhs.end();

        // Move all of the elements.
        while (first != last)
        {
          typename tpn::iflat_multiset<T, TKeyCompare>::iterator temp = first;
          ++temp;

          this->insert(tpn::move(*first));
          first = temp;
        }
      }
    }
#endif

  private:

    // Disable copy construction.
    iflat_multiset(const iflat_multiset&);

    storage_t& storage;

    TKeyCompare compare;

    /// Internal debugging.
    TYPHOON_DECLARE_DEBUG_COUNT

      //*************************************************************************
      /// Destructor.
      //*************************************************************************
#if defined(TYPHOON_POLYMORPHIC_FLAT_MULTISET) || defined(TYPHOON_POLYMORPHIC_CONTAINERS)
  public:
    virtual ~iflat_multiset()
    {
    }
#else
  protected:
    ~iflat_multiset()
    {
    }
#endif
  };

  //***************************************************************************
  /// Equal operator.
  ///\param lhs Reference to the first flat_multiset.
  ///\param rhs Reference to the second flat_multiset.
  ///\return <b>true</b> if the arrays are equal, otherwise <b>false</b>
  ///\ingroup flat_multiset
  //***************************************************************************
  template <typename T, typename TKeyCompare>
  bool operator ==(const tpn::iflat_multiset<T, TKeyCompare>& lhs, const tpn::iflat_multiset<T, TKeyCompare>& rhs)
  {
    return (lhs.size() == rhs.size()) && tpn::equal(lhs.begin(), lhs.end(), rhs.begin());
  }

  //***************************************************************************
  /// Not equal operator.
  ///\param lhs Reference to the first flat_multiset.
  ///\param rhs Reference to the second flat_multiset.
  ///\return <b>true</b> if the arrays are not equal, otherwise <b>false</b>
  ///\ingroup flat_multiset
  //***************************************************************************
  template <typename T, typename TKeyCompare>
  bool operator !=(const tpn::iflat_multiset<T, TKeyCompare>& lhs, const tpn::iflat_multiset<T, TKeyCompare>& rhs)
  {
    return !(lhs == rhs);
  }

  //***************************************************************************
  /// A flat_multiset implementation that uses a fixed size buffer.
  ///\tparam T        The value type.
  ///\tparam TCompare The type to compare keys. Default = tpn::less<T>
  ///\tparam MAX_SIZE_ The maximum number of elements that can be stored.
  ///\ingroup flat_multiset
  //***************************************************************************
  template <typename T, const size_t MAX_SIZE_, typename TCompare = tpn::less<T> >
  class flat_multiset : public tpn::iflat_multiset<T, TCompare>
  {
  public:

    static TYPHOON_CONSTANT size_t MAX_SIZE = MAX_SIZE_;

    //*************************************************************************
    /// Constructor.
    //*************************************************************************
    flat_multiset()
      : tpn::iflat_multiset<T, TCompare>(lookup, storage)
    {
    }

    //*************************************************************************
    /// Copy constructor.
    //*************************************************************************
    flat_multiset(const flat_multiset& other)
      : iflat_multiset<T, TCompare>(lookup, storage)
    {
      this->assign(other.cbegin(), other.cend());
    }

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Move constructor.
    //*************************************************************************
    flat_multiset(flat_multiset&& other)
      : tpn::iflat_multiset<T, TCompare>(lookup, storage)
    {
      if (&other != this)
      {
        this->move_container(tpn::move(other));
      }
    }
#endif

    //*************************************************************************
    /// Constructor, from an iterator range.
    ///\tparam TIterator The iterator type.
    ///\param first The iterator to the first element.
    ///\param last  The iterator to the last element + 1.
    //*************************************************************************
    template <typename TIterator>
    flat_multiset(TIterator first, TIterator last)
      : iflat_multiset<T, TCompare>(lookup, storage)
    {
      this->assign(first, last);
    }

#if TYPHOON_HAS_INITIALIZER_LIST
    //*************************************************************************
    /// Construct from initializer_list.
    //*************************************************************************
    flat_multiset(std::initializer_list<T> init)
      : iflat_multiset<T, TCompare>(lookup, storage)
    {
      this->assign(init.begin(), init.end());
    }
#endif

    //*************************************************************************
    /// Destructor.
    //*************************************************************************
    ~flat_multiset()
    {
      this->clear();
    }

    //*************************************************************************
    /// Assignment operator.
    //*************************************************************************
    flat_multiset& operator = (const flat_multiset& rhs)
    {
      if (&rhs != this)
      {
        this->assign(rhs.cbegin(), rhs.cend());
      }

      return *this;
    }

#if TYPHOON_USING_CPP11
    //*************************************************************************
    /// Move assignment operator.
    //*************************************************************************
    flat_multiset& operator = (flat_multiset&& rhs)
    {
      if (&rhs != this)
      {
        this->move_container(tpn::move(rhs));
      }

      return *this;
    }
#endif

  private:

    typedef typename tpn::iflat_multiset<T, TCompare>::value_type node_t;

    // The pool of nodes.
    tpn::pool<node_t, MAX_SIZE> storage;

    // The vector that stores pointers to the nodes.
    tpn::vector<node_t*, MAX_SIZE> lookup;
  };

  //*************************************************************************
  /// Template deduction guides.
  //*************************************************************************
#if TYPHOON_USING_CPP17 && TYPHOON_HAS_INITIALIZER_LIST
  template <typename... T>
  flat_multiset(T...) -> flat_multiset<tpn::nth_type_t<0, T...>, sizeof...(T)>;
#endif

  //*************************************************************************
  /// Make
  //*************************************************************************
#if TYPHOON_USING_CPP11 && TYPHOON_HAS_INITIALIZER_LIST
  template <typename TKey, typename TKeyCompare = tpn::less<TKey>, typename... T>
  constexpr auto make_flat_multiset(T&&... keys) -> tpn::flat_multiset<TKey, sizeof...(T), TKeyCompare>
  {
    return { {tpn::forward<T>(keys)...} };
  }
#endif
}

#endif
