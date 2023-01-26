//
// Copyright (c) 2016-2019 Jin (jaehwanspin@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/hyper-level-nerds/typhoon
//

#ifndef TYPHOON_THREAD_HPP
#define TYPHOON_THREAD_HPP

#include <typhoon/utility.hpp>
#include <typhoon/memory.hpp>

#include <zephyr/kernel.h>

namespace tpn {


class thread
{
private:
    using genuine_native_handle_type = k_thread;

public:
    using native_handle_type = k_tid_t;

    class id
    {
    private:
        using native_handle_type = k_tid_t;

    public:
        id() noexcept :
            id_(::k_current_get())
        {
        }

        explicit id(native_handle_type _id) noexcept :
            id_(_id)
        {
        }

        bool operator==(id rhs) noexcept
        { return this->id_ == rhs.id_; }

        bool operator!=(id rhs) noexcept
        { return this->id_ != rhs.id_; }

        bool operator<(id rhs) noexcept
        { return this->id_ < rhs.id_; }

        bool operator<=(id rhs) noexcept
        { return this->id_ <= rhs.id_; }

        bool operator>(id rhs) noexcept
        { return this->id_ > rhs.id_; }

        bool operator>=(id rhs) noexcept
        { return this->id_ >= rhs.id_; }

        native_handle_type id_;
    };

    thread() noexcept = default;

    thread(thread&& other) noexcept
    { constexpr auto asdf = sizeof(genuine_native_handle_type); }

    
};

bool operator==(thread::id lhs, thread::id rhs) noexcept
{ return lhs.id_ == rhs.id_; }

bool operator!=(thread::id lhs, thread::id rhs) noexcept
{ return lhs.id_ != rhs.id_; }

bool operator<(thread::id lhs, thread::id rhs) noexcept
{ return lhs.id_ < rhs.id_; }

bool operator<=(thread::id lhs, thread::id rhs) noexcept
{ return lhs.id_ <= rhs.id_; }

bool operator>(thread::id lhs, thread::id rhs) noexcept
{ return lhs.id_ > rhs.id_; }

bool operator>=(thread::id lhs, thread::id rhs) noexcept
{ return lhs.id_ >= rhs.id_; }

namespace this_thread {

    thread::id get_id() noexcept
    {
        return thread::id(::k_current_get());
    }

}

}

#endif