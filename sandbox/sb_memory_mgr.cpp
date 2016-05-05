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
#include <tlsf.h>

namespace Sandbox {
    
    
    
    
    MemoryMgr::MemoryMgr() : m_tlsf(0){
       
        m_tlsf = tlsf_create_with_pool(m_initial,INITIAL_SIZE);
#ifdef SB_MEMORY_MGR_STAT
        m_total_allocated = 0;
#endif
        
    }
    
    
    MemoryMgr::~MemoryMgr() {
#ifdef SB_MEMORY_MGR_STAT
        LogInfo() << "==== memory mgr statistics ====";
        LogInfo() << "total allocated: " << format_memory(GHL::UInt32(m_total_allocated));
        LogInfo() << "pools: " << m_pools.size();
#endif
        for (size_t i=0;i<m_pools.size();++i) {
            tlsf_remove_pool(m_tlsf, m_pools[i]->pool);
            delete m_pools[i];
        }
        tlsf_destroy(m_tlsf);
    }
    
    void MemoryMgr::append_pool() {
        pool_block_t* block = new pool_block_t();
        block->pool = tlsf_add_pool(m_tlsf, block->mem, POOL_SIZE);
        m_pools.push_back(block);
    }
    
    GHL::Byte* MemoryMgr::alloc( size_t size ) {
#ifdef SB_MEMORY_MGR_STAT
        m_total_allocated+=size;
#endif
        void* data = tlsf_malloc(m_tlsf,size);
        if (!data) {
            append_pool();
            data = tlsf_malloc(m_tlsf,size);
        }
        return static_cast<GHL::Byte*>(data);
    }
    void MemoryMgr::free( GHL::Byte* ptr , size_t size) {
        tlsf_free(m_tlsf, ptr);
    }
    
    GHL::Byte* MemoryMgr::realloc( GHL::Byte* ptr, size_t size,size_t nsize ) {
#ifdef SB_MEMORY_MGR_STAT
        m_total_allocated+=nsize;
#endif
        void* data = tlsf_realloc(m_tlsf,ptr,nsize);
        if (!data) {
            append_pool();
            data = tlsf_realloc(m_tlsf,ptr,nsize);
        }
        return static_cast<GHL::Byte*>(data);
    }
    
    GHL::UInt32 MemoryMgr::allocated() const {
        return INITIAL_SIZE + m_pools.size() * POOL_SIZE;
    }
}
