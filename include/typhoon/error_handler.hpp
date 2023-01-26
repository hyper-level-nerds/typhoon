
///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tpn
https://www.tpncpp.com

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

#ifndef TYPHOON_ERROR_HANDLER_HPP
#define TYPHOON_ERROR_HANDLER_HPP

///\defgroup error_handler error_handler
/// Error handler for when throwing exceptions is not required.
///\ingroup utilities

#include "platform.hpp"
#include "exception.hpp"
#include "function.hpp"
#include "nullptr.hpp"

#include <assert.h>

#if defined(TYPHOON_LOG_ERRORS) || defined(TYPHOON_IN_UNIT_TEST)
namespace tpn
{
  //***************************************************************************
  /// Error handler for when throwing exceptions is not required.
  ///\ingroup error_handler
  //***************************************************************************
  class error_handler
  {
  public:

    //*************************************************************************
    /// Callback class for free handler functions. Deprecated.
    //*************************************************************************
    struct free_function : public tpn::function<void, const tpn::exception&>
    {
      explicit free_function(void (*p_function_)(const tpn::exception&))
        : tpn::function<void, const tpn::exception&>(p_function_)
      {
      }
    };

    //*************************************************************************
    /// Callback class for member handler functions. Deprecated.
    //*************************************************************************
    template <typename TObject>
    struct member_function : public tpn::function<TObject, const tpn::exception&>
    {
      member_function(TObject& object_, void(TObject::*p_function_)(const tpn::exception&))
        : tpn::function<TObject, const tpn::exception&>(object_, p_function_)
      {
      }
    };

    //*****************************************************************************
    /// Sets the error callback function. Deprecated.
    ///\param f A reference to an tpn::function object that will handler errors.
    //*****************************************************************************
    static void set_callback(ifunction<const tpn::exception&>& f)
    {
      create((void*)(&f), ifunction_stub);
    }

    //*************************************************************************
    /// Create from function (Compile time).
    //*************************************************************************
    template <void(*Method)(const tpn::exception&)>
    static void set_callback()
    {
      create(TYPHOON_NULLPTR, function_stub<Method>);
    }

    //*************************************************************************
    /// Create from instance method (Run time).
    //*************************************************************************
    template <typename T, void(T::* Method)(const tpn::exception&)>
    static void set_callback(T& instance)
    {
      create((void*)(&instance), method_stub<T, Method>);
    }

    //*************************************************************************
    /// Create from const instance method (Run time).
    //*************************************************************************
    template <typename T, void(T::* Method)(const tpn::exception&) const>
    static void set_callback(const T& instance)
    {
      create((void*)(&instance), const_method_stub<T, Method>);
    }

    //*************************************************************************
    /// Create from instance method (Compile time).
    //*************************************************************************
    template <typename T, T& Instance, void(T::* Method)(const tpn::exception&)>
    static void set_callback()
    {
      create(method_instance_stub<T, Instance, Method>);
    }

    //*************************************************************************
    /// Create from const instance method (Compile time).
    //*************************************************************************
    template <typename T, T const& Instance, void(T::* Method)(const tpn::exception&) const>
    static void set_callback()
    {
      create(const_method_instance_stub<T, Instance, Method>);
    }

    //*****************************************************************************
    /// Sends the exception error to the user's handler function.
    ///\param e The exception error.
    //*****************************************************************************
    static void error(const tpn::exception& e)
    {
      invocation_element& invocation = get_invocation_element();

      if (invocation.stub != TYPHOON_NULLPTR)
      {
        (*invocation.stub)(invocation.object, e);
      }
    }

  private:

    typedef void(*stub_type)(void* object, const tpn::exception&);

    //*************************************************************************
    /// The internal invocation object.
    //*************************************************************************
    struct invocation_element
    {
      //***********************************************************************
      invocation_element()
        : object(TYPHOON_NULLPTR)
        , stub(TYPHOON_NULLPTR)
      {
      }

      //***********************************************************************
      void* object;
      stub_type stub;
    };

    //*************************************************************************
    /// Returns the static invocation element.
    //*************************************************************************
    static invocation_element& get_invocation_element()
    {
      static invocation_element invocation;

      return invocation;
    }

    //*************************************************************************
    /// Constructs a callback from an object and stub.
    //*************************************************************************
    static void create(void* object, stub_type stub)
    {
      invocation_element& invocation = get_invocation_element();

      invocation.object = object;
      invocation.stub   = stub;
    }

    //*************************************************************************
    /// Constructs a callback from a stub.
    //*************************************************************************
    static void create(stub_type stub)
    {
      invocation_element& invocation = get_invocation_element();

      invocation.object = TYPHOON_NULLPTR;
      invocation.stub   = stub;
    }

    //*************************************************************************
    /// Stub call for a member function. Run time instance.
    //*************************************************************************
    template <typename T, void(T::* Method)(const tpn::exception&)>
    static void method_stub(void* object, const tpn::exception& e)
    {
      T* p = static_cast<T*>(object);
      return (p->*Method)(e);
    }

    //*************************************************************************
    /// Stub call for a const member function. Run time instance.
    //*************************************************************************
    template <typename T, void(T::* Method)(const tpn::exception&) const>
    static void const_method_stub(void* object, const tpn::exception& e)
    {
      T* const p = static_cast<T*>(object);
      return (p->*Method)(e);
    }

    //*************************************************************************
    /// Stub call for a member function. Compile time instance.
    //*************************************************************************
    template <typename T, T& Instance, void(T::* Method)(const tpn::exception&)>
    static void method_instance_stub(void*, const tpn::exception& e)
    {
      return (Instance.*Method)(e);
    }

    //*************************************************************************
    /// Stub call for a const member function. Compile time instance.
    //*************************************************************************
    template <typename T, const T& Instance, void(T::* Method)(const tpn::exception&) const>
    static void const_method_instance_stub(void*, const tpn::exception& e)
    {
      (Instance.*Method)(e);
    }

    //*************************************************************************
    /// Stub call for a free function.
    //*************************************************************************
    template <void(*Method)(const tpn::exception&)>
    static void function_stub(void*, const tpn::exception& e)
    {
      (Method)(e);
    }

    //*************************************************************************
    /// Stub call for a ifunction. Run time instance.
    //*************************************************************************
    static void ifunction_stub(void* object, const tpn::exception& e)
    {
      tpn::ifunction<const tpn::exception&>* p = static_cast<tpn::ifunction<const tpn::exception&>*>(object);
      p->operator()(e);
    }
  };
}
#endif

//***************************************************************************
/// Asserts a condition.
/// Versions of the macro that return a constant value of 'true' will allow the compiler to optimise away
/// any 'if' statements that it is contained within.
/// If TYPHOON_NO_CHECKS is defined then no runtime checks are executed at all.
/// If asserts or exceptions are enabled then the error is thrown if the assert fails. The return value is always 'true'.
/// If TYPHOON_LOG_ERRORS is defined then the error is logged if the assert fails. The return value is the value of the boolean test.
/// Otherwise 'assert' is called. The return value is always 'true'.
///\ingroup error_handler
//***************************************************************************
#if defined(TYPHOON_NO_CHECKS)
  #define TYPHOON_ASSERT(b, e)                       // Does nothing.
  #define TYPHOON_ASSERT_AND_RETURN(b, e)            // Does nothing.
  #define TYPHOON_ASSERT_AND_RETURN_VALUE(b, e, v)   // Does nothing.
  
  #define TYPHOON_ASSERT_FAIL(e)                     // Does nothing.
  #define TYPHOON_ASSERT_FAIL_AND_RETURN(e)          // Does nothing.
  #define TYPHOON_ASSERT_FAIL_AND_RETURN_VALUE(e, v) // Does nothing.
#elif TYPHOON_USING_EXCEPTIONS
  #if defined(TYPHOON_LOG_ERRORS)
    #define TYPHOON_ASSERT(b, e) {if (!(b)) {tpn::error_handler::error((e)); throw((e));}}                                // If the condition fails, calls the error handler then throws an exception.
    #define TYPHOON_ASSERT_AND_RETURN(b, e) {if (!(b)) {tpn::error_handler::error((e)); throw((e)); return;}}             // If the condition fails, calls the error handler then throws an exception.
    #define TYPHOON_ASSERT_AND_RETURN_VALUE(b, e, v) {if (!(b)) {tpn::error_handler::error((e)); throw((e)); return(v);}} // If the condition fails, calls the error handler then throws an exception.
    
    #define TYPHOON_ASSERT_FAIL(e) {tpn::error_handler::error((e)); throw((e));}                                          // Calls the error handler then throws an exception.
    #define TYPHOON_ASSERT_FAIL_AND_RETURN(e) {tpn::error_handler::error((e)); throw((e)); return;}                       // Calls the error handler then throws an exception.
    #define TYPHOON_ASSERT_FAIL_AND_RETURN_VALUE(e, v) {tpn::error_handler::error((e)); throw((e)); return(v);}              // Calls the error handler then throws an exception.
  #else
    #define TYPHOON_ASSERT(b, e) {if (!(b)) {throw((e));}}                                // If the condition fails, throws an exception.
    #define TYPHOON_ASSERT_AND_RETURN(b, e) {if (!(b)) {throw((e)); return;}}             // If the condition fails, throws an exception.
    #define TYPHOON_ASSERT_AND_RETURN_VALUE(b, e, v) {if (!(b)) {throw((e)); return(v);}} // If the condition fails, throws an exception.
    
    #define TYPHOON_ASSERT_FAIL(e) {throw((e));}                                          // Throws an exception.
    #define TYPHOON_ASSERT_FAIL_AND_RETURN(e) {throw((e)); return;}                       // Throws an exception.
    #define TYPHOON_ASSERT_FAIL_AND_RETURN_VALUE(e, v) {throw((e)); return(v);}           // Throws an exception.

  #endif
#else
  #if defined(TYPHOON_LOG_ERRORS)
    #define TYPHOON_ASSERT(b, e) {if(!(b)) {tpn::error_handler::error((e));}}                                 // If the condition fails, calls the error handler
    #define TYPHOON_ASSERT_AND_RETURN(b, e) {if(!(b)) {tpn::error_handler::error((e)); return;}}              // If the condition fails, calls the error handler and return
    #define TYPHOON_ASSERT_AND_RETURN_VALUE(b, e, v) {if(!(b)) {tpn::error_handler::error((e)); return (v);}} // If the condition fails, calls the error handler and return a value
    
    #define TYPHOON_ASSERT_FAIL(e) {tpn::error_handler::error((e));}                                          // Calls the error handler
    #define TYPHOON_ASSERT_FAIL_AND_RETURN(e) {tpn::error_handler::error((e)); return;}                       // Calls the error handler and return
    #define TYPHOON_ASSERT_FAIL_AND_RETURN_VALUE(e, v) {tpn::error_handler::error((e)); return (v);}          // Calls the error handler and return a value
  #else
    #if TYPHOON_IS_DEBUG_BUILD
      #define TYPHOON_ASSERT(b, e) assert((b))                                                                // If the condition fails, asserts.
      #define TYPHOON_ASSERT_AND_RETURN(b, e) {if (!(b)) {assert(false); return;}}                            // If the condition fails, asserts and return.
      #define TYPHOON_ASSERT_AND_RETURN_VALUE(b, e, v) {if (!(b)) {assert(false); return(v);}}                // If the condition fails, asserts and return a value.
    
      #define TYPHOON_ASSERT_FAIL(e) assert(false)                                    // Asserts.
      #define TYPHOON_ASSERT_FAIL_AND_RETURN(e) {assert(false);  return;}             // Asserts.
      #define TYPHOON_ASSERT_FAIL_AND_RETURN_VALUE(e, v) {assert(false);  return(v);} // Asserts.
    #else
      #define TYPHOON_ASSERT(b, e)                                                    // Does nothing.
      #define TYPHOON_ASSERT_AND_RETURN(b, e) {if (!(b)) return;}                     // Returns.
      #define TYPHOON_ASSERT_AND_RETURN_VALUE(b, e, v) {if (!(b)) return(v);}         // Returns a value.
      
      #define TYPHOON_ASSERT_FAIL(e)                                                  // Does nothing.
      #define TYPHOON_ASSERT_FAIL_AND_RETURN(e) {return;}                             // Returns.
      #define TYPHOON_ASSERT_FAIL_AND_RETURN_VALUE(e, v) {return(v);}                 // Returns a value.
    #endif
  #endif
#endif

#if defined(TYPHOON_VERBOSE_ERRORS)
  #define TYPHOON_ERROR(e) (e(__FILE__, __LINE__))                    // Make an exception with the file name and line number.
  #define TYPHOON_ERROR_WITH_VALUE(e, v) (e(__FILE__, __LINE__, (v))) // Make an exception with the file name, line number and value.
#else
  #define TYPHOON_ERROR(e) (e("", __LINE__))                    // Make an exception with the line number.
  #define TYPHOON_ERROR_WITH_VALUE(e, v) (e("", __LINE__, (v))) // Make an exception with the file name, line number and value.
#endif

#if defined(TYPHOON_VERBOSE_ERRORS)
  #define TYPHOON_ERROR_TEXT(verbose_text, terse_text) (verbose_text) // Use the verbose text.
#else
  #define TYPHOON_ERROR_TEXT(verbose_text, terse_text) (terse_text)   // Use the terse text.
#endif

#endif

