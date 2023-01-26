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

#ifndef TYPHOON_INTRUSIVE_LINKS_HPP
#define TYPHOON_INTRUSIVE_LINKS_HPP

#include "platform.hpp"
#include "nullptr.hpp"
#include "type_traits.hpp"
#include "exception.hpp"
#include "error_handler.hpp"
#include "utility.hpp"
#include "algorithm.hpp"

#include <assert.h>

//*****************************************************************************
// Note:
// The link functions work slightly differently to the STL 'insert' convention
// in that the second link parameter will be inserted after the first.
// i.e.
// If the list contains '1', '2', '3', '4' and "link_splice '2','5'" is invoked the
// resulting list will contain '1', '2', '5', '3', '4'
// This is to maintain consistency between forward and bidirectional links
// and also is intuitive.
//*****************************************************************************

namespace tpn
{
  //***************************************************************************
  /// Link exception.
  //***************************************************************************
  class link_exception : public tpn::exception
  {
  public:

    link_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// not unlinked exception.
  //***************************************************************************
  class not_unlinked_exception : public tpn::link_exception
  {
  public:

    not_unlinked_exception(string_type file_name_, numeric_type line_number_)
      : link_exception(TYPHOON_ERROR_TEXT("link:still linked", TYPHOON_INTRUSIVE_LINKS_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// A forward link.
  //***************************************************************************
  template <const size_t ID_>
  struct forward_link
  {
    enum
    {
      ID = ID_,
    };

    void clear()
    {
      tpn_next = TYPHOON_NULLPTR;
    }

    bool is_linked() const
    {
      return tpn_next != TYPHOON_NULLPTR;
    }

    forward_link* tpn_next;
  };

  // Reference, Reference
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::forward_link<TLink::ID> >::value, void>::type
  link(TLink& lhs, TLink& rhs)
  {
    lhs.tpn_next = &rhs;
  }

  // Reference, Reference
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::forward_link<TLink::ID> >::value, void>::type
  link_splice(TLink& lhs, TLink& rhs)
  {
    rhs.tpn_next = lhs.tpn_next;
    lhs.tpn_next = &rhs;
  }

  // Pointer, Pointer
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::forward_link<TLink::ID> >::value, void>::type
  link(TLink* lhs, TLink* rhs)
  {
    if (lhs != TYPHOON_NULLPTR)
    {
      lhs->tpn_next = rhs;
    }
  }

  // Pointer, Pointer
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::forward_link<TLink::ID> >::value, void>::type
  link_splice(TLink* lhs, TLink* rhs)
  {
    if (lhs != TYPHOON_NULLPTR)
    {
      if (rhs != TYPHOON_NULLPTR)
      {
          rhs->tpn_next = lhs->tpn_next;
      }

      lhs->tpn_next = rhs;
    }
  }

  // Reference, Pointer
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::forward_link<TLink::ID> >::value, void>::type
  link(TLink& lhs, TLink* rhs)
  {
    lhs.tpn_next = rhs;
  }

  // Reference, Pointer
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::forward_link<TLink::ID> >::value, void>::type
  link_splice(TLink& lhs, TLink* rhs)
  {
    if (rhs != TYPHOON_NULLPTR)
    {
      rhs->tpn_next = lhs.tpn_next;
    }

    lhs.tpn_next = rhs;
  }

  // Pointer, Reference
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::forward_link<TLink::ID> >::value, void>::type
  link(TLink* lhs, TLink& rhs)
  {
    if (lhs != TYPHOON_NULLPTR)
    {
      lhs->tpn_next = &rhs;
    }
  }

  // Pointer, Reference
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::forward_link<TLink::ID> >::value, void>::type
  link_splice(TLink* lhs, TLink& rhs)
  {
    if (lhs != TYPHOON_NULLPTR)
    {
      rhs.tpn_next  = lhs->tpn_next;
      lhs->tpn_next = &rhs;
    }
  }

  // Reference, Reference, Reference
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::forward_link<TLink::ID> >::value, void>::type
  link_splice(TLink& lhs, TLink& first, TLink& last)
  {
    last.tpn_next = lhs.tpn_next;
    lhs.tpn_next  = &first;
  }

  // Pointer, Reference, Reference
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::forward_link<TLink::ID> >::value, void>::type
  link_splice(TLink* lhs, TLink& first, TLink& last)
  {
    if (lhs != TYPHOON_NULLPTR)
    {
      last.tpn_next = lhs->tpn_next;
      lhs->tpn_next  = &first;
    }
    else
    {
      last.tpn_next = TYPHOON_NULLPTR;
    }
  }

  // Reference
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::forward_link<TLink::ID> >::value, void>::type
  unlink_after(TLink& node)
  {
    if (node.tpn_next != TYPHOON_NULLPTR)
    {
      TLink* unlinked_node = node.tpn_next;
      node.tpn_next = unlinked_node->tpn_next;
    }
  }

  // Reference, Reference
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::forward_link<TLink::ID> >::value, void>::type
  unlink_after(TLink& before, TLink& last)
  {
    before.tpn_next = last.tpn_next;
  }

  //***************************************************************************
  /// A bidirectional link.
  //***************************************************************************
  template <const size_t ID_>
  struct bidirectional_link
  {
    enum
    {
      ID = ID_,
    };

    void clear()
    {
      tpn_previous = TYPHOON_NULLPTR;
      tpn_next     = TYPHOON_NULLPTR;
    }

    bool is_linked() const
    {
      return (tpn_previous != TYPHOON_NULLPTR) || (tpn_next != TYPHOON_NULLPTR);
    }

    void reverse()
    {
      using TYPHOON_OR_STD::swap; // Allow ADL

      swap(tpn_previous, tpn_next);
    }

    bidirectional_link* tpn_previous;
    bidirectional_link* tpn_next;

    void unlink()
    {
        // Connect the previous link with the next.
        if (tpn_previous != TYPHOON_NULLPTR)
        {
          tpn_previous->tpn_next = tpn_next;
        }

        // Connect the next link with the previous.
        if (tpn_next != TYPHOON_NULLPTR)
        {
          tpn_next->tpn_previous = tpn_previous;
        }
    }
  };

  // Reference, Reference
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::bidirectional_link<TLink::ID> >::value, void>::type
  link(TLink& lhs, TLink& rhs)
  {
    lhs.tpn_next     = &rhs;
    rhs.tpn_previous = &lhs;
  }

  // Reference, Reference
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::bidirectional_link<TLink::ID> >::value, void>::type
  link_splice(TLink& lhs, TLink& rhs)
  {
    rhs.tpn_next     = lhs.tpn_next;
    rhs.tpn_previous = &lhs;

    if (lhs.tpn_next != TYPHOON_NULLPTR)
    {
      lhs.tpn_next->tpn_previous = &rhs;
    }

    lhs.tpn_next = &rhs;
  }

  // Pointer, Pointer
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::bidirectional_link<TLink::ID> >::value, void>::type
  link(TLink* lhs, TLink* rhs)
  {
    if (lhs != TYPHOON_NULLPTR)
    {
      lhs->tpn_next = rhs;
    }

    if (rhs != TYPHOON_NULLPTR)
    {
      rhs->tpn_previous = lhs;
    }
  }

  // Pointer, Pointer
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::bidirectional_link<TLink::ID> >::value, void>::type
  link_splice(TLink* lhs, TLink* rhs)
  {
    if (rhs != TYPHOON_NULLPTR)
    {
      if (lhs != TYPHOON_NULLPTR)
      {
        rhs->tpn_next = lhs->tpn_next;
      }

      rhs->tpn_previous = lhs;
    }

    if (lhs != TYPHOON_NULLPTR)
    {
      if (lhs->tpn_next != TYPHOON_NULLPTR)
      {
        lhs->tpn_next->tpn_previous = rhs;
      }

      lhs->tpn_next = rhs;
    }
  }

  // Reference, Pointer
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::bidirectional_link<TLink::ID> >::value, void>::type
  link(TLink& lhs, TLink* rhs)
  {
    lhs.tpn_next = rhs;

    if (rhs != TYPHOON_NULLPTR)
    {
      rhs->tpn_previous = &lhs;
    }
  }

  // Reference, Pointer
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::bidirectional_link<TLink::ID> >::value, void>::type
  link_splice(TLink& lhs, TLink* rhs)
  {
    if (rhs != TYPHOON_NULLPTR)
    {
      rhs->tpn_next     = lhs.tpn_next;
      rhs->tpn_previous = &lhs;
    }

    if (lhs.tpn_next != TYPHOON_NULLPTR)
    {
      lhs.tpn_next->tpn_previous = rhs;
    }

    lhs.tpn_next = rhs;
  }

  // Pointer, Reference
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::bidirectional_link<TLink::ID> >::value, void>::type
  link(TLink* lhs, TLink& rhs)
  {
    if (lhs != TYPHOON_NULLPTR)
    {
      lhs->tpn_next = &rhs;
    }

    rhs.tpn_previous = lhs;
  }

  // Pointer, Reference
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::bidirectional_link<TLink::ID> >::value, void>::type
  link_splice(TLink* lhs, TLink& rhs)
  {
    if (lhs != TYPHOON_NULLPTR)
    {
      rhs.tpn_next = lhs->tpn_next;
    }

    rhs.tpn_previous = lhs;

    if (lhs != TYPHOON_NULLPTR)
    {
      if (lhs->tpn_next != TYPHOON_NULLPTR)
      {
        lhs->tpn_next->tpn_previous = &rhs;
      }

      lhs->tpn_next = &rhs;
    }
  }

  // Reference, Reference, Reference
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::bidirectional_link<TLink::ID> >::value, void>::type
  link_splice(TLink& lhs, TLink& first, TLink& last)
  {
    last.tpn_next = lhs.tpn_next;
    first.tpn_previous = &lhs;

    if (last.tpn_next != TYPHOON_NULLPTR)
    {
      last.tpn_next->tpn_previous = &last;
    }

    lhs.tpn_next = &first;
  }

  // Pointer, Reference, Reference
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::bidirectional_link<TLink::ID> >::value, void>::type
  link_splice(TLink* lhs, TLink& first, TLink& last)
  {
    if (lhs != TYPHOON_NULLPTR)
    {
      last.tpn_next = lhs->tpn_next;
    }
    else
    {
      last.tpn_next = TYPHOON_NULLPTR;
    }

    first.tpn_previous = lhs;

    if (last.tpn_next != TYPHOON_NULLPTR)
    {
      last.tpn_next->tpn_previous = &last;
    }

    if (lhs != TYPHOON_NULLPTR)
    {
      lhs->tpn_next = &first;
    }
  }

  // Reference
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::bidirectional_link<TLink::ID> >::value, void>::type
  unlink(TLink& node)
  {
    node.unlink();
  }

  // Reference Reference
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::bidirectional_link<TLink::ID> >::value, void>::type
  unlink(TLink& first, TLink& last)
  {
    if (&first == &last)
    {
      first.unlink();
    }
    else
    {
      if (last.tpn_next != TYPHOON_NULLPTR)
      {
        last.tpn_next->tpn_previous = first.tpn_previous;
      }

      if (first.tpn_previous != TYPHOON_NULLPTR)
      {
        first.tpn_previous->tpn_next = last.tpn_next;
      }
    }
  }

  //***************************************************************************
  /// A binary tree link.
  //***************************************************************************
  template <const size_t ID_>
  struct tree_link
  {
      enum
      {
        ID = ID_,
      };

      void clear()
      {
        tpn_parent = TYPHOON_NULLPTR;
        tpn_left   = TYPHOON_NULLPTR;
        tpn_right  = TYPHOON_NULLPTR;
      }

      bool is_linked() const
      {
        return (tpn_parent != TYPHOON_NULLPTR) || (tpn_left != TYPHOON_NULLPTR) || (tpn_right != TYPHOON_NULLPTR);
      }

      tree_link* tpn_parent;
      tree_link* tpn_left;
      tree_link* tpn_right;
  };

  // Reference, Reference
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::tree_link<TLink::ID> >::value, void>::type
  link_left(TLink& parent, TLink& leaf)
  {
    parent.tpn_left = &leaf;
    leaf.tpn_parent = &parent;
  }

  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::tree_link<TLink::ID> >::value, void>::type
  link_right(TLink& parent, TLink& leaf)
  {
    parent.tpn_right = &leaf;
    leaf.tpn_parent  = &parent;
  }

  // Pointer, Pointer
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::tree_link<TLink::ID> >::value, void>::type
  link_left(TLink* parent, TLink* leaf)
  {
    if (parent != TYPHOON_NULLPTR)
    {
      parent->tpn_left = leaf;
    }

    if (leaf != TYPHOON_NULLPTR)
    {
      leaf->tpn_parent = parent;
    }
  }

  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::tree_link<TLink::ID> >::value, void>::type
  link_right(TLink* parent, TLink* leaf)
  {
    if (parent != TYPHOON_NULLPTR)
    {
      parent->tpn_right = leaf;
    }

    if (leaf != TYPHOON_NULLPTR)
    {
      leaf->tpn_parent = parent;
    }
  }

  // Reference, Pointer
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::tree_link<TLink::ID> >::value, void>::type
  link_left(TLink& parent, TLink* leaf)
  {
    parent.tpn_left = leaf;

    if (leaf != TYPHOON_NULLPTR)
    {
      leaf->tpn_parent = &parent;
    }
  }

  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::tree_link<TLink::ID> >::value, void>::type
  link_right(TLink& parent, TLink* leaf)
  {
    parent.tpn_right = leaf;

    if (leaf != TYPHOON_NULLPTR)
    {
      leaf->tpn_parent = &parent;
    }
  }

  // Pointer, Reference
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::tree_link<TLink::ID> >::value, void>::type
  link_left(TLink* parent, TLink& leaf)
  {
    if (parent != TYPHOON_NULLPTR)
    {
      parent->tpn_left = &leaf;
    }

    leaf.tpn_parent = parent;
  }

  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::tree_link<TLink::ID> >::value, void>::type
  link_right(TLink* parent, TLink& leaf)
  {
    if (parent != TYPHOON_NULLPTR)
    {
      parent->tpn_right = &leaf;
    }

    leaf.tpn_parent = parent;
  }

  // Reference, Reference
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::tree_link<TLink::ID> >::value, void>::type
  link_rotate_left(TLink& parent, TLink& leaf)
  {
     parent.tpn_right = leaf.tpn_left;

     if (parent.tpn_right != TYPHOON_NULLPTR)
     {
       parent.tpn_right->tpn_parent = &parent;
     }

     leaf.tpn_parent   = parent.tpn_parent;
     parent.tpn_parent = &leaf;
     leaf.tpn_left     = &parent;
  }

  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::tree_link<TLink::ID> >::value, void>::type
  link_rotate_right(TLink& parent, TLink& leaf)
  {
     parent.tpn_left = leaf.tpn_right;

     if (parent.tpn_left != TYPHOON_NULLPTR)
     {
       parent.tpn_left->tpn_parent = &parent;
     }

     leaf.tpn_parent   = parent.tpn_parent;
     parent.tpn_parent = &leaf;
     leaf.tpn_right    = &parent;
  }

  // Pointer, Pointer
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::tree_link<TLink::ID> >::value, void>::type
  link_rotate_left(TLink* parent, TLink* leaf)
  {
    if ((parent != TYPHOON_NULLPTR) && (leaf != TYPHOON_NULLPTR))
    {
      link_rotate_left(*parent, *leaf);
    }
  }

  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::tree_link<TLink::ID> >::value, void>::type
  link_rotate_right(TLink* parent, TLink* leaf)
  {
    if ((parent != TYPHOON_NULLPTR) && (leaf != TYPHOON_NULLPTR))
    {
      link_rotate_right(*parent, *leaf);
    }
  }

  // Reference, Pointer
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::tree_link<TLink::ID> >::value, void>::type
  link_rotate_left(TLink& parent, TLink* leaf)
  {
    if (leaf != TYPHOON_NULLPTR)
    {
      link_rotate_left(parent, *leaf);
    }
  }

  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::tree_link<TLink::ID> >::value, void>::type
  link_rotate_right(TLink& parent, TLink* leaf)
  {
    if (leaf != TYPHOON_NULLPTR)
    {
      link_rotate_right(parent, *leaf);
    }
  }

  // Pointer, Reference
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::tree_link<TLink::ID> >::value, void>::type
  link_rotate_left(TLink* parent, TLink& leaf)
  {
    if (parent != TYPHOON_NULLPTR)
    {
      link_rotate_left(*parent, leaf);
    }
  }

  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::tree_link<TLink::ID> >::value, void>::type
  link_rotate_right(TLink* parent, TLink& leaf)
  {
    if (parent != TYPHOON_NULLPTR)
    {
      link_rotate_right(*parent, leaf);
    }
  }

  // Reference, Reference
  /// Automatically detects whether a left or right rotate is expected.
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::tree_link<TLink::ID> >::value, void>::type
  link_rotate(TLink& parent, TLink& leaf)
  {
    if (parent.tpn_left == &leaf)
    {
      tpn::link_rotate_right(parent, leaf);
    }
    else
    {
      tpn::link_rotate_left(parent, leaf);
    }
  }

  // Pointer, Pointer
  /// Automatically detects whether a left or right rotate is expected.
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::tree_link<TLink::ID> >::value, void>::type
  link_rotate(TLink* parent, TLink* leaf)
  {
    if ((parent != TYPHOON_NULLPTR) && (leaf != TYPHOON_NULLPTR))
    {
      if (parent->tpn_left == leaf)
      {
        tpn::link_rotate_right(*parent, *leaf);
      }
      else
      {
        tpn::link_rotate_left(*parent, *leaf);
      }
    }
  }

  // Reference, Pointer
  /// Automatically detects whether a left or right rotate is expected.
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::tree_link<TLink::ID> >::value, void>::type
  link_rotate(TLink& parent, TLink* leaf)
  {
    if (leaf != TYPHOON_NULLPTR)
    {
      if (parent.tpn_left == leaf)
      {
        tpn::link_rotate_right(parent, *leaf);
      }
      else
      {
        tpn::link_rotate_left(parent, *leaf);
      }
    }
  }

  // Pointer, Reference
  /// Automatically detects whether a left or right rotate is expected.
  template <typename TLink>
  typename tpn::enable_if<tpn::is_same<TLink, tpn::tree_link<TLink::ID> >::value, void>::type
  link_rotate(TLink* parent, TLink& leaf)
  {
    if (parent != TYPHOON_NULLPTR)
    {
      if (parent->tpn_left == &leaf)
      {
        tpn::link_rotate_right(*parent, leaf);
      }
      else
      {
        tpn::link_rotate_left(*parent, leaf);
      }
    }
  }
}

#endif
