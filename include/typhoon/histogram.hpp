///\file

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

#ifndef TYPHOON_HISTOGRAM_HPP
#define TYPHOON_HISTOGRAM_HPP

#include "platform.hpp"
#include "functional.hpp"
#include "algorithm.hpp"
#include "array.hpp"
#include "flat_map.hpp"
#include "static_assert.hpp"
#include "type_traits.hpp"
#include "integral_limits.hpp"

namespace tpn
{
  namespace private_histogram
  {
    //***************************************************************************
    /// Base for histograms.
    //***************************************************************************
    template <typename TCount, size_t Max_Size_>
    class histogram_common
    {
    public:

      TYPHOON_STATIC_ASSERT(tpn::is_integral<TCount>::value, "Only integral count allowed"); 

      static TYPHOON_CONSTANT size_t Max_Size = Max_Size_;

      typedef typename tpn::array<TCount, Max_Size>::const_iterator const_iterator;

      //*********************************
      /// Beginning of the histogram.
      //*********************************
      const_iterator begin() const
      {
        return accumulator.begin();
      }

      //*********************************
      /// Beginning of the histogram.
      //*********************************
      const_iterator cbegin() const
      {
        return accumulator.cbegin();
      }

      //*********************************
      /// End of the histogram.
      //*********************************
      const_iterator end() const
      {
        return accumulator.end();
      }

      //*********************************
      /// End of the histogram.
      //*********************************
      const_iterator cend() const
      {
        return accumulator.cend();
      }

      //*********************************
      /// Clear the histogram.
      //*********************************
      void clear()
      {
        accumulator.fill(TCount(0));
      }

      //*********************************
      /// Size of the histogram.
      //*********************************
      TYPHOON_CONSTEXPR size_t size() const
      {
        return Max_Size;
      }

      //*********************************
      /// Max size of the histogram.
      //*********************************
      TYPHOON_CONSTEXPR size_t max_size() const
      {
        return Max_Size;
      }

      //*********************************
      /// Count of items in the histogram.
      //*********************************
      size_t count() const
      {
        return tpn::accumulate(accumulator.begin(), accumulator.end(), size_t(0));
      }

    protected:

      tpn::array<TCount, Max_Size> accumulator;
    };
  }

  //***************************************************************************
  /// Histogram with a compile time start index.
  //***************************************************************************
  template <typename TKey, typename TCount, size_t Max_Size, int32_t Start_Index = tpn::integral_limits<int32_t>::max>
  class histogram 
    : public tpn::private_histogram::histogram_common<TCount, Max_Size>
    , public tpn::unary_function<TKey, void>
  {
  public:

    TYPHOON_STATIC_ASSERT(tpn::is_integral<TKey>::value, "Only integral keys allowed");
    TYPHOON_STATIC_ASSERT(tpn::is_integral<TCount>::value, "Only integral count allowed");   

    typedef TKey   key_type;
    typedef TCount count_type;
    typedef TCount value_type;

    //*********************************
    /// Constructor
    //*********************************
    histogram()
    {
      this->accumulator.fill(count_type(0));
    }

    //*********************************
    /// Constructor
    //*********************************
    template <typename TIterator>
    histogram(TIterator first, TIterator last)
    {
      this->accumulator.fill(count_type(0));
      add(first, last);
    }

    //*********************************
    /// Copy constructor
    //*********************************
    histogram(const histogram& other)
    {
      this->accumulator = other.accumulator;
    }

#if TYPHOON_USING_CPP11
    //*********************************
    /// Move constructor
    //*********************************
    histogram(histogram&& other)
    {
      this->accumulator = tpn::move(other.accumulator);
    }
#endif

    //*********************************
    /// Copy assignment
    //*********************************
    histogram& operator =(const histogram& rhs)
    {
      this->accumulator = rhs.accumulator;

      return *this;
    }

#if TYPHOON_USING_CPP11
    //*********************************
    /// Move assignment
    //*********************************
    histogram& operator =(histogram&& rhs)
    {
      this->accumulator = tpn::move(rhs.accumulator);

      return *this;
    }
#endif

    //*********************************
    /// Add
    //*********************************
    void add(key_type key)
    {
      ++this->accumulator[key - Start_Index];
    }

    //*********************************
    /// Add
    //*********************************
    template <typename TIterator>
    void add(TIterator first, TIterator last)
    {
      while (first != last)
      {
        add(*first);
        ++first;
      }
    }

    //*********************************
    /// operator ()
    //*********************************
    void operator ()(key_type key)
    {
      add(key);
    }

    //*********************************
    /// operator ()
    //*********************************
    template <typename TIterator>
    void operator ()(TIterator first, TIterator last)
    {
      add(first, last);
    }

    //*********************************
    /// operator []
    //*********************************
    value_type operator [](key_type key) const
    {
      return this->accumulator[key];
    }
  };

  //***************************************************************************
  /// Histogram with a run time start index.
  //***************************************************************************
  template<typename TKey, typename TCount, size_t Max_Size>
  class histogram<TKey, TCount, Max_Size, tpn::integral_limits<int32_t>::max>
    : public tpn::private_histogram::histogram_common<TCount, Max_Size>
    , public tpn::unary_function<TKey, void>
  {
  public:

    TYPHOON_STATIC_ASSERT(tpn::is_integral<TKey>::value, "Only integral keys allowed");
    TYPHOON_STATIC_ASSERT(tpn::is_integral<TCount>::value, "Only integral count allowed");

    typedef TKey   key_type;
    typedef TCount count_type;
    typedef TCount value_type;

    //*********************************
    /// Constructor
    //*********************************
    explicit histogram(key_type start_index_)
      : start_index(start_index_)
    {
      this->accumulator.fill(count_type(0));
    }

    //*********************************
    /// Constructor
    //*********************************
    template <typename TIterator>
    histogram(key_type start_index_, TIterator first, TIterator last)
      : start_index(start_index_)
    {
      this->accumulator.fill(count_type(0));
      add(first, last);
    }

    //*********************************
    /// Copy constructor
    //*********************************
    histogram(const histogram& other)
    {
      this->accumulator = other.accumulator;
    }

#if TYPHOON_USING_CPP11
    //*********************************
    /// Move constructor
    //*********************************
    histogram(histogram&& other)
    {
      this->accumulator = tpn::move(other.accumulator);
    }
#endif

    //*********************************
    /// Copy assignment
    //*********************************
    histogram& operator =(const histogram& rhs)
    {
      this->accumulator = rhs.accumulator;

      return *this;
    }

#if TYPHOON_USING_CPP11
    //*********************************
    /// Move assignment
    //*********************************
    histogram& operator =(histogram&& rhs)
    {
      this->accumulator = tpn::move(rhs.accumulator);

      return *this;
    }
#endif

    //*********************************
    /// Add
    //*********************************
    void add(key_type key)
    {
      ++this->accumulator[key - start_index];
    }

    //*********************************
    /// Add
    //*********************************
    template <typename TIterator>
    void add(TIterator first, TIterator last)
    {
      while (first != last)
      {
        add(*first);
        ++first;
      }
    }

    //*********************************
    /// operator ()
    //*********************************
    void operator ()(key_type key)
    {
      add(key);
    }

    //*********************************
    /// operator ()
    //*********************************
    template <typename TIterator>
    void operator ()(TIterator first, TIterator last)
    {
      add(first, last);
    }

    //*********************************
    /// operator []
    //*********************************
    value_type operator [](key_type key) const
    {
      return this->accumulator[key];
    }

  private:

    key_type start_index;
  };

  //***************************************************************************
  /// Histogram for sparse keys.
  //***************************************************************************
  template<typename TKey, typename TCount, size_t Max_Size_>
  class sparse_histogram : public tpn::unary_function<TKey, void>
  {
  private:

    typedef tpn::flat_map<TKey, TCount, Max_Size_> accumulator_type;

  public:

    TYPHOON_STATIC_ASSERT(tpn::is_integral<TCount>::value, "Only integral count allowed");

    static TYPHOON_CONSTANT size_t Max_Size = Max_Size_;

    typedef TKey   key_type;
    typedef TCount count_type;
    typedef typename accumulator_type::value_type     value_type;
    typedef typename accumulator_type::const_iterator const_iterator;

  public:

    //*********************************
    /// Constructor
    //*********************************
    sparse_histogram()
    {
    }

    //*********************************
    /// Constructor
    //*********************************
    template <typename TIterator>
    sparse_histogram(TIterator first, TIterator last)
    {
      add(first, last);
    }

    //*********************************
    /// Copy constructor
    //*********************************
    sparse_histogram(const sparse_histogram& other)
    {
      this->accumulator = other.accumulator;
    }

#if TYPHOON_USING_CPP11
    //*********************************
    /// Move constructor
    //*********************************
    sparse_histogram(sparse_histogram&& other)
    {
      accumulator = tpn::move(other.accumulator);
    }
#endif

    //*********************************
    /// Copy assignment
    //*********************************
    sparse_histogram& operator =(const sparse_histogram& rhs)
    {
      accumulator = rhs.accumulator;
      
      return *this;
    }

#if TYPHOON_USING_CPP11
    //*********************************
    /// Move assignment
    //*********************************
    sparse_histogram& operator =(sparse_histogram&& rhs)
    {
      accumulator = tpn::move(rhs.accumulator);

      return *this;
    }
#endif

    //*********************************
    /// Beginning of the histogram.
    //*********************************
    const_iterator begin() const
    {
      return accumulator.begin();
    }

    //*********************************
    /// Beginning of the histogram.
    //*********************************
    const_iterator cbegin() const
    {
      return accumulator.cbegin();
    }

    //*********************************
    /// End of the histogram.
    //*********************************
    const_iterator end() const
    {
      return accumulator.begin();
    }

    //*********************************
    /// End of the histogram.
    //*********************************
    const_iterator cend() const
    {
      return accumulator.cbegin();
    }

    //*********************************
    /// Add
    //*********************************
    void add(const key_type& key)
    {
      ++accumulator[key];
    }

    //*********************************
    /// Add
    //*********************************
    template <typename TIterator>
    void add(TIterator first, TIterator last)
    {
      while (first != last)
      {
        add(*first);
        ++first;
      }
    }

    //*********************************
    /// operator ()
    //*********************************
    void operator ()(const key_type& key)
    {
      add(key);
    }

    //*********************************
    /// operator ()
    //*********************************
    template <typename TIterator>
    void operator ()(TIterator first, TIterator last)
    {
      add(first, last);
    }

    //*********************************
    /// operator []
    //*********************************
    const value_type& operator [](const key_type& key) const
    {
      static const value_type unused(key_type(), count_type(0));

      typename accumulator_type::const_iterator itr = accumulator.find(key);

      if (itr != accumulator.end())
      {
        return *itr;
      }
      else
      {
        return unused;
      }
    }

    //*********************************
    /// Clear the histogram.
    //*********************************
    void clear()
    {
      accumulator.clear();
    }

    //*********************************
    /// Size of the histogram.
    //*********************************
    size_t size() const
    {
      return accumulator.size();
    }

    //*********************************
    /// Max size of the histogram.
    //*********************************
    TYPHOON_CONSTEXPR size_t max_size() const
    {
      return Max_Size;
    }

    //*********************************
    /// Count of items in the histogram.
    //*********************************
    size_t count() const
    {
      count_type sum = count_type(0);

      const_iterator itr = accumulator.begin();

      while (itr != accumulator.end())
      {
        sum += (*itr).second;
        ++itr;
      }

      return sum;
    }

  private:

    tpn::flat_map<key_type, count_type, Max_Size> accumulator;
  };
}

#endif
