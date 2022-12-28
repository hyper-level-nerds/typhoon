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

#ifndef TYPHOON_MESSAGE_HPP
#define TYPHOON_MESSAGE_HPP

#include "platform.hpp"
#include "error_handler.hpp"
#include "exception.hpp"
#include "message_types.hpp"
#include "type_traits.hpp"
#include "static_assert.hpp"

#include <stdint.h>

namespace tphn
{
  //***************************************************************************
  class message_exception : public tphn::exception
  {
  public:

    message_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  class unhandled_message_exception : public tphn::message_exception
  {
  public:

    unhandled_message_exception(string_type file_name_, numeric_type line_number_)
      : message_exception(TYPHOON_ERROR_TEXT("message:unknown", TYPHOON_MESSAGE_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  // Message interface.
  //***************************************************************************
  class imessage
  {
  public:

    virtual ~imessage()
    {
    }

    TYPHOON_NODISCARD virtual tphn::message_id_t get_message_id() const TYPHOON_NOEXCEPT = 0;
  };

  //***************************************************************************
  // Message type.
  //***************************************************************************
  template <tphn::message_id_t ID_, typename TParent = tphn::imessage>
  class message : public TParent
  {
    TYPHOON_STATIC_ASSERT((tphn::is_base_of<tphn::imessage, TParent>::value), "TParent is not derived from tphn::imessage");

  public:

    enum
    {
      ID = ID_
    };

    TYPHOON_NODISCARD tphn::message_id_t get_message_id() const TYPHOON_NOEXCEPT TYPHOON_OVERRIDE
    {
      return ID;
    }
  };
}

#endif
