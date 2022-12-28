
///\file

/******************************************************************************
The MIT License(MIT)
Embedded Template Library.
https://github.com/TYPHOONCPP/tphn
https://www.tphncpp.com
Copyright(c) 2014 John Wellbelove
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

#ifndef TYPHOON_FRAME_CHECK_SEQUENCE_HPP
#define TYPHOON_FRAME_CHECK_SEQUENCE_HPP

#include "platform.hpp"
#include "static_assert.hpp"
#include "type_traits.hpp"
#include "binary.hpp"
#include "iterator.hpp"

#include <stdint.h>

TYPHOON_STATIC_ASSERT(TYPHOON_USING_8BIT_TYPES, "This file does not currently support targets with no 8bit type");

///\defgroup frame_check_sequence Frame check sequence calculation
///\ingroup maths

namespace tphn
{
  namespace private_frame_check_sequence
  {
    //***************************************************
    /// add_insert_iterator
    /// An output iterator used to add new values.
    //***************************************************
    template <typename TFCS>
    class add_insert_iterator : public tphn::iterator<TYPHOON_OR_STD::output_iterator_tag, void, void, void, void>
    {
    public:

      //***********************************
      explicit add_insert_iterator(TFCS& fcs) TYPHOON_NOEXCEPT
        : p_fcs(&fcs)
      {
      }

      //***********************************
      add_insert_iterator& operator*() TYPHOON_NOEXCEPT
      {
        return *this;
      }

      //***********************************
      add_insert_iterator& operator++() TYPHOON_NOEXCEPT
      {
        return *this;
      }

      //***********************************
      add_insert_iterator& operator++(int) TYPHOON_NOEXCEPT
      {
        return *this;
      }

      //***********************************
      add_insert_iterator& operator =(uint8_t value)
      {
        p_fcs->add(value);
        return *this;
      }

    private:

      TFCS* p_fcs;
    };
  }

  //***************************************************************************
  /// Calculates a frame check sequence according to the specified policy.
  ///\tparam TPolicy The type used to enact the policy.
  ///\ingroup frame_check_sequence
  //***************************************************************************
  template <typename TPolicy>
  class frame_check_sequence
  {
  public:

    typedef TPolicy policy_type;
    typedef typename policy_type::value_type value_type;
    typedef private_frame_check_sequence::add_insert_iterator<frame_check_sequence<TPolicy> > add_insert_iterator;

    TYPHOON_STATIC_ASSERT(tphn::is_unsigned<value_type>::value, "Signed frame check type not supported");

    //*************************************************************************
    /// Default constructor.
    //*************************************************************************
    frame_check_sequence()
    {
      reset();
    }

    //*************************************************************************
    /// Constructor from range.
    /// \param begin Start of the range.
    /// \param end   End of the range.
    //*************************************************************************
    template<typename TIterator>
    frame_check_sequence(TIterator begin, const TIterator end)
    {
      TYPHOON_STATIC_ASSERT(sizeof(typename tphn::iterator_traits<TIterator>::value_type) == 1, "Type not supported");

      reset();
      add(begin, end);
    }

    //*************************************************************************
    /// Resets the FCS to the initial state.
    //*************************************************************************
    void reset()
    {
      frame_check = policy.initial();
    }

    //*************************************************************************
    /// Adds a range.
    /// \param begin
    /// \param end
    //*************************************************************************
    template<typename TIterator>
    void add(TIterator begin, const TIterator end)
    {
      TYPHOON_STATIC_ASSERT(sizeof(typename tphn::iterator_traits<TIterator>::value_type) == 1, "Type not supported");

      while (begin != end)
      {
        frame_check = policy.add(frame_check, *begin);
        ++begin;
      }
    }

    //*************************************************************************
    /// \param value The uint8_t to add to the FCS.
    //*************************************************************************
    void add(uint8_t value_)
    {
      frame_check = policy.add(frame_check, value_);
    }

    //*************************************************************************
    /// Gets the FCS value.
    //*************************************************************************
    value_type value() const
    {
      return policy.final(frame_check);
    }

    //*************************************************************************
    /// Conversion operator to value_type.
    //*************************************************************************
    operator value_type () const
    {
      return policy.final(frame_check);
    }

    //*************************************************************************
    /// Gets an add_insert_iterator for input.
    //*************************************************************************
    add_insert_iterator input()
    {
      return add_insert_iterator(*this);
    }

  private:

    value_type  frame_check;
    policy_type policy;
  };
}

#endif
