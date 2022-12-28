/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tphn
https://www.tphncpp.com

Copyright(c) 2017 John Wellbelove

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

#ifndef TYPHOON_SCHEDULER_HPP
#define TYPHOON_SCHEDULER_HPP

#include "platform.hpp"
#include "vector.hpp"
#include "nullptr.hpp"
#include "error_handler.hpp"
#include "exception.hpp"
#include "task.hpp"
#include "type_traits.hpp"
#include "function.hpp"

#include <stdint.h>

namespace tphn
{
  //***************************************************************************
  /// Base exception class for scheduler.
  //***************************************************************************
  class scheduler_exception : public tphn::exception
  {
  public:

    scheduler_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : tphn::exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// 'No tasks' exception.
  //***************************************************************************
  class scheduler_no_tasks_exception : public tphn::scheduler_exception
  {
  public:

    scheduler_no_tasks_exception(string_type file_name_, numeric_type line_number_)
      : tphn::scheduler_exception(TYPHOON_ERROR_TEXT("scheduler:no tasks", TYPHOON_SCHEDULER_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// 'Null tasks' exception.
  //***************************************************************************
  class scheduler_null_task_exception : public tphn::scheduler_exception
  {
  public:

    scheduler_null_task_exception(string_type file_name_, numeric_type line_number_)
      : tphn::scheduler_exception(TYPHOON_ERROR_TEXT("scheduler:null task", TYPHOON_SCHEDULER_FILE_ID"B"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// 'Too many tasks' exception.
  //***************************************************************************
  class scheduler_too_many_tasks_exception : public tphn::scheduler_exception
  {
  public:

    scheduler_too_many_tasks_exception(string_type file_name_, numeric_type line_number_)
      : tphn::scheduler_exception(TYPHOON_ERROR_TEXT("scheduler:too many tasks", TYPHOON_SCHEDULER_FILE_ID"C"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Sequential Single.
  /// A policy the scheduler can use to decide what to do next.
  /// Only calls the task to process work once, if it has work to do.
  //***************************************************************************
  struct scheduler_policy_sequential_single
  {
    bool schedule_tasks(tphn::ivector<tphn::task*>& task_list)
    {
      bool idle = true;

      for (size_t index = 0UL; index < task_list.size(); ++index)
      {
        tphn::task& task = *(task_list[index]);

        if (task.task_request_work() > 0)
        {
          task.task_process_work();
          idle = false;
        }
      }

      return idle;
    }
  };

  /// Typedef for backwards compatibility with miss-spelt struct name.
  TYPHOON_DEPRECATED_REASON("Misspelt class name") typedef scheduler_policy_sequential_single scheduler_policy_sequencial_single;

  //***************************************************************************
  /// Sequential Multiple.
  /// A policy the scheduler can use to decide what to do next.
  /// Calls the task to process work until it reports that it has no more.
  //***************************************************************************
  struct scheduler_policy_sequential_multiple
  {
    bool schedule_tasks(tphn::ivector<tphn::task*>& task_list)
    {
      bool idle = true;

      for (size_t index = 0UL; index < task_list.size(); ++index)
      {
        tphn::task& task = *(task_list[index]);

        while (task.task_request_work() > 0)
        {
          task.task_process_work();
          idle = false;
        }
      }

      return idle;
    }
  };

  /// Typedef for backwards compatibility with miss-spelt struct name.
  TYPHOON_DEPRECATED typedef scheduler_policy_sequential_multiple scheduler_policy_sequencial_multiple;

  //***************************************************************************
  /// Highest Priority.
  /// A policy the scheduler can use to decide what to do next.
  /// Calls the highest priority task that has work.
  //***************************************************************************
  struct scheduler_policy_highest_priority
  {
    bool schedule_tasks(tphn::ivector<tphn::task*>& task_list)
    {
      bool idle = true;

      size_t index = 0UL;
      while (index < task_list.size())
      {
        tphn::task& task = *(task_list[index]);

        if (task.task_request_work() > 0)
        {
          task.task_process_work();
          idle = false;
          break;
        }
        else
        {
          ++index;
        }
      }

      return idle;
    }
  };

  //***************************************************************************
  /// Most Work.
  /// A policy the scheduler can use to decide what to do next.
  /// Calls the task that has the most work.
  /// Starts looking from the task with the highest priority.
  //***************************************************************************
  struct scheduler_policy_most_work
  {
    bool schedule_tasks(tphn::ivector<tphn::task*>& task_list)
    {
      bool idle = true;

      size_t most_index = 0UL;
      uint32_t most_work = 0UL;

      for (size_t index = 0UL; index < task_list.size(); ++index)
      {
        tphn::task& task = *(task_list[index]);

        uint32_t n_work = task.task_request_work();

        if (n_work > most_work)
        {
          most_index = index;
          most_work = n_work;
          idle = false;
        }
      }

      if (!idle)
      {
        task_list[most_index]->task_process_work();
      }

      return idle;
    }
  };

  //***************************************************************************
  /// Scheduler base.
  //***************************************************************************
  class ischeduler
  {
  public:

    //*******************************************
    // Virtuals.
    //*******************************************
    virtual void start() = 0;

    virtual ~ischeduler()
    {
    }

    //*******************************************
    /// Set the idle callback.
    //*******************************************
    void set_idle_callback(tphn::ifunction<void>& callback)
    {
      p_idle_callback = &callback;
    }

    //*******************************************
    /// Set the watchdog callback.
    //*******************************************
    void set_watchdog_callback(tphn::ifunction<void>& callback)
    {
      p_watchdog_callback = &callback;
    }

    //*******************************************
    /// Set the running state for the scheduler.
    //*******************************************
    void set_scheduler_running(bool scheduler_running_)
    {
      scheduler_running = scheduler_running_;
    }

    //*******************************************
    /// Get the running state for the scheduler.
    //*******************************************
    bool scheduler_is_running() const
    {
      return scheduler_running;
    }

    //*******************************************
    /// Force the scheduler to exit.
    //*******************************************
    void exit_scheduler()
    {
      scheduler_exit = true;
    }

    //*******************************************
    /// Add a task.
    /// Add to the task list in priority order.
    //*******************************************
    void add_task(tphn::task& task)
    {
      TYPHOON_ASSERT(!task_list.full(), TYPHOON_ERROR(tphn::scheduler_too_many_tasks_exception));

      if (!task_list.full())
      {
        typename task_list_t::iterator itask = tphn::upper_bound(task_list.begin(),
                                                                   task_list.end(),
                                                                   task.get_task_priority(),
                                                                   compare_priority());

        task_list.insert(itask, &task);

        task.on_task_added();
      }
    }

    //*******************************************
    /// Add a task list.
    /// Adds to the tasks to the internal task list in priority order.
    /// Input order is ignored.
    //*******************************************
    template <typename TSize>
    void add_task_list(tphn::task** p_tasks, TSize size)
    {
      for (TSize i = 0; i < size; ++i)
      {
        TYPHOON_ASSERT((p_tasks[i] != TYPHOON_NULLPTR), TYPHOON_ERROR(tphn::scheduler_null_task_exception));
        add_task(*(p_tasks[i]));
        p_tasks[i]->on_task_added();
      }
    }

  protected:

    //*******************************************
    /// Constructor.
    //*******************************************
    ischeduler(tphn::ivector<tphn::task*>& task_list_)
      : scheduler_running(false),
        scheduler_exit(false),
        p_idle_callback(TYPHOON_NULLPTR),
        p_watchdog_callback(TYPHOON_NULLPTR),
        task_list(task_list_)
    {
    }

    bool scheduler_running;
    bool scheduler_exit;
    tphn::ifunction<void>* p_idle_callback;
    tphn::ifunction<void>* p_watchdog_callback;

  private:

    //*******************************************
    // Used to order tasks in descending priority.
    //*******************************************
    struct compare_priority
    {
      bool operator()(tphn::task_priority_t priority, tphn::task* ptask) const
      {
        return priority > ptask->get_task_priority();
      }
    };

    typedef tphn::ivector<tphn::task*> task_list_t;
    task_list_t& task_list;
  };

  //***************************************************************************
  /// Scheduler.
  //***************************************************************************
  template <typename TSchedulerPolicy, size_t MAX_TASKS_>
  class scheduler : public tphn::ischeduler, protected TSchedulerPolicy
  {
  public:

    enum
    {
      MAX_TASKS = MAX_TASKS_,
    };

    scheduler()
      : ischeduler(task_list)
    {
    }

    //*******************************************
    /// Start the scheduler.
    //*******************************************
    void start()
    {
      TYPHOON_ASSERT(task_list.size() > 0, TYPHOON_ERROR(tphn::scheduler_no_tasks_exception));

      scheduler_running = true;

      while (!scheduler_exit)
      {
        if (scheduler_running)
        {
          bool idle = TSchedulerPolicy::schedule_tasks(task_list);

          if (p_watchdog_callback)
          {
            (*p_watchdog_callback)();
          }

          if (idle && p_idle_callback)
          {
            (*p_idle_callback)();
          }
        }
      }
    }

  private:

    typedef tphn::vector<tphn::task*, MAX_TASKS> task_list_t;
    task_list_t task_list;
  };
}

#endif
