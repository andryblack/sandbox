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
        m_block_pools[0] = new block_pool<8>();
        m_block_pools[1] = new block_pool<16>();
        m_block_pools[2] = new block_pool<32>();
        m_block_pools[3] = new block_pool<64>();
        m_block_pools[4] = new block_pool<128>();
        m_block_pools[5] = new block_pool<256>();
        m_first_page = 0;
#ifdef SB_MEMORY_MGR_STAT
        m_total_allocated = 0;
#endif
    }
    
    MemoryMgr::~MemoryMgr() {
#ifdef SB_MEMORY_MGR_STAT
        LogInfo() << "==== memory mgr statistics ====";
        LogInfo() << "total allocated: " << format_memory(GHL::UInt32(m_total_allocated));
        static const size_t block_sizes[blocks_allocators] = {
            8,16,32,64,128,256
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

            LogInfo() << "block " << block_sizes[i] << "b allocated:" << format_memory(GHL::UInt32(total*block_sizes[i])) << " in " << pages << " pages(" << format_memory(GHL::UInt32(PAGE_SIZE*pages))<<")";
#endif            
            delete m_block_pools[i];
        }
        page* p = m_first_page;
        while (p) {
            page* n = p->next;
            delete [] reinterpret_cast<GHL::Byte*>(p);
            p = n;
        }
    }
    
    GHL::Byte* MemoryMgr::alloc( size_t size ) {
#ifdef SB_MEMORY_MGR_STAT
        m_total_allocated+=size;
#endif
        if (size<=256) {
            if (size<=8) return m_block_pools[0]->alloc();
            if (size<=16) return m_block_pools[1]->alloc();
            if (size<=32) return m_block_pools[2]->alloc();
            if (size<=64) return m_block_pools[3]->alloc();
            if (size<=128) return m_block_pools[4]->alloc();
            return m_block_pools[5]->alloc();
        } else if (size<MAX_ALLOC) {
            
        }
        return new GHL::Byte[ size ];
    }
    void MemoryMgr::free( GHL::Byte* ptr ) {
        for (size_t i=0;i<blocks_allocators;++i) {
            if (m_block_pools[i]->free(ptr)) return;
        }
        delete [] ptr;
    }
    
    GHL::UInt32 MemoryMgr::allocated() const {
        GHL::UInt32 res = 0;
        for (size_t i=0;i<blocks_allocators;++i) {
            block_pool_base* bp = m_block_pools[i];
            while (bp) {
                res+=PAGE_SIZE;
                bp = bp->next;
            }
        }
        return res;
    }
}
