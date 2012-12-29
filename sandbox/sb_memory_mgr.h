//
//  sb_memory_mgr.h
//  backgammon-osx
//
//  Created by Andrey Kunitsyn on 12/21/12.
//  Copyright (c) 2012 Andrey Kunitsyn. All rights reserved.
//

#ifndef __backgammon_osx__sb_memory_mgr__
#define __backgammon_osx__sb_memory_mgr__

#include "sb_notcopyable.h"
#include "sb_assert.h"
#include <ghl_types.h>
#include <cstring>

#define SB_MEMORY_MGR_STAT

namespace Sandbox {

    class MemoryMgr : public NotCopyable {
    private:
        static const size_t PAGE_SIZE = 1024*8;
        static const size_t ALIGN = 16;
        struct block_pool_base {
            size_t   first_free;
            block_pool_base*    next;
            virtual GHL::Byte* alloc() = 0;
            virtual bool free(GHL::Byte* ptr) = 0;
            static const size_t NO_FREE = -1;
#ifdef SB_MEMORY_MGR_STAT
            size_t  allocated;
#endif
            block_pool_base() : first_free(0),next(0) {
#ifdef SB_MEMORY_MGR_STAT
                allocated = false;
#endif
            }
            virtual ~block_pool_base() {
                delete next;
            }
        };
        template <size_t size> class block_pool : public block_pool_base{
        private:
            static const size_t block_size = size;
            static const size_t blocks_amount = (PAGE_SIZE-ALIGN-sizeof(block_pool_base))/block_size;
            typedef GHL::Byte block_t[block_size];
            block_t m_blocks[blocks_amount];
        public:
            block_pool() {
                for (size_t i=0;i<blocks_amount;++i) {
                    *reinterpret_cast<size_t*>(m_blocks[i]) = i+1;
                }
                *reinterpret_cast<size_t*>(m_blocks[blocks_amount-1]) = NO_FREE;
            }
            virtual GHL::Byte* alloc() {
                if (first_free!=NO_FREE) {
                    GHL::Byte* res = m_blocks[first_free];
                    first_free = *reinterpret_cast<size_t*>(res);
#ifdef SB_MEMORY_MGR_STAT
                    ++allocated;
#endif
                    return res;
                }
                if (!next) 
                    next = new block_pool();
                return next->alloc();
            }
            virtual bool free(GHL::Byte* ptr) {
                if ( ptr>= m_blocks[0] && ptr< (m_blocks+blocks_amount)[0] ) {
                    size_t block = (ptr-m_blocks[0])/block_size;
                    *reinterpret_cast<size_t*>(m_blocks[block]) = first_free;
                    first_free = block;
                    return true;
                }
                if (next) return next->free(ptr);
                return false;
            }
        };
        /// 8 16 32 64 128 256
        static const size_t blocks_allocators = 6;
        block_pool_base*    m_block_pools[blocks_allocators];
        struct page {
            page*   next;
            size_t  size;
            void*   free;
        };
        page*   m_first_page;
        static const size_t MAX_ALLOC = PAGE_SIZE - sizeof(page) - ALIGN;
#ifdef SB_MEMORY_MGR_STAT
        size_t m_total_allocated;
#endif
    public:
        MemoryMgr();
        ~MemoryMgr();
        GHL::Byte* alloc( size_t size );
        void free( GHL::Byte* ptr );
        GHL::UInt32 allocated() const;
    };
    
}

#endif /* defined(__backgammon_osx__sb_memory_mgr__) */
