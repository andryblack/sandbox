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
#include <sbstd/sb_assert.h>
#include <ghl_types.h>
#include <cstring>

#ifdef SB_DEBUG
#ifndef SB_SILENT
#define SB_MEMORY_MGR_STAT
#endif
#endif

namespace Sandbox {

    class MemoryMgr : public NotCopyable {
    private:
        struct block_pool_base {
            size_t   first_free;
            block_pool_base*    next;
            virtual GHL::Byte* alloc() = 0;
            virtual bool realloc(GHL::Byte* ptr,size_t nsize) = 0;
            virtual bool free(GHL::Byte* ptr) = 0;
            static const size_t NO_FREE = -1;
            size_t  page_size;
#ifdef SB_MEMORY_MGR_STAT
            size_t  allocated;
#endif
            block_pool_base() : first_free(0),next(0) {
#ifdef SB_MEMORY_MGR_STAT
                allocated = 0;
#endif
            }
            virtual ~block_pool_base() {
                delete next;
            }
        };
        template <size_t size,size_t kb> class block_pool : public block_pool_base{
        private:
            static const size_t MEM_PAGE_SIZE = 1024*kb;
            static const size_t MEM_ALIGN = 16;
            
            static const size_t block_size = size;
            static const size_t blocks_amount = (MEM_PAGE_SIZE-MEM_ALIGN-sizeof(block_pool_base))/block_size;
            typedef GHL::Byte block_t[block_size];
            block_t m_blocks[blocks_amount];
        public:
            block_pool() {
                page_size = MEM_PAGE_SIZE;
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
            virtual bool realloc(GHL::Byte* ptr,size_t nsize) {
                if ( ptr>= m_blocks[0] && ptr< (m_blocks+blocks_amount)[0] ) {
                    if (nsize <= block_size)
                        return true;
                    return false;
                }
                if (next) return next->realloc(ptr,nsize);
                return false;
            }
        };
        /// 16 32 64 128 256 512
        static const size_t blocks_allocators = 6;
        block_pool_base*    m_block_pools[blocks_allocators];
        
#ifdef SB_MEMORY_MGR_STAT
        size_t m_total_allocated;
#endif
        block_pool_base* pool_for_size(size_t size);
    public:
        MemoryMgr();
        ~MemoryMgr();
        GHL::Byte* alloc( size_t size );
        void free( GHL::Byte* ptr , size_t size );
        GHL::Byte* realloc( GHL::Byte* ptr, size_t size, size_t nsize );
        GHL::UInt32 allocated() const;
    };
    
}

#endif /* defined(__backgammon_osx__sb_memory_mgr__) */
