//
//  sb_memory_mgr.cpp
//  backgammon-osx
//
//  Created by Andrey Kunitsyn on 12/21/12.
//  Copyright (c) 2012 Andrey Kunitsyn. All rights reserved.
//

#include "sb_memory_mgr.h"
#include "sb_math.h"
#include "sb_log.h"

namespace Sandbox {
    
    
    MemoryMgr::MemoryMgr() {
        m_block_pools[0] = new block_pool<16,32>();
        m_block_pools[1] = new block_pool<32,128>();
        m_block_pools[2] = new block_pool<64,1024>();
        m_block_pools[3] = new block_pool<128,1024>();
        m_block_pools[4] = new block_pool<256,1024>();
        m_block_pools[5] = new block_pool<512,1024>();
        
#ifdef SB_MEMORY_MGR_STAT
        m_total_allocated = 0;
#endif
    }
    
    MemoryMgr::block_pool_base* MemoryMgr::pool_for_size(size_t size) {
        if (size<=512) {
            if (size<=16) return m_block_pools[0];
            if (size<=32) return m_block_pools[1];
            if (size<=64) return m_block_pools[2];
            if (size<=128) return m_block_pools[3];
            if (size<=256) return m_block_pools[4];
            return m_block_pools[5];
        }
        return 0;
    }
    
    MemoryMgr::~MemoryMgr() {
#ifdef SB_MEMORY_MGR_STAT
        LogInfo() << "==== memory mgr statistics ====";
        LogInfo() << "total allocated: " << format_memory(GHL::UInt32(m_total_allocated));
        static const size_t block_sizes[blocks_allocators] = {
            16,32,64,128,256,512
        };

#endif
        for (size_t i=0;i<blocks_allocators;++i) {
#ifdef SB_MEMORY_MGR_STAT
            size_t total = 0;
            size_t pages = 0;
            block_pool_base* bp = m_block_pools[i];
            while (bp) {
                total += bp->allocated;
                bp = bp->next;
                ++pages;
            }

            LogInfo() << "block " << block_sizes[i] << "b allocated:" << format_memory(GHL::UInt32(total*block_sizes[i])) << " in " << pages << " pages(" << format_memory(GHL::UInt32(m_block_pools[i]->page_size*pages))<<")";
#endif            
            delete m_block_pools[i];
        }
    }
    
    GHL::Byte* MemoryMgr::alloc( size_t size ) {
#ifdef SB_MEMORY_MGR_STAT
        m_total_allocated+=size;
#endif
        block_pool_base* pool = pool_for_size(size);
        if (pool) {
            return pool->alloc();
        }
        return new GHL::Byte[ size ];
    }
    void MemoryMgr::free( GHL::Byte* ptr , size_t size) {
        block_pool_base* pool = pool_for_size(size);
        if (pool) {
            pool->free(ptr);
            return;
        }
        delete [] ptr;
    }
    
    GHL::Byte* MemoryMgr::realloc( GHL::Byte* ptr, size_t size,size_t nsize ) {
        block_pool_base* pool = pool_for_size(size); {
            if (pool) {
                if (pool->realloc(ptr, nsize))
                    return ptr;
            }
        }
        return 0;
    }
    
    GHL::UInt32 MemoryMgr::allocated() const {
        GHL::UInt32 res = 0;
        for (size_t i=0;i<blocks_allocators;++i) {
            block_pool_base* bp = m_block_pools[i];
            while (bp) {
                res+=bp->page_size;
                bp = bp->next;
            }
        }
        return res;
    }
}
