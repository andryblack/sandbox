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

#include <stdint.h>

//#define SB_MEMORY_MGR_DEBUG

#ifdef SB_MEMORY_MGR_DEBUG
static const char mem_header[] = {0x90,0x0d,0xbe,0xe1};
static const char mem_footer[] = {0xbe,0xe1,0x90,0x0d};
static const char mem_header_b[] = {0xde,0xad,0xbe,0xef};
static const char mem_footer_b[] = {0xbe,0xef,0xde,0xad};

#define MEM_HEADER 8
#define MEM_FOOTER 4

static std::string dump_mem(const void * m) {
    const unsigned char* c = reinterpret_cast<const unsigned char*>(m);
    char buf[32];
    snprintf(buf, 32, "%02x %02x %02x %02x",c[0],c[1],c[2],c[3]);
    return buf;
}

#define MEM_INIT(d,s) ::memcpy(reinterpret_cast<char*>(d),mem_header,MEM_HEADER); \
    ::memcpy(reinterpret_cast<char*>(d)+s+MEM_HEADER,mem_footer,MEM_FOOTER); \
    LogInfo() << "inited: " << reinterpret_cast<void*>(d) << " " << s; \
    if (s==17) mem_probe = d;
#define MEM_CHECK(d,s) if(::memcmp(reinterpret_cast<char*>(d),mem_header,MEM_HEADER)!=0) {\
    ::Sandbox::LogError() << "invalid header " << reinterpret_cast<void*>(d) << " " <<dump_mem(d) << " " << s; \
    sb_assert(false); \
};\
if(::memcmp(reinterpret_cast<char*>(d)+s+MEM_HEADER,mem_footer,MEM_FOOTER) != 0) {\
    ::Sandbox::LogError() << "invalid footer " << reinterpret_cast<void*>(d) << " " \
    <<dump_mem(reinterpret_cast<char*>(d)+s+MEM_HEADER) << " " << s; \
    sb_assert(false); \
    };
#define MEM_RELEASE(d,s) \
    ::memcpy(reinterpret_cast<char*>(d),mem_header_b,MEM_HEADER); \
    ::memcpy(reinterpret_cast<char*>(d)+s+MEM_HEADER,mem_footer_b,MEM_FOOTER);

#else
#define MEM_HEADER 0
#define MEM_FOOTER 0
#define MEM_INIT(d,s)
#define MEM_RELEASE(d,s)
#define MEM_CHECK(d,s)
#endif
//
//#define ONLY_CHECK
//#define tlsf_malloc(a,s) ::malloc(s)
//#define tlsf_free(a,s) ::free(s)
//#define tlsf_realloc(a,p,s) ::realloc(p,s)

namespace Sandbox {
    
    
        static const size_t mem_align = 8;
    
    MemoryMgr::MemoryMgr() : m_tlsf(0){
        uintptr_t addr  = (uintptr_t)m_initial;
        size_t remove = 0;
        if (addr % mem_align != 0) {
            remove = mem_align - addr % mem_align;
        }
        m_tlsf = tlsf_create_with_pool(&m_initial[remove],INITIAL_SIZE-remove);
        sb_assert(m_tlsf);
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
            ::free(m_pools[i]);
        }
        tlsf_destroy(m_tlsf);
    }
    
    void MemoryMgr::append_pool() {
        LogInfo() << "append_pool";
        pool_block_t* block = static_cast<pool_block_t*>(::malloc(sizeof(pool_block_t)));
        if (!block) {
            LogError() << "failed allocate block";
            return;
        }
        uintptr_t addr  = (uintptr_t)block->mem;
        size_t remove = 0;
        if (addr % mem_align != 0) {
            remove = mem_align - addr % mem_align;
        }
        block->pool = tlsf_add_pool(m_tlsf, &block->mem[remove], POOL_SIZE-remove);
        m_pools.push_back(block);
    }
    
    GHL::Byte* MemoryMgr::alloc( size_t size ) {
#ifdef SB_MEMORY_MGR_STAT
        m_total_allocated+=size;
#endif
        void* data = tlsf_malloc(m_tlsf,size+MEM_HEADER+MEM_FOOTER);
        if (!data) {
            append_pool();
            data = tlsf_malloc(m_tlsf,size+MEM_HEADER+MEM_FOOTER);
            if (!data) return 0;
        }
        MEM_INIT(data,size)
        return static_cast<GHL::Byte*>(data)+MEM_HEADER;
    }
    void MemoryMgr::free( GHL::Byte* ptr , size_t size) {
        MEM_CHECK(ptr-MEM_HEADER,size)
        MEM_RELEASE(ptr-MEM_HEADER,size)
        tlsf_free(m_tlsf, ptr-MEM_HEADER);
    }
    
    GHL::Byte* MemoryMgr::realloc( GHL::Byte* ptr, size_t size,size_t nsize ) {
#ifdef SB_MEMORY_MGR_STAT
        m_total_allocated+=nsize;
#endif
        MEM_CHECK(ptr-MEM_HEADER,size)
        MEM_RELEASE(ptr-MEM_HEADER,size)
        void* data = tlsf_realloc(m_tlsf,ptr-MEM_HEADER,nsize+MEM_HEADER+MEM_FOOTER);
        if (!data) {
            append_pool();
            data = tlsf_realloc(m_tlsf,ptr-MEM_HEADER,nsize+MEM_HEADER+MEM_FOOTER);
            if (!data) return 0;
        }
        MEM_INIT(data,nsize)
        return static_cast<GHL::Byte*>(data)+MEM_HEADER;
    }
    
    GHL::UInt32 MemoryMgr::allocated() const {
        return GHL::UInt32(INITIAL_SIZE + m_pools.size() * POOL_SIZE);
    }
}
