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

#ifndef TYPHOON_MULTI_SPAN_HPP
#define TYPHOON_MULTI_SPAN_HPP

#include "platform.hpp"
#include "iterator.hpp"
#include "algorithm.hpp"
#include "vector.hpp"
#include "span.hpp"

///\defgroup multi_multi_span multi_span multi_span
/// Scatter/Gather functionality
///\ingroup containers

namespace tpn
{
  template <typename T>
  class multi_span
  {
  public:
    
    typedef T                                 element_type;
    typedef typename tpn::remove_cv<T>::type  value_type;
    typedef size_t                            size_type;
    typedef T&                                reference;
    typedef const T&                          const_reference;
    typedef T*                                pointer;
    typedef const T*                          const_pointer;

    typedef tpn::span<T>                      span_type;
    typedef tpn::span<const span_type>        span_list_type;

    //*************************************************************************
    /// Iterator
    //*************************************************************************
    class iterator : public tpn::iterator<TYPHOON_OR_STD::forward_iterator_tag, element_type>
    {
    public:

      friend class multi_span;

      iterator()
        : p_current(TYPHOON_NULLPTR)
        , p_end(TYPHOON_NULLPTR)
        , p_value(TYPHOON_NULLPTR)
      {
      }

      //*****************************************
      iterator& operator ++()
      {
        if (p_current != p_end)
        {
          ++p_value;

          if (p_value == p_current->end())
          {
            do
            {
              ++p_current;
            } while ((p_current != p_end) && p_current->empty());

            if (p_current != p_end)
            {
              p_value = p_current->begin();
            }
            else
            {
              p_value = TYPHOON_NULLPTR;
            }
          }
        }

        return *this;
      }

      //*****************************************
      iterator operator ++(int)
      {
        iterator temp = *this;

        operator ++();

        return temp;
      }

      //*************************************************************************
      /// * operator
      //*************************************************************************
      reference operator *()
      {
        return *p_value;
      }

      //*************************************************************************
      /// * operator
      //*************************************************************************
      const_reference operator *() const
      {
        return *p_value;
      }

      //*************************************************************************
      /// -> operator
      //*************************************************************************
      pointer operator ->()
      {
        return *p_value;
      }

      //*************************************************************************
      /// -> operator
      //*************************************************************************
      const_pointer operator ->() const
      {
        return *p_value;
      }

      //*************************************************************************
      /// == operator
      //*************************************************************************
      friend bool operator ==(const iterator& lhs, const iterator& rhs)
      {
        return (lhs.p_current == rhs.p_current);
      }

      //*************************************************************************
      /// != operator
      //*************************************************************************
      friend bool operator !=(const iterator& lhs, const iterator& rhs)
      {
        return !(lhs == rhs);
      }

    private:

      typedef typename span_list_type::iterator span_list_iterator;

      //*****************************************
      iterator(span_list_iterator p_current_, span_list_iterator p_end_)
        : p_current(p_current_)
        , p_end(p_end_)
        , p_value(TYPHOON_NULLPTR)
      {
        if (p_current != p_end)
        {
          p_value = p_current->begin();
        }
      }

      typedef const span_type* span_list_pointer;

      span_list_pointer p_current;
      span_list_pointer p_end;
      pointer           p_value;
    };

    //*************************************************************************
    /// Constructor.
    //*************************************************************************
    TYPHOON_CONSTEXPR multi_span(span_list_type span_list_)
      : span_list(span_list_)
    {
    }

    //*************************************************************************
    /// Construct from a container or other type that supports
    /// data() and size() member functions.
    //*************************************************************************
    template <typename TContainer>
    TYPHOON_CONSTEXPR multi_span(TContainer& a) TYPHOON_NOEXCEPT
      : span_list(a.data(), a.data() + a.size())
    {
    }

    //*************************************************************************
    /// Construct from a container or other type that supports
    /// data() and size() member functions.
    //*************************************************************************
    template <typename TContainer>
    TYPHOON_CONSTEXPR multi_span(const TContainer& a) TYPHOON_NOEXCEPT
      : span_list(a.data(), a.data() + a.size())
    {
    }

    //*************************************************************************
    /// Constructor.
    //*************************************************************************
    template <typename TIterator>
    TYPHOON_CONSTEXPR multi_span(TIterator begin_, TIterator end_)
      : span_list(tpn::addressof(*begin_), tpn::distance(begin_, end_))
    {
    }

    //*************************************************************************
    /// Constructor.
    //*************************************************************************
    template <typename TIterator>
    TYPHOON_CONSTEXPR multi_span(TIterator begin_, size_t length_)
      : span_list(tpn::addressof(*begin_), length_)
    {
    }

    //*************************************************************************
    /// Copy Constructor.
    //*************************************************************************
    TYPHOON_CONSTEXPR multi_span(const multi_span& other)
      : span_list(other.span_list)
    {
    }

    //*************************************************************************
    /// Assignment operator
    //*************************************************************************
    TYPHOON_CONSTEXPR multi_span& operator = (const multi_span & other)
    {
      span_list = other.span_list;

      return *this;
    }

    //*************************************************************************
    /// 
    //*************************************************************************
    TYPHOON_CONSTEXPR iterator begin() const
    {
      return iterator(span_list.begin(), span_list.end());
    }

    //*************************************************************************
    /// 
    //*************************************************************************
    TYPHOON_CONSTEXPR iterator end() const
    {
      return iterator(span_list.end(), span_list.end());
    }

    //*************************************************************************
    /// Returns the number of elements in the multi_span.
    //*************************************************************************
    TYPHOON_CONSTEXPR14 size_t size() const TYPHOON_NOEXCEPT
    {
      size_t total_n_spans = 0U;

      for (typename span_list_type::iterator itr = span_list.begin();
           itr != span_list.end(); 
           ++itr)
      {
        total_n_spans += itr->size();
      }

      return total_n_spans;
    }

    //*************************************************************************
    /// Returns <b>true</b> if the multi_span size is zero.
    //*************************************************************************
    TYPHOON_CONSTEXPR bool empty() const TYPHOON_NOEXCEPT
    {
      if (span_list.empty())
      {
        return true;
      }
      else
      {
        return size() == 0U;
      }
    }

    //*************************************************************************
    /// Returns the size of the multi_span.
    //*************************************************************************
    TYPHOON_CONSTEXPR14 size_t size_bytes() const TYPHOON_NOEXCEPT
    {
      size_t total_n_spans_bytes = 0U;

      for (typename span_list_type::iterator itr = span_list.begin(); 
           itr != span_list.end(); 
           ++itr)
      {
        total_n_spans_bytes += itr->size_bytes();
      }

      return total_n_spans_bytes;
    }

    //*************************************************************************
    /// Returns the number of spans in the multi_span.
    //*************************************************************************
    TYPHOON_CONSTEXPR size_t size_spans() const TYPHOON_NOEXCEPT
    {
      return span_list.size();
    }

  private:

    span_list_type span_list;
  };
}

#endif

