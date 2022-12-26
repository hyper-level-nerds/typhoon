///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/TYPHOONCPP/tphn
https://www.tphncpp.com

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

#ifndef TYPHOON_FIXED_MEMORY_BLOCK_POOL_INCLUDED
#define TYPHOON_FIXED_MEMORY_BLOCK_POOL_INCLUDED

#include "platform.h"
#include "imemory_block_allocator.h"
#include "generic_pool.h"
#include "alignment.h"

namespace tphn
{
  //*************************************************************************
  /// The fixed sized memory block pool.
  /// The allocated memory blocks are all the same size.
  //*************************************************************************
  template <size_t VBlock_Size, size_t VAlignment, size_t VSize>
  class fixed_sized_memory_block_allocator : public imemory_block_allocator
  {
  public:
    static TYPHOON_CONSTANT size_t Block_Size = VBlock_Size;
    static TYPHOON_CONSTANT size_t Alignment  = VAlignment;
    static TYPHOON_CONSTANT size_t Size       = VSize;

    //*************************************************************************
    /// Default constructor
    //*************************************************************************
    fixed_sized_memory_block_allocator()
    {
    }

  private:

    /// A structure that has the size Block_Size.
    struct block
    {
      char data[Block_Size];
    };

    //*************************************************************************
    /// The overridden virtual function to allocate a block.
    //*************************************************************************
    virtual void* allocate_block(size_t required_size, size_t required_alignment) TYPHOON_OVERRIDE
    {
      if ((required_alignment <= Alignment) &&
          (required_size <= Block_Size) && 
          !pool.full())
      {
        return  pool.template allocate<block>();
      }
      else
      {
        return TYPHOON_NULLPTR;
      }
    }

    //*************************************************************************
    /// The overridden virtual function to release a block.
    //*************************************************************************
    virtual bool release_block(const void* const pblock) TYPHOON_OVERRIDE
    {
      if (pool.is_in_pool(pblock))
      {
        pool.release(static_cast<const block* const>(pblock));
        return true;
      }
      else
      {
        return false;
      }
    }

    //*************************************************************************
    /// Returns true if the allocator is the owner of the block.
    //*************************************************************************
    virtual bool is_owner_of_block(const void* const pblock) const TYPHOON_OVERRIDE
    {
      return pool.is_in_pool(pblock);
    }

    /// The generic pool from which allocate memory blocks.
    tphn::generic_pool<Block_Size, Alignment, Size> pool;
  };
}

#endif