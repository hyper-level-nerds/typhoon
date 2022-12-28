/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tphn
https://www.tphncpp.com

Copyright(c) 2021 Jeremy Overesch, John Wellbelove

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

#ifndef TYPHOON_HFSM_HPP
#define TYPHOON_HFSM_HPP

#include "fsm.hpp"

namespace tphn
{
  //***************************************************************************
  /// The HFSM class.
  /// Builds on the FSM class by overriding the receive function and adding
  /// state hierarchy walking functions.
  //***************************************************************************
  class hfsm : public tphn::fsm
  {
  public:

    //*******************************************
    /// Constructor.
    //*******************************************
    hfsm(tphn::message_router_id_t id)
      : fsm(id)
    {
    }

    using fsm::receive;

    //*******************************************
    /// Top level message handler for the HFSM.
    //*******************************************
    void receive(const tphn::imessage& message) TYPHOON_OVERRIDE
    {
      tphn::fsm_state_id_t next_state_id = p_state->process_event(message);

      if (next_state_id != ifsm_state::No_State_Change)
      {
        TYPHOON_ASSERT(next_state_id < number_of_states, TYPHOON_ERROR(tphn::fsm_state_id_exception));
        tphn::ifsm_state* p_next_state = state_list[next_state_id];

        // Have we changed state?
        if (p_next_state != p_state)
        {
          tphn::ifsm_state* p_root = common_ancestor(p_state, p_next_state);
          do_exits(p_root, p_state);

          p_state = p_next_state;

          next_state_id = do_enters(p_root, p_next_state, true);

          if (next_state_id != ifsm_state::No_State_Change)
          {
            TYPHOON_ASSERT(next_state_id < number_of_states, TYPHOON_ERROR(tphn::fsm_state_id_exception));
            p_state = state_list[next_state_id];
          }
        }
      }
    }

  private:

    //*******************************************
    /// Return the first common ancestor of the two states.
    //*******************************************
    static tphn::ifsm_state* common_ancestor(tphn::ifsm_state* s1, tphn::ifsm_state* s2)
    {
      size_t depth1 = get_depth(s1);
      size_t depth2 = get_depth(s2);

      // Adjust s1 and s2 to the same depth.
      if (depth1 > depth2)
      {
        s1 = adjust_depth(s1, depth1 - depth2);
      }
      else
      {
        s2 = adjust_depth(s2, depth2 - depth1);
      }

      // Now they're aligned to the same depth they can step towards the root together.
      while (s1 != s2)
      {
        s1 = s1->p_parent;
        s2 = s2->p_parent;
      }

      return s1;
    }

    //*******************************************
    /// Find the depth of the state.
    //*******************************************
    static size_t get_depth(tphn::ifsm_state* s)
    {
      size_t depth = 0UL;

      while (s != TYPHOON_NULLPTR)
      {
        s = s->p_parent;
        ++depth;
      }

      return depth;
    }

    //*******************************************
    /// Align the depths of the states.
    //*******************************************
    static tphn::ifsm_state* adjust_depth(tphn::ifsm_state* s, size_t offset)
    {
      while (offset != 0U)
      {
        s = s->p_parent;
        --offset;
      }

      return s;
    }

    //*******************************************
    /// Entering the state.
    //*******************************************
    static tphn::fsm_state_id_t do_enters(const tphn::ifsm_state* p_root, tphn::ifsm_state* p_target, bool activate_default_children)
    {
      TYPHOON_ASSERT(p_target != TYPHOON_NULLPTR, TYPHOON_ERROR(tphn::fsm_null_state_exception));

      // We need to go recursively up the tree if the target and root don't match
      if ((p_root != p_target) && (p_target->p_parent != TYPHOON_NULLPTR))
      {
        if (p_target->p_parent != p_root)
        {
          // The parent we're calling shouldn't activate its defaults, or this state will be deactivated.
          do_enters(p_root, p_target->p_parent, false);
        }

        // Set us as our parent's active child
        p_target->p_parent->p_active_child = p_target;
      }

      tphn::fsm_state_id_t next_state = p_target->on_enter_state();
      TYPHOON_ASSERT(ifsm_state::No_State_Change == next_state, TYPHOON_ERROR(tphn::fsm_state_composite_state_change_forbidden));

      // Activate default child if we need to activate any initial states in an active composite state.
      if (activate_default_children)
      {
        while (p_target->p_default_child != TYPHOON_NULLPTR)
        {
          p_target = p_target->p_default_child;
          p_target->p_parent->p_active_child = p_target;
          next_state = p_target->on_enter_state();
          TYPHOON_ASSERT(ifsm_state::No_State_Change == next_state, TYPHOON_ERROR(tphn::fsm_state_composite_state_change_forbidden));
        }

        next_state = p_target->get_state_id();
      }

      return next_state;
    }

    //*******************************************
    /// Exiting the state.
    //*******************************************
    static void do_exits(const tphn::ifsm_state* p_root, tphn::ifsm_state* p_source)
    {
      tphn::ifsm_state* p_current = p_source;

      // Iterate down to the lowest child
      while (p_current->p_active_child != TYPHOON_NULLPTR)
      {
        p_current = p_current->p_active_child;
      }

      // Run exit state on all states up to the root
      while (p_current != p_root)
      {
        p_current->on_exit_state();
        p_current = p_current->p_parent;
      }
    }
  };
}
#endif
