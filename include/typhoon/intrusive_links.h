///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tphn
https://www.tphncpp.com

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

#ifndef TYPHOON_INTRUSIVE_LINKS_INCLUDED
#define TYPHOON_INTRUSIVE_LINKS_INCLUDED

#include "platform.h"
#include "nullptr.h"
#include "type_traits.h"
#include "exception.h"
#include "error_handler.h"
#include "utility.h"
#include "algorithm.h"

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

namespace tphn
{
  //***************************************************************************
  /// Link exception.
  //***************************************************************************
  class link_exception : public tphn::exception
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
  class not_unlinked_exception : public tphn::link_exception
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
      tphn_next = TYPHOON_NULLPTR;
    }

    bool is_linked() const
    {
      return tphn_next != TYPHOON_NULLPTR;
    }

    forward_link* tphn_next;
  };

  // Reference, Reference
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::forward_link<TLink::ID> >::value, void>::type
  link(TLink& lhs, TLink& rhs)
  {
    lhs.tphn_next = &rhs;
  }

  // Reference, Reference
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::forward_link<TLink::ID> >::value, void>::type
  link_splice(TLink& lhs, TLink& rhs)
  {
    rhs.tphn_next = lhs.tphn_next;
    lhs.tphn_next = &rhs;
  }

  // Pointer, Pointer
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::forward_link<TLink::ID> >::value, void>::type
  link(TLink* lhs, TLink* rhs)
  {
    if (lhs != TYPHOON_NULLPTR)
    {
      lhs->tphn_next = rhs;
    }
  }

  // Pointer, Pointer
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::forward_link<TLink::ID> >::value, void>::type
  link_splice(TLink* lhs, TLink* rhs)
  {
    if (lhs != TYPHOON_NULLPTR)
    {
      if (rhs != TYPHOON_NULLPTR)
      {
          rhs->tphn_next = lhs->tphn_next;
      }

      lhs->tphn_next = rhs;
    }
  }

  // Reference, Pointer
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::forward_link<TLink::ID> >::value, void>::type
  link(TLink& lhs, TLink* rhs)
  {
    lhs.tphn_next = rhs;
  }

  // Reference, Pointer
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::forward_link<TLink::ID> >::value, void>::type
  link_splice(TLink& lhs, TLink* rhs)
  {
    if (rhs != TYPHOON_NULLPTR)
    {
      rhs->tphn_next = lhs.tphn_next;
    }

    lhs.tphn_next = rhs;
  }

  // Pointer, Reference
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::forward_link<TLink::ID> >::value, void>::type
  link(TLink* lhs, TLink& rhs)
  {
    if (lhs != TYPHOON_NULLPTR)
    {
      lhs->tphn_next = &rhs;
    }
  }

  // Pointer, Reference
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::forward_link<TLink::ID> >::value, void>::type
  link_splice(TLink* lhs, TLink& rhs)
  {
    if (lhs != TYPHOON_NULLPTR)
    {
      rhs.tphn_next  = lhs->tphn_next;
      lhs->tphn_next = &rhs;
    }
  }

  // Reference, Reference, Reference
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::forward_link<TLink::ID> >::value, void>::type
  link_splice(TLink& lhs, TLink& first, TLink& last)
  {
    last.tphn_next = lhs.tphn_next;
    lhs.tphn_next  = &first;
  }

  // Pointer, Reference, Reference
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::forward_link<TLink::ID> >::value, void>::type
  link_splice(TLink* lhs, TLink& first, TLink& last)
  {
    if (lhs != TYPHOON_NULLPTR)
    {
      last.tphn_next = lhs->tphn_next;
      lhs->tphn_next  = &first;
    }
    else
    {
      last.tphn_next = TYPHOON_NULLPTR;
    }
  }

  // Reference
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::forward_link<TLink::ID> >::value, void>::type
  unlink_after(TLink& node)
  {
    if (node.tphn_next != TYPHOON_NULLPTR)
    {
      TLink* unlinked_node = node.tphn_next;
      node.tphn_next = unlinked_node->tphn_next;
    }
  }

  // Reference, Reference
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::forward_link<TLink::ID> >::value, void>::type
  unlink_after(TLink& before, TLink& last)
  {
    before.tphn_next = last.tphn_next;
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
      tphn_previous = TYPHOON_NULLPTR;
      tphn_next     = TYPHOON_NULLPTR;
    }

    bool is_linked() const
    {
      return (tphn_previous != TYPHOON_NULLPTR) || (tphn_next != TYPHOON_NULLPTR);
    }

    void reverse()
    {
      using TYPHOON_OR_STD::swap; // Allow ADL

      swap(tphn_previous, tphn_next);
    }

    bidirectional_link* tphn_previous;
    bidirectional_link* tphn_next;

    void unlink()
    {
        // Connect the previous link with the next.
        if (tphn_previous != TYPHOON_NULLPTR)
        {
          tphn_previous->tphn_next = tphn_next;
        }

        // Connect the next link with the previous.
        if (tphn_next != TYPHOON_NULLPTR)
        {
          tphn_next->tphn_previous = tphn_previous;
        }
    }
  };

  // Reference, Reference
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::bidirectional_link<TLink::ID> >::value, void>::type
  link(TLink& lhs, TLink& rhs)
  {
    lhs.tphn_next     = &rhs;
    rhs.tphn_previous = &lhs;
  }

  // Reference, Reference
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::bidirectional_link<TLink::ID> >::value, void>::type
  link_splice(TLink& lhs, TLink& rhs)
  {
    rhs.tphn_next     = lhs.tphn_next;
    rhs.tphn_previous = &lhs;

    if (lhs.tphn_next != TYPHOON_NULLPTR)
    {
      lhs.tphn_next->tphn_previous = &rhs;
    }

    lhs.tphn_next = &rhs;
  }

  // Pointer, Pointer
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::bidirectional_link<TLink::ID> >::value, void>::type
  link(TLink* lhs, TLink* rhs)
  {
    if (lhs != TYPHOON_NULLPTR)
    {
      lhs->tphn_next = rhs;
    }

    if (rhs != TYPHOON_NULLPTR)
    {
      rhs->tphn_previous = lhs;
    }
  }

  // Pointer, Pointer
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::bidirectional_link<TLink::ID> >::value, void>::type
  link_splice(TLink* lhs, TLink* rhs)
  {
    if (rhs != TYPHOON_NULLPTR)
    {
      if (lhs != TYPHOON_NULLPTR)
      {
        rhs->tphn_next = lhs->tphn_next;
      }

      rhs->tphn_previous = lhs;
    }

    if (lhs != TYPHOON_NULLPTR)
    {
      if (lhs->tphn_next != TYPHOON_NULLPTR)
      {
        lhs->tphn_next->tphn_previous = rhs;
      }

      lhs->tphn_next = rhs;
    }
  }

  // Reference, Pointer
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::bidirectional_link<TLink::ID> >::value, void>::type
  link(TLink& lhs, TLink* rhs)
  {
    lhs.tphn_next = rhs;

    if (rhs != TYPHOON_NULLPTR)
    {
      rhs->tphn_previous = &lhs;
    }
  }

  // Reference, Pointer
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::bidirectional_link<TLink::ID> >::value, void>::type
  link_splice(TLink& lhs, TLink* rhs)
  {
    if (rhs != TYPHOON_NULLPTR)
    {
      rhs->tphn_next     = lhs.tphn_next;
      rhs->tphn_previous = &lhs;
    }

    if (lhs.tphn_next != TYPHOON_NULLPTR)
    {
      lhs.tphn_next->tphn_previous = rhs;
    }

    lhs.tphn_next = rhs;
  }

  // Pointer, Reference
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::bidirectional_link<TLink::ID> >::value, void>::type
  link(TLink* lhs, TLink& rhs)
  {
    if (lhs != TYPHOON_NULLPTR)
    {
      lhs->tphn_next = &rhs;
    }

    rhs.tphn_previous = lhs;
  }

  // Pointer, Reference
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::bidirectional_link<TLink::ID> >::value, void>::type
  link_splice(TLink* lhs, TLink& rhs)
  {
    if (lhs != TYPHOON_NULLPTR)
    {
      rhs.tphn_next = lhs->tphn_next;
    }

    rhs.tphn_previous = lhs;

    if (lhs != TYPHOON_NULLPTR)
    {
      if (lhs->tphn_next != TYPHOON_NULLPTR)
      {
        lhs->tphn_next->tphn_previous = &rhs;
      }

      lhs->tphn_next = &rhs;
    }
  }

  // Reference, Reference, Reference
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::bidirectional_link<TLink::ID> >::value, void>::type
  link_splice(TLink& lhs, TLink& first, TLink& last)
  {
    last.tphn_next = lhs.tphn_next;
    first.tphn_previous = &lhs;

    if (last.tphn_next != TYPHOON_NULLPTR)
    {
      last.tphn_next->tphn_previous = &last;
    }

    lhs.tphn_next = &first;
  }

  // Pointer, Reference, Reference
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::bidirectional_link<TLink::ID> >::value, void>::type
  link_splice(TLink* lhs, TLink& first, TLink& last)
  {
    if (lhs != TYPHOON_NULLPTR)
    {
      last.tphn_next = lhs->tphn_next;
    }
    else
    {
      last.tphn_next = TYPHOON_NULLPTR;
    }

    first.tphn_previous = lhs;

    if (last.tphn_next != TYPHOON_NULLPTR)
    {
      last.tphn_next->tphn_previous = &last;
    }

    if (lhs != TYPHOON_NULLPTR)
    {
      lhs->tphn_next = &first;
    }
  }

  // Reference
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::bidirectional_link<TLink::ID> >::value, void>::type
  unlink(TLink& node)
  {
    node.unlink();
  }

  // Reference Reference
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::bidirectional_link<TLink::ID> >::value, void>::type
  unlink(TLink& first, TLink& last)
  {
    if (&first == &last)
    {
      first.unlink();
    }
    else
    {
      if (last.tphn_next != TYPHOON_NULLPTR)
      {
        last.tphn_next->tphn_previous = first.tphn_previous;
      }

      if (first.tphn_previous != TYPHOON_NULLPTR)
      {
        first.tphn_previous->tphn_next = last.tphn_next;
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
        tphn_parent = TYPHOON_NULLPTR;
        tphn_left   = TYPHOON_NULLPTR;
        tphn_right  = TYPHOON_NULLPTR;
      }

      bool is_linked() const
      {
        return (tphn_parent != TYPHOON_NULLPTR) || (tphn_left != TYPHOON_NULLPTR) || (tphn_right != TYPHOON_NULLPTR);
      }

      tree_link* tphn_parent;
      tree_link* tphn_left;
      tree_link* tphn_right;
  };

  // Reference, Reference
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::tree_link<TLink::ID> >::value, void>::type
  link_left(TLink& parent, TLink& leaf)
  {
    parent.tphn_left = &leaf;
    leaf.tphn_parent = &parent;
  }

  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::tree_link<TLink::ID> >::value, void>::type
  link_right(TLink& parent, TLink& leaf)
  {
    parent.tphn_right = &leaf;
    leaf.tphn_parent  = &parent;
  }

  // Pointer, Pointer
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::tree_link<TLink::ID> >::value, void>::type
  link_left(TLink* parent, TLink* leaf)
  {
    if (parent != TYPHOON_NULLPTR)
    {
      parent->tphn_left = leaf;
    }

    if (leaf != TYPHOON_NULLPTR)
    {
      leaf->tphn_parent = parent;
    }
  }

  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::tree_link<TLink::ID> >::value, void>::type
  link_right(TLink* parent, TLink* leaf)
  {
    if (parent != TYPHOON_NULLPTR)
    {
      parent->tphn_right = leaf;
    }

    if (leaf != TYPHOON_NULLPTR)
    {
      leaf->tphn_parent = parent;
    }
  }

  // Reference, Pointer
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::tree_link<TLink::ID> >::value, void>::type
  link_left(TLink& parent, TLink* leaf)
  {
    parent.tphn_left = leaf;

    if (leaf != TYPHOON_NULLPTR)
    {
      leaf->tphn_parent = &parent;
    }
  }

  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::tree_link<TLink::ID> >::value, void>::type
  link_right(TLink& parent, TLink* leaf)
  {
    parent.tphn_right = leaf;

    if (leaf != TYPHOON_NULLPTR)
    {
      leaf->tphn_parent = &parent;
    }
  }

  // Pointer, Reference
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::tree_link<TLink::ID> >::value, void>::type
  link_left(TLink* parent, TLink& leaf)
  {
    if (parent != TYPHOON_NULLPTR)
    {
      parent->tphn_left = &leaf;
    }

    leaf.tphn_parent = parent;
  }

  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::tree_link<TLink::ID> >::value, void>::type
  link_right(TLink* parent, TLink& leaf)
  {
    if (parent != TYPHOON_NULLPTR)
    {
      parent->tphn_right = &leaf;
    }

    leaf.tphn_parent = parent;
  }

  // Reference, Reference
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::tree_link<TLink::ID> >::value, void>::type
  link_rotate_left(TLink& parent, TLink& leaf)
  {
     parent.tphn_right = leaf.tphn_left;

     if (parent.tphn_right != TYPHOON_NULLPTR)
     {
       parent.tphn_right->tphn_parent = &parent;
     }

     leaf.tphn_parent   = parent.tphn_parent;
     parent.tphn_parent = &leaf;
     leaf.tphn_left     = &parent;
  }

  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::tree_link<TLink::ID> >::value, void>::type
  link_rotate_right(TLink& parent, TLink& leaf)
  {
     parent.tphn_left = leaf.tphn_right;

     if (parent.tphn_left != TYPHOON_NULLPTR)
     {
       parent.tphn_left->tphn_parent = &parent;
     }

     leaf.tphn_parent   = parent.tphn_parent;
     parent.tphn_parent = &leaf;
     leaf.tphn_right    = &parent;
  }

  // Pointer, Pointer
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::tree_link<TLink::ID> >::value, void>::type
  link_rotate_left(TLink* parent, TLink* leaf)
  {
    if ((parent != TYPHOON_NULLPTR) && (leaf != TYPHOON_NULLPTR))
    {
      link_rotate_left(*parent, *leaf);
    }
  }

  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::tree_link<TLink::ID> >::value, void>::type
  link_rotate_right(TLink* parent, TLink* leaf)
  {
    if ((parent != TYPHOON_NULLPTR) && (leaf != TYPHOON_NULLPTR))
    {
      link_rotate_right(*parent, *leaf);
    }
  }

  // Reference, Pointer
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::tree_link<TLink::ID> >::value, void>::type
  link_rotate_left(TLink& parent, TLink* leaf)
  {
    if (leaf != TYPHOON_NULLPTR)
    {
      link_rotate_left(parent, *leaf);
    }
  }

  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::tree_link<TLink::ID> >::value, void>::type
  link_rotate_right(TLink& parent, TLink* leaf)
  {
    if (leaf != TYPHOON_NULLPTR)
    {
      link_rotate_right(parent, *leaf);
    }
  }

  // Pointer, Reference
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::tree_link<TLink::ID> >::value, void>::type
  link_rotate_left(TLink* parent, TLink& leaf)
  {
    if (parent != TYPHOON_NULLPTR)
    {
      link_rotate_left(*parent, leaf);
    }
  }

  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::tree_link<TLink::ID> >::value, void>::type
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
  typename tphn::enable_if<tphn::is_same<TLink, tphn::tree_link<TLink::ID> >::value, void>::type
  link_rotate(TLink& parent, TLink& leaf)
  {
    if (parent.tphn_left == &leaf)
    {
      tphn::link_rotate_right(parent, leaf);
    }
    else
    {
      tphn::link_rotate_left(parent, leaf);
    }
  }

  // Pointer, Pointer
  /// Automatically detects whether a left or right rotate is expected.
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::tree_link<TLink::ID> >::value, void>::type
  link_rotate(TLink* parent, TLink* leaf)
  {
    if ((parent != TYPHOON_NULLPTR) && (leaf != TYPHOON_NULLPTR))
    {
      if (parent->tphn_left == leaf)
      {
        tphn::link_rotate_right(*parent, *leaf);
      }
      else
      {
        tphn::link_rotate_left(*parent, *leaf);
      }
    }
  }

  // Reference, Pointer
  /// Automatically detects whether a left or right rotate is expected.
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::tree_link<TLink::ID> >::value, void>::type
  link_rotate(TLink& parent, TLink* leaf)
  {
    if (leaf != TYPHOON_NULLPTR)
    {
      if (parent.tphn_left == leaf)
      {
        tphn::link_rotate_right(parent, *leaf);
      }
      else
      {
        tphn::link_rotate_left(parent, *leaf);
      }
    }
  }

  // Pointer, Reference
  /// Automatically detects whether a left or right rotate is expected.
  template <typename TLink>
  typename tphn::enable_if<tphn::is_same<TLink, tphn::tree_link<TLink::ID> >::value, void>::type
  link_rotate(TLink* parent, TLink& leaf)
  {
    if (parent != TYPHOON_NULLPTR)
    {
      if (parent->tphn_left == &leaf)
      {
        tphn::link_rotate_right(*parent, leaf);
      }
      else
      {
        tphn::link_rotate_left(*parent, leaf);
      }
    }
  }
}

#endif
